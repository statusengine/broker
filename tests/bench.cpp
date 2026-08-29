/**
 * Micro benchmarks for the two paths that carry the message volume.
 *
 * naemon's own process_check_result() is stubbed out, so what is measured here is the
 * broker's share of the work, which is the only part we can do anything about.
 *
 * Build with --buildtype=release, the numbers are meaningless at -O0.
 */

#include <chrono>
#include <iconv.h>
#include <uchardet.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "test_support.h"

#include "MessageHandler/MessageHandler.h"
#include "NagiosObject.h"

using namespace statusengine;

namespace {

    /// MessageHandler is abstract; the benchmark only needs its ProcessMessage.
    class BenchHandler : public MessageHandler {
      public:
        explicit BenchHandler(IStatusengine *se) : MessageHandler(se) {}
        bool Connect() override {
            return true;
        }
        bool Worker(unsigned long &) override {
            return false;
        }
        void SendMessage(Queue, const std::string &) override {}
    };

    std::string ReadFixture(const std::string &name) {
        std::ifstream in(std::string(STATUSENGINE_FIXTURE_DIR) + "/" + name);
        std::stringstream buffer;
        buffer << in.rdbuf();
        return buffer.str();
    }

    /// Builds an OCSP worker payload with `count` service checks, the shape the broker
    /// receives from another instance: {"messages":[{...,"servicecheck":{...}}, ...]}.
    std::string BulkServiceChecks(size_t count) {
        std::string fixtureText = ReadFixture("statusngin_servicechecks.json");
        yyjson_doc *fixture = yyjson_read(fixtureText.c_str(), fixtureText.length(), 0);
        yyjson_val *messages = yyjson_obj_get(yyjson_doc_get_root(fixture), "messages");
        yyjson_val *one = yyjson_arr_get(messages, 0);

        yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
        yyjson_mut_val *root = yyjson_mut_obj(doc);
        yyjson_mut_doc_set_root(doc, root);
        yyjson_mut_val *arr = yyjson_mut_arr(doc);
        for (size_t i = 0; i < count; ++i) {
            yyjson_mut_arr_append(arr, yyjson_val_mut_copy(doc, one));
        }
        yyjson_mut_obj_add_val(doc, root, "messages", arr);

        char *out = yyjson_mut_write(doc, 0, nullptr);
        std::string result(out ? out : "");
        free(out);
        yyjson_mut_doc_free(doc);
        yyjson_doc_free(fixture);
        return result;
    }

    std::string SingleCommand() {
        return R"({"Command":"check_result","Data":{"host_name":"localhost",)"
               R"("service_description":"Uptime","output":"OK - up 3 days, load average 0.14",)"
               R"("long_output":"","perf_data":"uptime=3;;;0 load=0.14;;;0","check_type":1,)"
               R"("return_code":0,"start_time":1785470664,"end_time":1785470668,)"
               R"("early_timeout":0,"latency":0.6,"exited_ok":1}})";
    }

    long long sink = 0;

    template <typename F>
    void Bench(const char *name, unsigned long iterations, F body) {
        body(); // warm up caches and any one time allocation
        auto start = std::chrono::steady_clock::now();
        for (unsigned long i = 0; i < iterations; ++i) {
            body();
        }
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      std::chrono::steady_clock::now() - start)
                      .count();
        double perOp = static_cast<double>(ns) / static_cast<double>(iterations);
        std::cout << std::left << std::setw(38) << name << std::right << std::setw(12) << std::fixed
                  << std::setprecision(0) << perOp << " ns/op" << std::setw(14) << std::setprecision(0)
                  << (perOp > 0 ? 1e9 / perOp : 0) << " ops/s" << std::endl;
    }

} // namespace

int main() {
    FakeStatusengine se;
    BenchHandler handler(&se);

    const std::string ascii = "OK - up 3 days, load average 0.14, 4 users, all services responding";
    const std::string utf8 = "Größe des Puffers überschritten, Dienst läuft nicht mehr korrekt";
    std::string latin1;
    for (size_t i = 0; i < utf8.size();) {
        unsigned char c = static_cast<unsigned char>(utf8[i]);
        if (c < 0x80) { latin1 += static_cast<char>(c); i += 1; }
        else { latin1 += static_cast<char>(((c & 0x1fu) << 6) | (static_cast<unsigned char>(utf8[i + 1]) & 0x3fu)); i += 2; }
    }

    Encoder encoder;
    std::cout << "--- send path: charset handling ---" << std::endl;
    Bench("Encoder ascii", 200000, [&] { sink += encoder.ToUtf8(ascii.c_str()).size(); });
    Bench("Encoder utf8", 200000, [&] { sink += encoder.ToUtf8(utf8.c_str()).size(); });
    Bench("Encoder latin1", 200000, [&] { sink += encoder.ToUtf8(latin1.c_str()).size(); });

    // Split the encoder cost: how much is uchardet's detection, how much is the conversion?
    {
        uchardet_t uc = uchardet_new();
        Bench("  uchardet detect only ascii", 200000, [&] {
            uchardet_handle_data(uc, ascii.c_str(), ascii.size());
            uchardet_data_end(uc);
            sink += uchardet_get_charset(uc)[0];
            uchardet_reset(uc);
        });
        Bench("  uchardet detect only utf8", 200000, [&] {
            uchardet_handle_data(uc, utf8.c_str(), utf8.size());
            uchardet_data_end(uc);
            sink += uchardet_get_charset(uc)[0];
            uchardet_reset(uc);
        });
        uchardet_delete(uc);

        Bench("  iconv ASCII->UTF-8 only", 200000, [&] {
            iconv_t cd = iconv_open("UTF-8", "ASCII");
            std::string out(ascii.size() * 4, '\0');
            char *in = const_cast<char *>(ascii.c_str());
            size_t inLeft = ascii.size();
            char *outCur = &out[0];
            size_t outLeft = out.size();
            iconv(cd, &in, &inLeft, &outCur, &outLeft);
            iconv_close(cd);
            sink += outCur - &out[0];
        });
    }

    std::cout << "\n--- send path: whole message ---" << std::endl;
    service nagService;
    std::memset(&nagService, 0, sizeof(nagService));
    nagService.host_name = const_cast<char *>("localhost");
    nagService.description = const_cast<char *>("Uptime");
    nagService.check_command = const_cast<char *>("check_uptime");

    nebstruct_service_check_data scd;
    std::memset(&scd, 0, sizeof(scd));
    scd.type = NEBTYPE_SERVICECHECK_PROCESSED;
    scd.host_name = const_cast<char *>("localhost");
    scd.service_description = const_cast<char *>("Uptime");
    scd.object_ptr = &nagService;
    scd.output = const_cast<char *>(ascii.c_str());
    scd.long_output = const_cast<char *>("");
    scd.perf_data = const_cast<char *>("uptime=3;;;0 load=0.14;;;0");

    Bench("NagiosServiceCheckData build", 100000, [&] {
        NagiosServiceCheckData msg(&scd);
        sink += static_cast<long long>(msg.ToString().size());
    });

    std::cout << "\n--- receive path ---" << std::endl;
    const std::string single = SingleCommand();
    Bench("ProcessMessage command (1 result)", 100000,
          [&] { handler.ProcessMessage(WorkerQueue::Command, single); });

    const std::string bulk100 = BulkServiceChecks(100);
    Bench("ProcessMessage ocsp bulk (100)", 2000,
          [&] { handler.ProcessMessage(WorkerQueue::OCSP, bulk100); });
    // How much of that is parsing, which we cannot avoid?
    Bench("  yyjson_read only (bulk)", 2000, [&] {
        yyjson_doc *d = yyjson_read(bulk100.c_str(), bulk100.size(), 0);
        sink += static_cast<long long>(yyjson_obj_size(yyjson_doc_get_root(d)));
        yyjson_doc_free(d);
    });
    // And how much is the std::string copy of the payload that step 4 targets?
    Bench("  payload copy only (bulk)", 200000, [&] {
        std::string copy(bulk100.data(), bulk100.size());
        sink += static_cast<long long>(copy.size());
    });

    std::cout << "\n--- point 6: incidental costs ---" << std::endl;

    // The map copy that FlushBulkQueue does on every flush, only to log a queue name.
    Bench("QueueIds() copied (auto)", 200000, [&] {
        auto QueueId = QueueNameHandler::Instance().QueueIds();
        sink += static_cast<long long>(QueueId.size());
    });
    Bench("QueueIds() by reference", 200000, [&] {
        const auto &QueueId = QueueNameHandler::Instance().QueueIds();
        sink += static_cast<long long>(QueueId.size());
    });

    // LogStream formats into its stringstream before it knows whether the level keeps the
    // message, so a suppressed Info line still costs the full formatting.
    {
        LogStream discarding;
        discarding.SetLogLevel(LogLevel::Warning); // Info is dropped
        LogStream emitting;
        emitting.SetLogLevel(LogLevel::Info);
        Bench("Log line, level discards it", 200000, [&] {
            discarding << "Sent bulk message (" << 200ul << ") for queue "
                       << std::string("statusngin_servicechecks") << LogLevel::Info;
        });
        Bench("Log line, level emits it", 200000, [&] {
            capturedLogs.clear();
            emitting << "Sent bulk message (" << 200ul << ") for queue "
                     << std::string("statusngin_servicechecks") << LogLevel::Info;
        });
    }

    // The SPACED vs PLAIN comparison that used to live here measured a json-c
    // output option. yyjson always writes minified, so there is nothing to compare.

    std::cout << "\nbulk payload of 100: " << bulk100.size() << " bytes, "
              << "processed check results: " << processedCheckResults << std::endl;
    return sink == 0x7fffffff ? 1 : 0; // keep the sink alive
}

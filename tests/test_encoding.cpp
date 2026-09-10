#include <doctest/doctest.h>

#include <string>
#include <vector>

#include "Encoding.h"

using statusengine::Encoder;

namespace {
    /// The same text as UTF-8 and as single byte latin1, built byte-wise so that no
    /// greedy hex escape in a string literal can merge two bytes into one.
    const std::string kUtf8 =
        "Die Verbindung zum Server ist fehlgeschlagen. Bitte prüfen Sie die Konfiguration "
        "und überprüfen Sie, ob der Dienst läuft. Größe überschritten.";

    std::string ToLatin1(const std::string &utf8) {
        std::string out;
        for (size_t i = 0; i < utf8.size();) {
            unsigned char c = static_cast<unsigned char>(utf8[i]);
            if (c < 0x80) {
                out += static_cast<char>(c);
                i += 1;
            }
            else {
                unsigned cp = ((c & 0x1fu) << 6) | (static_cast<unsigned char>(utf8[i + 1]) & 0x3fu);
                out += static_cast<char>(cp);
                i += 2;
            }
        }
        return out;
    }
} // namespace

TEST_CASE("Encoder converts latin1 to utf8") {
    Encoder encoder;
    const std::string latin1 = ToLatin1(kUtf8);
    CHECK(latin1.size() < kUtf8.size()); // the fixture really is single byte

    CHECK(encoder.ToUtf8(latin1.c_str()) == kUtf8);
}

TEST_CASE("Encoder passes utf8 through unchanged") {
    Encoder encoder;
    CHECK(encoder.ToUtf8(kUtf8.c_str()) == kUtf8);
}

TEST_CASE("Encoder does not modify the caller's buffer") {
    Encoder encoder;
    const std::string latin1 = ToLatin1(kUtf8);
    std::vector<char> buffer(latin1.begin(), latin1.end());
    buffer.push_back('\0');

    encoder.ToUtf8(buffer.data());

    CHECK(std::string(buffer.data()) == latin1);
}

TEST_CASE("Encoder handles trivial input") {
    Encoder encoder;
    CHECK(encoder.ToUtf8(nullptr).empty());
    CHECK(encoder.ToUtf8("").empty());
    CHECK(encoder.ToUtf8("OK - everything fine") == "OK - everything fine");
}

TEST_CASE("Encoder can be reused across calls") {
    Encoder encoder;
    const std::string latin1 = ToLatin1(kUtf8);
    for (int i = 0; i < 5; ++i) {
        CHECK(encoder.ToUtf8(latin1.c_str()) == kUtf8);
        CHECK(encoder.ToUtf8("plain ascii") == "plain ascii");
    }
}

TEST_CASE("Encoder keeps embedded newlines and pipes") {
    Encoder encoder;
    CHECK(encoder.ToUtf8("first line\nsecond line|perf=1") == "first line\nsecond line|perf=1");
}

TEST_CASE("valid multi byte utf8 passes through untouched") {
    Encoder encoder;
    // two, three and four byte sequences
    const std::string samples[] = {"ü ö ä ß", "€ ✓ →", "\xf0\x9f\x91\x8d ok", "日本語のテキスト"};
    for (const auto &sample : samples) {
        CHECK(encoder.ToUtf8(sample.c_str()) == sample);
    }
}

TEST_CASE("malformed utf8 is not mistaken for valid") {
    // These must fall through to charset detection rather than being passed through as
    // "already utf8", otherwise broken bytes would reach the queue.
    Encoder encoder;

    SUBCASE("lone continuation byte") {
        const char input[] = "ok \x80 tail";
        CHECK(encoder.ToUtf8(input) != std::string(input));
    }
    SUBCASE("truncated two byte sequence at end") {
        const char input[] = "ok \xc3";
        CHECK(encoder.ToUtf8(input) != std::string(input));
    }
    SUBCASE("overlong encoding of '/'") {
        const char input[] = "ok \xc0\xaf tail";
        CHECK(encoder.ToUtf8(input) != std::string(input));
    }
    SUBCASE("surrogate half") {
        const char input[] = "ok \xed\xa0\x80 tail";
        CHECK(encoder.ToUtf8(input) != std::string(input));
    }
}

TEST_CASE("ascii output is returned byte for byte") {
    // ASCII is a subset of UTF-8, so the fast path returns it unchanged where the old code
    // ran it through an iconv ASCII->UTF-8 conversion. The bytes have to be identical.
    Encoder encoder;
    const std::string sample = "OK - up 3 days, load 0.14 | uptime=3;;;0 load=0.14;;;0";
    CHECK(encoder.ToUtf8(sample.c_str()) == sample);
}

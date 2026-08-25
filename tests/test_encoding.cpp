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

#include "Encoding.h"

#include <cstring>
#include <iconv.h>

#include "gsl.h"

namespace statusengine {

    namespace {
        /**
         * True if the bytes already are valid UTF-8. Pure ASCII is a subset of UTF-8, so
         * this one check covers both cases that need no conversion at all.
         *
         * Worth having because uchardet's statistical detection costs around 4us on a
         * typical plugin output, while this scan is a few nanoseconds per byte - and
         * virtually all real plugin output is ASCII or valid UTF-8 already.
         */
        bool IsValidUtf8(const char *data, size_t length) {
            const unsigned char *p = reinterpret_cast<const unsigned char *>(data);
            for (size_t i = 0; i < length;) {
                const unsigned char c = p[i];
                if (c < 0x80) {
                    i += 1;
                    continue;
                }

                size_t continuation;
                unsigned int codepoint;
                if ((c & 0xe0) == 0xc0) {
                    continuation = 1;
                    codepoint = c & 0x1fu;
                }
                else if ((c & 0xf0) == 0xe0) {
                    continuation = 2;
                    codepoint = c & 0x0fu;
                }
                else if ((c & 0xf8) == 0xf0) {
                    continuation = 3;
                    codepoint = c & 0x07u;
                }
                else {
                    return false; // continuation byte in leading position, or 5/6 byte form
                }

                if (i + continuation >= length) {
                    return false; // truncated sequence
                }
                for (size_t k = 1; k <= continuation; ++k) {
                    if ((p[i + k] & 0xc0) != 0x80) {
                        return false;
                    }
                    codepoint = (codepoint << 6) | (p[i + k] & 0x3fu);
                }

                // Reject the encodings a strict decoder rejects, so that anything this
                // function passes through really is well formed UTF-8.
                if ((continuation == 1 && codepoint < 0x80) || (continuation == 2 && codepoint < 0x800) ||
                    (continuation == 3 && codepoint < 0x10000)) {
                    return false; // overlong
                }
                if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                    return false; // out of range or surrogate half
                }

                i += continuation + 1;
            }
            return true;
        }
    } // namespace

    Encoder::Encoder() : uc(uchardet_new()), warn() {}

    Encoder::~Encoder() {
        uchardet_delete(uc);
        uc = nullptr;
    }

    void Encoder::SetWarnCallback(WarnCallback callback) {
        warn = std::move(callback);
    }

    std::string Encoder::ToUtf8(const char *inputData) {
        if (inputData == nullptr) {
            return std::string();
        }
        // we can't use strnlen here, we don't have any idea of the length here...
        const auto inputLength = std::strlen(inputData);
        if (inputLength == 0) {
            return std::string();
        }

        // Fast path: nothing to detect and nothing to convert if it already is UTF-8.
        // Skipping uchardet here is what makes the common case cheap.
        if (IsValidUtf8(inputData, inputLength)) {
            return std::string(inputData, inputLength);
        }

        uchardet_handle_data(uc, inputData, inputLength); //TODO error handling
        uchardet_data_end(uc);
        // uchardet owns the returned string, it must not be freed. Copy it before the reset
        // below, which clears the detection result.
        auto detectedCharset = uchardet_get_charset(uc);
        const std::string charset(detectedCharset == nullptr ? "" : detectedCharset);
        uchardet_reset(uc);

        // An empty charset is uchardet's documented failure return, and there is nothing to
        // convert if the input already is UTF-8.
        if (charset.empty() || charset == "UTF-8") {
            return std::string(inputData, inputLength);
        }

        auto cd = iconv_open("UTF-8", charset.c_str());
        if (cd == reinterpret_cast<iconv_t>(-1)) {
            if (warn) {
                warn("Cannot convert from charset '" + charset + "' to UTF-8, passing the string through unchanged");
            }
            return std::string(inputData, inputLength);
        }
        auto closeIconv = gsl::finally([&cd] { iconv_close(cd); });

        // utf-8 possibly needs up to 4 bytes for a single character :/
        std::string result(inputLength * 4, '\0');

        // iconv advances both cursors, so it must not be handed the pointers we still need:
        // inputData belongs to the caller, and result's buffer has to be freed from its start.
        char *inputCursor = const_cast<char *>(inputData);
        size_t inputLeft = inputLength;
        char *outputCursor = &result[0];
        size_t outputLeft = result.size();

        if (iconv(cd, &inputCursor, &inputLeft, &outputCursor, &outputLeft) == static_cast<size_t>(-1)) {
            if (warn) {
                warn("Could not convert a string from charset '" + charset + "' to UTF-8, passing it through unchanged");
            }
            return std::string(inputData, inputLength);
        }

        // iconv returns the number of irreversible conversions, not the output length. The
        // number of bytes written is how far the output cursor moved.
        result.resize(static_cast<size_t>(outputCursor - &result[0]));
        return result;
    }
} // namespace statusengine

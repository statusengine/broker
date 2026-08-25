#include "Encoding.h"

#include <cstring>
#include <iconv.h>

#include "gsl.h"

namespace statusengine {

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

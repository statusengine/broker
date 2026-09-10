#pragma once

#include <functional>
#include <string>

#include <uchardet.h>

namespace statusengine {

    /**
     * Detects the charset of a string and converts it to UTF-8.
     *
     * This deliberately does not depend on naemon or on IStatusengine, so that it can be
     * built and tested on its own. Problems during a conversion are reported through the
     * warning callback instead of being logged directly.
     *
     * Holds a uchardet handle and is not thread safe.
     */
    class Encoder {
      public:
        using WarnCallback = std::function<void(const std::string &)>;

        Encoder();
        ~Encoder();

        Encoder(const Encoder &) = delete;
        Encoder(Encoder &&) = delete;
        Encoder &operator=(const Encoder &) = delete;
        Encoder &operator=(Encoder &&) = delete;

        void SetWarnCallback(WarnCallback callback);

        /**
         * Returns inputData as UTF-8. A null or empty input yields an empty string. If the
         * charset cannot be detected or the conversion fails, the input is returned
         * unchanged and the warning callback is invoked.
         */
        std::string ToUtf8(const char *inputData);

      private:
        uchardet_t uc;
        WarnCallback warn;
    };
} // namespace statusengine

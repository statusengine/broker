#pragma once

#include <sstream>

#include "nebmodule.h"

namespace statusengine {
    inline void SendLogMessage(std::ostream &os, int logLevel) {
        std::ostringstream *ss = reinterpret_cast<std::ostringstream *>(&os);
        nm_log(logLevel, "%s", ("Statusengine: " + ss->str()).c_str());
    }

    inline std::ostream &eom(std::ostream &os) {
        SendLogMessage(os, NSLOG_INFO_MESSAGE);

        return os;
    }

    inline std::ostream &eoem(std::ostream &os) {
        SendLogMessage(os, NSLOG_RUNTIME_ERROR);

        return os;
    }

    inline std::ostream &eowm(std::ostream &os) {
        SendLogMessage(os, NSLOG_RUNTIME_WARNING);

        return os;
    }
} // namespace statusengine

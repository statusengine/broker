#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <sstream>

#include "nebmodule.h"

namespace statusengine {
    inline void SendLogMessage(std::ostream &os, int logLevel) {
        std::ostringstream *ss = reinterpret_cast<std::ostringstream *>(&os);
        nm_log(logLevel, "%s", ss->str().c_str());
        ss->str("Statusengine: ");
        ss->clear();
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

#endif // !LOGSTREAM_H

#include "LogStream.h"

#include "nebmodule.h"


namespace statusengine {

	static void SendLogMessage(std::ostream& os, int logLevel) {
		std::ostringstream *ss = reinterpret_cast<std::ostringstream*>(&os);
		nm_log(logLevel, "[Statusengine] %s", ss->str().c_str());
		ss->str("");
		ss->clear();
	}

	std::ostream& eom(std::ostream& os) {
		SendLogMessage(os, NSLOG_INFO_MESSAGE);

		return os;
	}

	std::ostream& eoem(std::ostream& os) {
		SendLogMessage(os, NSLOG_RUNTIME_ERROR);

		return os;
	}

	std::ostream& eowm(std::ostream& os) {
		SendLogMessage(os, NSLOG_RUNTIME_WARNING);

		return os;
	}
}

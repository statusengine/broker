#include "nebmodule.h"

#include <string>

#include "Statusengine.h"


// This is required by naemon
extern "C" {
	NEB_API_VERSION(CURRENT_NEB_API_VERSION);
}


statusengine::Statusengine *se;

extern "C" int nebmodule_init(int flags, char *args, nebmodule *handle) {

	se = new statusengine::Statusengine(handle, std::string(args));
	return se->Init();
}

extern "C" int nebmodule_deinit(int flags, int reason) {
	delete se;

	return 0;
}

#pragma once

#ifndef BUILD_NAGIOS
extern "C" {
#include "naemon/naemon.h"
}
#else
#include <cstring>

#include "nagios/broker.h"
#include "nagios/comments.h"
#include "nagios/common.h"
#include "nagios/downtime.h"
#include "nagios/macros.h"
#include "nagios/nagios.h"
#include "nagios/nebcallbacks.h"
#include "nagios/nebmodules.h"
#include "nagios/nebstructs.h"

typedef int NEBCallbackType;
typedef nagios_comment comment;

inline void nm_log(long unsigned logLevel, const char *_, const char *message) {
    char *temp = nullptr;
    temp = strdup(message);
    write_to_all_logs(temp, logLevel);
    delete temp;
}
#endif

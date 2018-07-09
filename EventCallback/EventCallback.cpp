#include "EventCallback.h"

#include "Statusengine.h"

namespace statusengine {
    EventCallback::EventCallback(Statusengine *se) : se(se) {}
    EventCallback::~EventCallback() {}
} // namespace statusengine

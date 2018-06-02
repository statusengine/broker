#include "MessageHandler.h"

#include "Statusengine.h"

namespace statusengine {

    MessageHandler::MessageHandler(Statusengine *se) : se(se) {}
    MessageHandler::~MessageHandler(){};
} // namespace statusengine
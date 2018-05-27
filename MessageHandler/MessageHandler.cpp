#include "MessageHandler.h"

#include "Statusengine.h"

namespace statusengine {

    MessageHandler::MessageHandler(Statusengine *se) : se(se) {}
    MessageHandler::~MessageHandler(){};

    bool MessageHandler::Connect() {
        return true;
    }
} // namespace statusengine
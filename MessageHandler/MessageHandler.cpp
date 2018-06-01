#include "MessageHandler.h"

#include "Statusengine.h"

#include <iostream>

namespace statusengine {

    MessageHandler::MessageHandler(Statusengine *se) : se(se) {
        std::cout << "MessageHandler::MessageHandler: " << se << std::endl;
    }
    MessageHandler::~MessageHandler(){};
} // namespace statusengine
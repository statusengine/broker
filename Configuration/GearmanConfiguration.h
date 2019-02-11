#pragma once

#include <string>

#include "MessageHandlerConfiguration.h"

namespace statusengine {
    class GearmanConfiguration : public MessageHandlerConfiguration {
      public:
        explicit GearmanConfiguration(IStatusengine *se);

        virtual bool Load(const toml::Table &tbl);

        std::string URL;
    };
} // namespace statusengine

#include "GearmanConfiguration.h"


namespace statusengine {
    GearmanConfiguration::GearmanConfiguration(IStatusengine *se) : MessageHandlerConfiguration(se) {}

    bool GearmanConfiguration::Load(const toml::Table &tbl) {
        try {
            URL = toml::get<std::string>(tbl.at("URL"));
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Invalid value for key URL" << LogLevel::Error;
            return false;
        }

        return true;
    }
} // namespace statusengine

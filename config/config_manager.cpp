#include "config_manager.h"

namespace jimu {
namespace config {

ConfigManager::ConfigManager() :
    logger_manager_config() {}

ConfigManager* ConfigManager::instance() {
    static ConfigManager config_manager;
    return &config_manager;
}

uint32_t ConfigManager::init() {
    std::ifstream config_file("./conf/jimu_config.json");
    Json json_config = Json::parse(config_file);

    if (parse_jimu_logger_manager_config(
            json_config, logger_manager_config)) {
        return -1;
    }

    return 0;
}

}
}
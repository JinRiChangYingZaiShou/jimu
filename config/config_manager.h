#ifndef JIMU_CONFIG_CONFIG_MANAGER_H
#define JIMU_CONFIG_CONFIG_MANAGER_H

#include <fstream>

#include "config_common.h"
#include "config/parse/config_parse.h"

namespace jimu {
namespace config {

class ConfigManager {
public:
    JimuLoggerManagerConfig logger_manager_config;

    ConfigManager();
    ~ConfigManager();

    static ConfigManager* instance();
    uint32_t init();
}

}
}

#endif
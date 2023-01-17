#ifndef JIMU_CONFIG_PARSE_PARSE_CONFIG_PARSE_H
#define JIMU_CONFIG_PARSE_PARSE_CONFIG_PARSE_H

#include "config/model/logger_config.h"

#include "third-party/json/include/nlohmann/json.hpp"

namespace jimu {
namespace config {

uint32_t parse_jimu_logger_manager_config(Json &json_config,
    JimuLoggerManagerConfig &config_var);

}
}

#endif
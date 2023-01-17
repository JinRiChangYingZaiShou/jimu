#ifndef JIMU_CONFIG_MODEL_LOGGER_CONFIG_H
#define JIMU_CONFIG_MODEL_LOGGER_CONFIG_H

#include <string>
#include <atomic>
#include <iostream>
#include <vector>

namespace jimu {
namespace config {

struct JimuLoggerConfig {
    std::string logger_name;
    uint64_t logger_level;
};

struct JimuLoggerManagerConfig {
    uint64_t logger_level_all;
    uint32_t write_pool_thread_num;
    std::vector<JimuLoggerConfig> logger_array;
};

}
}

#endif
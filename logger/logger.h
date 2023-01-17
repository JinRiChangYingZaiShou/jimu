#ifndef JIMU_LOGGER_LOGGER_H
#define JIMU_LOGGER_LOGGER_H

#include <string>
#include <atomic>
#include <fstream>
#include <iostream>
#include <chrono>
#include <map>
#include <vector>

#include "logger_buffer.h"
#include "config/parse/config_parse.h"
#include "tools/time_tools.h"

#include "third-party/json/include/nlohmann/json.hpp"

namespace jimu {
namespace logger {

using Json = nlohmann::json;

class JimuLogger {
    std::string logger_name;
    std::string logger_file_suffix;
    std::string file_path;

    std::atomic<WriteFilePoint> log_file_p;
    
    LoggerBuffer* write_buffer;

    static std::string get_time_suffix();
    void update_file_path();
    bool check_logger_time();
    void update_file_point();

public:
    JimuLogger();
    ~JimuLogger();

    uint32_t init(std::string logger_name);

    uint32_t write_log(std::string log);
};

class JimuLoggerManager {
    using JimuLoggerManagerTable =
        std::map<std::string, std::shared_ptr<JimuLogger>>;
    using JimuLoggerLevelManagerTable =
        std::map<std::string, uint64_t>;
    
    uint32_t write_pool_thread_num;
    uint64_t logger_level_standard;

    JimuLoggerManagerTable table;
    JimuLoggerLevelManagerTable level_table;

    uint32_t check_logger_level(const std::string &logger_name);

public:
    JimuLoggerManager();
    ~JimuLoggerManager();

    static JimuLoggerManager* instance();

    uint32_t init(Json &json_config);
    uint32_t write_log(std::string logger_name, std::string file_name,
        uint32_t line, std::string log_str);
};

#define WRITE_LOG(LOG_NAME, LOG_STR) (JimuLoggerManager::instance() \
    ->write_log(LOG_NAME, __FILE__, __LINE__, LOG_STR))

}
}

#endif
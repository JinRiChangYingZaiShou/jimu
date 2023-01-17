#include "logger.h"

namespace jimu {
namespace logger {

std::string JimuLogger::get_time_suffix() {
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto time_tm = localtime(&tt);
    char data_str[20];
    sprintf(data_str, "%04d%02d%02d%02d", time_tm->tm_year + 1900,
		time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour);
    return std::string(data_str);
}

void JimuLogger::update_file_path() {
    logger_file_suffix = get_time_suffix();
    file_path = "./log/";
    file_path = file_path + logger_name + "." + logger_file_suffix;
}

bool JimuLogger::check_logger_time() {
    return !logger_file_suffix.compare(get_time_suffix());
}

void JimuLogger::update_file_point() {
    WriteFilePoint new_file_p = std::make_shared<std::ofstream>();
    new_file_p->open(file_path, std::ios_base::app);

    log_file_p.store(new_file_p);
    write_buffer->reset_file_p(new_file_p);
}

JimuLogger::JimuLogger() : logger_name(""), log_file_p(nullptr),
    write_buffer(nullptr), file_path("") {}

uint32_t JimuLogger::init(std::string logger_name) {
    write_buffer = new LoggerBuffer();
    this->logger_name = logger_name;
    update_file_path();
    update_file_point();

    return 0;
}

uint32_t JimuLogger::write_log(std::string log) {
    auto ret = write_buffer->write(log.c_str(), log.length());

    if (check_logger_time()) {
        update_file_path();
        update_file_point();
    }

    return ret;
}

JimuLogger::~JimuLogger() {
    delete write_buffer;
}

JimuLoggerManager* JimuLoggerManager::instance() {
    static JimuLoggerManager logger_manager;
    return &logger_manager;
}

JimuLoggerManager::JimuLoggerManager() : write_pool_thread_num(0),
    logger_level_standard(0), table(), level_table() {}

JimuLoggerManager::~JimuLoggerManager() {}

uint32_t JimuLoggerManager::init(Json &json_config) {
    jimu::config::JimuLoggerManagerConfig config_var;
    if (parse_jimu_logger_manager_config(json_config, config_var)) {
        return 1;
    }

    if(LoggerBufferFlushPool::instance()
        ->init(config_var.write_pool_thread_num)) {
        return 2;
    }
    logger_level_standard = config_var.logger_level_all;
    
    for (auto & logger_config : config_var.logger_array) {
        if (table.count(logger_config.logger_name)) {
            return 3;
        }

        table.insert(logger_config.logger_name, std::make_shared<JimuLogger>());
        level_table.insert(logger_config.logger_name, logger_config.logger_level);
    }
    
    for (auto & [key, value] : table) {
        if (value->init(key)) {
            return 4;
        }
    }

    return 0;
}

uint32_t JimuLoggerManager::check_logger_level(const std::string &logger_name) {
    auto ret = level_table.find(logger_name);
    if (ret == level_table.end()) {
        return 1;
    }

    if (ret->second & logger_level_standard) {
        return 0;
    }
    else {
        return 2;
    }
}

uint32_t JimuLoggerManager::write_log(std::string logger_name, std::string file_name,
        uint32_t line, std::string log_str) {

    if (check_logger_level(logger_name)) {
        return 1;
    }
    
    std::ostringstream log_str_stream;

    log_str_stream << "[" << logger_name << "]-";
    log_str_stream << jimu::tools::get_current_time() << "-";
    log_str_stream << "[" << file_name << ":" << line << "]:";
    log_str_stream << log_str << "\n";

    return table[logger_name]->write_log(log_str_stream.str());
}

}
}
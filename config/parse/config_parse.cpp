#include "config_parse.h"

namespace jimu {
namespace config {

uint32_t parse_jimu_logger_manager_config(Json &json_config,
    JimuLoggerManagerConfig &config_var) {

    Json tmp_json_config = json_config.at("logger_manager_config");
    
    tmp_json_config.at("logger_level_all").get_to(config_var.logger_level_all);
    tmp_json_config.at("write_pool_thread_num")
        .get_to(config_var.write_pool_thread_num);

    auto json_array = tmp_json_config.at("logger_array").items();
    for (auto & json_item : json_array) {
        auto obj = json_item.value();
        JimuLoggerConfig logger_config;

        obj.at("logger_name").get_to(logger_config.logger_name);
        obj.at("logger_level").get_to(logger_config.logger_level);

        config_var.logger_array.push_back(logger_config);
    }

    return 0;
}

}
}
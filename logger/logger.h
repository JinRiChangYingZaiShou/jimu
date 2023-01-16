#ifndef JIMU_LOGGER_LOGGER_H
#define JIMU_LOGGER_LOGGER_H

#include <string>
#include <fstream>
#include <iostream>

#include "logger_buffer.h"

namespace jimu {
namespace logger {

class JimuLogger {
    std::string file_name;
    std::ofstream* log_file_p;
    
};

class JimuLoggerManager {

};

}
}

#endif
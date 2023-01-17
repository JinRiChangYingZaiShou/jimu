#include "time_tools.h"

namespace jimu {
namespace tools {

std::string get_current_time(){
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm* now_tm = std::localtime(&now_time_t);

	char buffer[128];
	strftime(buffer, sizeof(buffer), "%F %T", now_tm);

	std::ostringstream ss;
	ss.fill('0');

	std::chrono::milliseconds ms;
	std::chrono::microseconds cs;

	ms = std::chrono::
        duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
	ss << buffer << ":" << ms.count();

	return ss.str();
}

}
}
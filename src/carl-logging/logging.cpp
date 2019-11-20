#include "logging.h"

#include "Logger.h"

namespace carl::logging {

bool visible(LogLevel level, const std::string& channel) noexcept {
	return Logger::getInstance().visible(level, channel);
}

void log(LogLevel level, const std::string& channel, const std::stringstream& ss, const RecordInfo& info) {
	Logger::getInstance().log(level, channel, ss, info);
}

}
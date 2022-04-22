#include "carl-logging.h"
#include "logging.h"
#include "Logger.h"

namespace carl {
namespace logging {

void setInitialLogLevel()
{
	carl::logging::logger().configure("carl_logfile", "carl.log");
	carl::logging::logger().filter("carl_logfile")
		("carl", carl::logging::LogLevel::LVL_INFO)
		("carl.ran", carl::logging::LogLevel::LVL_DEBUG)
		("carl.converter", carl::logging::LogLevel::LVL_DEBUG)

	;

	carl::logging::logger().configure("stdout", std::cout);
	carl::logging::logger().filter("stdout")
		("carl", carl::logging::LogLevel::LVL_WARN)
//		("carl.algsubs", carl::logging::LogLevel::LVL_DEBUG)
		("carl.ran", carl::logging::LogLevel::LVL_DEBUG)
		("carl.converter", carl::logging::LogLevel::LVL_DEBUG)
		("carl.poly", carl::logging::LogLevel::LVL_DEBUG)
//		("carl.ran.realroots", carl::logging::LogLevel::LVL_TRACE)
//		("carl.fieldext", carl::logging::LogLevel::LVL_TRACE)
//		("carl.formula", carl::logging::LogLevel::LVL_WARN)
	;
	carl::logging::logger().resetFormatter();
}

}
}

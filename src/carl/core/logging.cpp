#include <iostream>

#include "logging.h"
#include "carlLogging.h"

namespace carl {
namespace logging {

void setInitialLogLevel()
{
	carl::logging::logger().configure("logfile", "carl.log");
	carl::logging::logger().filter("logfile")
		("carl", carl::logging::LogLevel::LVL_INFO)
		("carl.cad", carl::logging::LogLevel::LVL_TRACE)
		("carl.cad.dot.elimination", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.elimination", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.eliminationset", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.sampleset", carl::logging::LogLevel::LVL_INFO)
		("carl.core.resultant", carl::logging::LogLevel::LVL_INFO)
		("carl.core.rootfinder", carl::logging::LogLevel::LVL_INFO)
		("carl.core.factorizedpolynomial", carl::logging::LogLevel::LVL_ALL)
	;

	carl::logging::logger().configure("stdout", std::cout);
	carl::logging::logger().filter("stdout")
		("carl", carl::logging::LogLevel::LVL_WARN)
	;
	carl::logging::logger().resetFormatter();
}

}
}
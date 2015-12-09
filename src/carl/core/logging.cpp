#include <iostream>

#include "logging.h"
#include "carlLogging.h"

namespace carl {
namespace logging {

void setInitialLogLevel()
{
	carl::logging::logger().configure("carl_logfile", "carl.log");
	carl::logging::logger().filter("carl_logfile")
		("carl", carl::logging::LogLevel::LVL_INFO)
		("carl.cad", carl::logging::LogLevel::LVL_TRACE)
		("carl.cad.projection", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.dot.elimination", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.elimination", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.eliminationset", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.sampleset", carl::logging::LogLevel::LVL_INFO)
		("carl.core.resultant", carl::logging::LogLevel::LVL_INFO)
		("carl.core.rootfinder", carl::logging::LogLevel::LVL_INFO)
		("carl.core.factorizedpolynomial", carl::logging::LogLevel::LVL_ALL)
		("carl.ran", carl::logging::LogLevel::LVL_DEBUG)
	;

	carl::logging::logger().configure("stdout", std::cout);
	carl::logging::logger().filter("stdout")
		("carl", carl::logging::LogLevel::LVL_WARN)
		("carl.cad", carl::logging::LogLevel::LVL_DEBUG)
		("carl.cad.projection", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.sampleset", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.elimination", carl::logging::LogLevel::LVL_INFO)
		("carl.cad.dot", carl::logging::LogLevel::LVL_WARN)
		("carl.ran", carl::logging::LogLevel::LVL_DEBUG)
		("carl.core.rootfinder", carl::logging::LogLevel::LVL_TRACE)
		("carl.cad.cg", carl::logging::LogLevel::LVL_DEBUG)
		("carl.formula", carl::logging::LogLevel::LVL_WARN)
	;
	carl::logging::logger().resetFormatter();
}

}
}

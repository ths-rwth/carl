#include <iostream>

#include "logging.h"
#include "carlLogging.h"

namespace carl
{

void setInitialLogLevel()
{
	carl::logging::logger().configure("logfile", "carl.log");
	carl::logging::logger().filter("logfile")
		("carl", carl::logging::LogLevel::INFO)
		("carl.cad", carl::logging::LogLevel::TRACE)
		("carl.cad.sampleset", carl::logging::LogLevel::INFO)
	;

	carl::logging::logger().configure("stdout", std::cout);
	carl::logging::logger().filter("stdout")
		("carl", carl::logging::LogLevel::WARN)
	;
	carl::logging::logger().resetFormatter();
}
}
#include <iostream>

#include "logging.h"
#include "carlLogging.h"

namespace carl
{

void setInitialLogLevel()
{
#ifdef LOGi2_USE_LOG4CPLUS
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl"));
	// Set minimal loglovel
    logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
	DEVLOGGER.setLogLevel(log4cplus::ERROR_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.cad")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.cad.sampleset")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.rootfinder")).setLogLevel(log4cplus::INFO_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.evaluation")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.mvpolynomial")).setLogLevel(log4cplus::INFO_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.hensel")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.resultant")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.gb")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
#else
	carl::logging::logger().configure("logfile", "carl.log");
	carl::logging::logger().filter("logfile")
		("carl", carl::logging::LogLevel::INFO)
		("carl.cad", carl::logging::LogLevel::TRACE)
	;

	carl::logging::logger().configure("stdout", std::cout);
	carl::logging::logger().filter("stdout")
		("carl", carl::logging::LogLevel::WARN)
	;
#endif
}
}
#include "logging.h"

namespace carl
{
#ifdef LOGi2_USE_LOG4CPLUS

void setInitialLogLevel()
{
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl"));
	// Set minimal loglovel
    logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
	DEVLOGGER.setLogLevel(log4cplus::ERROR_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.cad")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.rootfinder")).setLogLevel(log4cplus::INFO_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.evaluation")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.mvpolynomial")).setLogLevel(log4cplus::INFO_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.hensel")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.core.resultant")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
	log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.gb")).setLogLevel(log4cplus::TRACE_LOG_LEVEL);
}
#endif
}
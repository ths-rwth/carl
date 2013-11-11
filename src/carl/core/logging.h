/**
 * @file:   logging.h
 * @author: Sebastian Junges
 *
 * A small wrapper which sends messages to LOG4CPLUS if LOG4CPLUS is available,
 * otherwise the messages are just printed to std::cout.
 *
 * Important Macros:
 * USELOG4CPLUS (pass messages to log4cplus)
 * LOGi2_DISABLE (disable logging)
 *
 * Created: 21/11/2012
 * @version: 2013-10-28
 */

#pragma once

#include "config.h"
#include <string>

//#define LOGi2_DISABLE_FATAL_MSG
//#define LOGi2_DISABLE_ERROR_MSG
//#define LOGi2_DISABLE_WARN_MSG
//#define LOGi2_DISABLE_INFO_MSG
//#define LOGi2_DISABLE_DEBUG_MSG
//#define LOGi2_DISABLE_TRACE_MSG

#define LOGi2_TOFILE


#ifndef CARL_LOGGING
#define LOGi2_DISABLE
#undef LOGi2_USE_LOG4CPLUS
#endif

#ifndef LOGi2_DISABLE
	#ifdef LOGi2_USE_LOG4CPLUS
		// include the appropriate headers.
		#include <log4cplus/logger.h>
		#include <log4cplus/loggingmacros.h>
		#include <log4cplus/configurator.h>
		#include <log4cplus/fileappender.h>
		#include <log4cplus/layout.h>
		#include <log4cplus/consoleappender.h>
		#include <log4cplus/loglevel.h>
		#include "log4cplus/helpers/loglog.h"
	#endif
#endif

namespace carl
{

#ifdef LOGi2_DISABLE
	// If we do not use logging, we do not have to do anything with the messages
	#define LOGi2_DISABLE_FATAL_MSG
#endif

#ifdef LOGi2_DISABLE_FATAL_MSG
	//if we do not use fatal message, we do not use logging at all.
	#define LOGi2_DISABLE
	//And no messages of lower severity.
	#define LOGi2_DISABLE_ERROR_MSG
	//Logging instructions become no-instructions.
	#define LOGMSG_FATAL(log, msg) ;
#endif

#ifdef LOGi2_DISABLE_ERROR_MSG
	#define LOGi2_DISABLE_WARN_MSG
	#define LOGMSG_ERROR(log, msg) ;
#endif

#ifdef LOGi2_DISABLE_WARN_MSG
	#define LOGi2_DISABLE_INFO_MSG
	#define LOGMSG_WARN(log, msg) ;
#endif

#ifdef LOGi2_DISABLE_INFO_MSG
	#define LOGi2_DISABLE_DEBUG_MSG
	#define LOGMSG_INFO(log, msg) ;
#endif

#ifdef LOGi2_DISABLE_DEBUG_MSG
	#define LOGi2_DISABLE_TRACE_MSG
	#define LOGMSG_DEBUG(log, msg) ;
#endif


#ifdef LOGi2_DISABLE_TRACE_MSG
	#define LOGMSG_TRACE(log, msg) ;
#endif


#ifndef LOGi2_DISABLE
	#ifdef LOGi2_USE_LOG4CPLUS
	// use LOG4CPLUS types
		typedef log4cplus::Logger Log;
		#define ROOTLOGGER log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl"))
		#define ASSERTIONLOGGER log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.assert"))
		#define DEVLOGGER (log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl.dev")))
		#define getLog(name) log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name))
		// use LOG4CPLUS macros
		#ifndef LOGi2_DISABLE_FATAL_MSG
			#define LOGMSG_FATAL(log, msg) LOG4CPLUS_FATAL((log), msg)
		#endif
		#ifndef LOGi2_DISABLE_ERROR_MSG
			#define LOGMSG_ERROR(log, msg) LOG4CPLUS_ERROR((log), msg)
		#endif
		#ifndef LOGi2_DISABLE_WARN_MSG
			#define LOGMSG_WARN(log, msg) LOG4CPLUS_WARN((log), msg)
		#endif
		#ifndef LOGi2_DISABLE_INFO_MSG
			#define LOGMSG_INFO(log, msg) LOG4CPLUS_INFO((log), msg)
		#endif
		#ifndef LOGi2_DISABLE_DEBUG_MSG
			#define LOGMSG_DEBUG(log, msg) LOG4CPLUS_DEBUG((log), msg)
		#endif
		#ifndef LOGi2_DISABLE_TRACE_MSG
			#define LOGMSG_TRACE(log, msg) LOG4CPLUS_TRACE((log), msg)
		#endif
	#else
		typedef void* Log;
		#define getLog(name) NULL;

		#define LOGMSG(level, log, msg) std::cout << "(" << (level) << ")[" << __FUNCTION__ << ", l." << __LINE__  << "]\t" << msg << std::endl
		#ifndef LOGi2_DISABLE_FATAL_MSG
			#define LOGMSG_FATAL(log, msg) LOGMSG("fatal", log, msg)
		#endif
		#ifndef LOGi2_DISABLE_ERROR_MSG
			#define LOGMSG_ERROR(log, msg) LOGMSG("error", log, msg)
		#endif
		#ifndef LOGi2_DISABLE_WARN_MSG
			#define LOGMSG_WARN(log, msg) LOGMSG("warn", log, msg)
		#endif
		#ifndef LOGi2_DISABLE_INFO_MSG
			#define LOGMSG_INFO(log, msg) LOGMSG("info", log, msg)
		#endif
		#ifndef LOGi2_DISABLE_DEBUG_MSG
			#define LOGMSG_DEBUG(log, msg) LOGMSG("debug", log, msg)
		#endif
		#ifndef LOGi2_DISABLE_TRACE_MSG
			#define LOGMSG_TRACE(log, msg) LOGMSG("trace", log, msg)
		#endif
	#endif //USELOG4CPLUS
#else
	typedef void* Log;
	#define getLog(name) NULL;
#endif

#ifndef LOGi2_DISABLE	
	#ifdef LOGi2_USE_LOG4CPLUS
		#define LOG_ASSERT(condition, message) ASSERTIONLOGGER.assertion((condition), (std::string)" " + std::string(message))
		#define LOG_NOTIMPLEMENTED() DEVLOGGER.assertion(false, (std::string)"Not implemented method-stub called: " +  (std::string)__PRETTY_FUNCTION__)
		#define LOG_INEFFICIENT() DEVLOGGER.log(log4cplus::WARN_LOG_LEVEL, (std::string)"Inefficient method called: " +  (std::string)__PRETTY_FUNCTION__)
	#else
		#define LOG_ASSERT(condition, message) assert(condition)
		#define LOG_NOTIMPLEMENTED() assert(false);
		#define LOG_INEFFICIENT() std::cout <<  "Inefficient method called:" << __PRETTY_FUNCTION__ << std::endl;
	#endif
#else
	#define LOG_ASSERT(condition, message) assert(condition)
	#define LOG_NOTIMPLEMENTED() assert(false)
	#define LOG_INEFFICIENT() ;
#endif

void setInitialLogLevel();

inline void configureLogging() {
#ifdef LOGi2_USE_LOG4CPLUS
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("carl"));
    #ifdef LOGi2_TOFILE
		log4cplus::SharedAppenderPtr fileAppender(new log4cplus::FileAppender(LOG4CPLUS_TEXT("carl.log")));
    #else
		log4cplus::SharedAppenderPtr fileAppender(new log4cplus::ConsoleAppender());
    #endif
    // Set layout. 
    // Notice that the current version of log4cplus uses the deprecated auto_ptr
    fileAppender->setLayout(std::auto_ptr<log4cplus::Layout>( new log4cplus::PatternLayout("%r [%T] %-5p %-25c |%-25b:%4L| -\t %m%n")));
    // Set output.
    logger.addAppender(fileAppender);
	setInitialLogLevel();
#endif
}

}

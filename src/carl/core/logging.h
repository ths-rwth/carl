/**
 * @file:   logging.h
 * @author: Sebastian Junges
 *
 * A small wrapper which sends messages to LOG4CPLUS if LOG4CPLUS is available,
 * otherwise the messages are just printed to std::cout.
 *
 * Important Macros:
 * DISABLE_LOGGING (disable any logging)
 * CARL_LOGGING (use our logging library)
 *
 * Created: 21/11/2012
 * @version: 2013-10-28
 */

#pragma once

#include "config.h"

#ifdef DISABLE_LOGGING
	#undef CARL_LOGGING
#endif

#ifdef CARL_LOGGING
	#include "carlLogging.h"
#endif

namespace carl
{

#if defined CARL_LOGGING
	#define LOGMSG_FATAL(channel, msg) CARLLOG_FATAL(channel, msg)
	#define LOGMSG_ERROR(channel, msg) CARLLOG_ERROR(channel, msg)
	#define LOGMSG_WARN(channel, msg) CARLLOG_WARN(channel, msg)
	#define LOGMSG_INFO(channel, msg) CARLLOG_INFO(channel, msg)
	#define LOGMSG_TRACE(channel, msg) CARLLOG_TRACE(channel, msg)
	#define LOGMSG_FUNC( ... ) CARLLOG_FUNC(__VA_ARGS__)

	#define LOG_ASSERT(channel, condition, msg) CARLLOG_ASSERT(channel, condition, msg)
	#define LOG_NOTIMPLEMENTED() CARLLOG_ERROR("", "Not implemented method-stub called.")
	#define LOG_INEFFICIENT() CARLLOG_WARN("", "Inefficient method called.")
#elif defined DISABLE_LOGGING
	#define LOGMSG_FATAL(channel, msg)
	#define LOGMSG_ERROR(channel, msg)
	#define LOGMSG_WARN(channel, msg)
	#define LOGMSG_INFO(channel, msg)
	#define LOGMSG_TRACE(channel, msg)
	#define LOGMSG_FUNC( ... )

	#define LOG_ASSERT(channel, condition, msg) assert(condition)
	#define LOG_NOTIMPLEMENTED()
	#define LOG_INEFFICIENT()
#else
	#define LOGMSG_BASIC(level, channel, msg) std::cerr << level << " " << channel << " " << __FILE__ << ":" << __LINE__ << " " << msg
	#define LOGMSG_FATAL(channel, msg) LOGMSG_BASIC("FATAL", channel, msg)
	#define LOGMSG_ERROR(channel, msg) LOGMSG_BASIC("ERROR", channel, msg)
	#define LOGMSG_WARN(channel, msg) LOGMSG_BASIC("WARN", channel, msg)
	#define LOGMSG_INFO(channel, msg)
	#define LOGMSG_TRACE(channel, msg)
	#define LOGMSG_FUNC( ... )

	#define LOG_ASSERT(channel, condition, msg) assert(condition)
	#define LOG_NOTIMPLEMENTED() std::cerr << "Not implemented method-stub called: " << __func__ << std::endl
	#define LOG_INEFFICIENT() std::cerr << "Inefficient method called: " << __func__ << std::endl
#endif

void setInitialLogLevel();

inline void configureLogging() {
#ifdef CARL_LOGGING
	setInitialLogLevel();
#endif
}

}

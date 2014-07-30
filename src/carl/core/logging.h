/**
 * @file:   logging.h
 * @author: Gereon Kremer
 *
 * A small wrapper for any logging library we might want to use.
 * If DISABLE_LOGGING is set, all logging is disabled.
 * Otherwise, if CARL_LOGGING is set, we log using our own logging machinery.
 * If neither is set, important messages are sent to std::cerr.
 *
 * Created: 21/11/2012
 * @version: 2014-07-30
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

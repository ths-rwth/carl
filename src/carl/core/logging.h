/**
 * @file logging.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * A small wrapper for any logging library we might want to use.
 * If LOGGING is not set, all logging is disabled.
 * Otherwise, if LOGGING_CARL is set, we log using our own logging machinery.
 * If neither is set, important messages are sent to `std::cerr`.
 *
 * Note that this header should *not* be included if you want to use the carl
 * logging facilities yourself. To do that, include carlLogging.h and create
 * logging macros like below for your own application.
 *
 * Created: 21/11/2012
 * @version: 2014-07-30
 */

#pragma once

#include <iostream>

#include "config.h"

#include "carlLogging.h"

#if !defined LOGGING
	#undef LOGGING_CARL
#endif

namespace carl {
namespace logging {

#if defined LOGGING_CARL
	#define CARL_LOGGING_ENABLED
	#define CARL_LOG_FATAL(channel, msg) __CARL_LOG_FATAL(channel, msg)
	#define CARL_LOG_ERROR(channel, msg) __CARL_LOG_ERROR(channel, msg)
	#define CARL_LOG_WARN(channel, msg) __CARL_LOG_WARN(channel, msg)
	#define CARL_LOG_INFO(channel, msg) __CARL_LOG_INFO(channel, msg)
	#define CARL_LOG_DEBUG(channel, msg) __CARL_LOG_DEBUG(channel, msg)	
	#define CARL_LOG_TRACE(channel, msg) __CARL_LOG_TRACE(channel, msg)

	#define CARL_LOG_FUNC(channel, args) __CARL_LOG_FUNC(channel, args)
	#define CARL_LOG_ASSERT(channel, condition, msg) __CARL_LOG_ASSERT(channel, condition, msg)
	#define CARL_LOG_NOTIMPLEMENTED() __CARL_LOG_ERROR("", "Not implemented method-stub called.")
	#define CARL_LOG_INEFFICIENT() __CARL_LOG_WARN("", "Inefficient method called.")
#elif defined LOGGING
	#define CARL_LOGGING_ENABLED
	#define CARL_LOG_BASIC(level, channel, msg) std::cerr << level << " " << channel << " " << __FILE__ << ":" << __LINE__ << " " << msg << std::endl
	#define CARL_LOG_FATAL(channel, msg) CARL_LOG_BASIC("FATAL", channel, msg)
	#define CARL_LOG_ERROR(channel, msg) CARL_LOG_BASIC("ERROR", channel, msg)
	#define CARL_LOG_WARN(channel, msg) CARL_LOG_BASIC("WARN", channel, msg)
	#define CARL_LOG_INFO(channel, msg)
	#define CARL_LOG_DEBUG(channel, msg)
	#define CARL_LOG_TRACE(channel, msg)

	#define CARL_LOG_FUNC(channel, args)
	#define CARL_LOG_ASSERT(channel, condition, msg) assert(condition)
	#define CARL_LOG_NOTIMPLEMENTED() std::cerr << "Not implemented method-stub called: " << __func__ << std::endl
	#define CARL_LOG_INEFFICIENT() std::cerr << "Inefficient method called: " << __func__ << std::endl
#else
	#define CARL_LOG_FATAL(channel, msg)
	#define CARL_LOG_ERROR(channel, msg)
	#define CARL_LOG_WARN(channel, msg)
	#define CARL_LOG_INFO(channel, msg)
	#define CARL_LOG_DEBUG(channel, msg)
	#define CARL_LOG_TRACE(channel, msg)

	#define CARL_LOG_FUNC(channel, args)
	#define CARL_LOG_ASSERT(channel, condition, msg) assert(condition)
	#define CARL_LOG_NOTIMPLEMENTED()
	#define CARL_LOG_INEFFICIENT()
#endif

#ifdef LOGGING_DISABLE_INEFFICIENT
	#undef CARL_LOG_INEFFICIENT
	#define CARL_LOG_INEFFICIENT()
	#undef CARL_LOG_NOTIMPLEMENTED
	#define CARL_LOG_NOTIMPLEMENTED()
#endif

void setInitialLogLevel();

inline void configureLogging() {
#ifdef CARL_LOGGING_ENABLED
	#if defined NDEBUG
		std::cerr << "CArL: You are running in release mode with logging enabled. Are you sure, that this is what you want?" << std::endl;
	#endif
#endif
#ifdef LOGGING_CARL
	setInitialLogLevel();
#endif
}

}
}

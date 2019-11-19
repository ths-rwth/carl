/**
 * @file
 *
 * A small wrapper for any logging library we might want to use.
 * If LOGGING is not set, all logging is disabled.
 *
 * Note that this header should *not* be included if you want to use the carl
 * logging facilities yourself. To do that, include carlLogging.h and create
 * logging macros like below for your own application.
 */

#pragma once

#include <ostream>

#include "config.h"

#include "carlLogging.h"

namespace carl {
namespace logging {

#if defined LOGGING
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
#else
	#define CARL_LOG_FATAL(channel, msg) std::cerr << (channel) << ": " << msg << std::endl;
	#define CARL_LOG_ERROR(channel, msg) std::cerr << (channel) << ": " << msg << std::endl;
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
	setInitialLogLevel();
#endif
}

}
}

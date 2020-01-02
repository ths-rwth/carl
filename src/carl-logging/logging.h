#pragma once

#include "LogLevel.h"

#include <sstream>
#include <string>

namespace carl::logging {

/**
 * Additional information about a log message.
 */
struct RecordInfo {
	/// File name.
	std::string filename;
	/// Function name.
	std::string func;
	/// Line number.
	std::size_t line;
};

bool visible(LogLevel level, const std::string& channel) noexcept;
void log(LogLevel level, const std::string& channel, const std::stringstream& ss, const RecordInfo& info);

}

#ifdef __VS
#define __func__ __FUNCTION__
#endif

/// Create a record info.
#define __CARL_LOG_RECORD ::carl::logging::RecordInfo{__FILE__, __func__, __LINE__}
/// Create a record info without function name.
#define __CARL_LOG_RECORD_NOFUNC ::carl::logging::RecordInfo{__FILE__, "", __LINE__}
/// Basic logging macro.
#define __CARL_LOG(level, channel, expr) { \
	if (::carl::logging::visible(level, channel)) { \
		std::stringstream __ss; __ss << expr; ::carl::logging::log(level, channel, __ss, __CARL_LOG_RECORD); \
	}}

/// Basic logging macro without function name.
#define __CARL_LOG_NOFUNC(level, channel, expr) { \
	if (::carl::logging::visible(level, channel)) { \
		std::stringstream __ss; __ss << expr; ::carl::logging::log(level, channel, __ss, __CARL_LOG_RECORD_NOFUNC); \
	}}

/// Intended to be called when entering a function. Format: `<function name>(<args>)`.
#define __CARL_LOG_FUNC(channel, args) __CARL_LOG_NOFUNC(::carl::logging::LogLevel::LVL_TRACE, channel, __func__ << "(" << args << ")");

/// Log with level LVL_TRACE.
#define __CARL_LOG_TRACE(channel, expr) __CARL_LOG(::carl::logging::LogLevel::LVL_TRACE, channel, expr)
/// Log with level LVL_DEBUG.
#define __CARL_LOG_DEBUG(channel, expr) __CARL_LOG(::carl::logging::LogLevel::LVL_DEBUG, channel, expr)
/// Log with level LVL_INFO.
#define __CARL_LOG_INFO(channel, expr) __CARL_LOG(::carl::logging::LogLevel::LVL_INFO, channel, expr)
/// Log with level LVL_WARN.
#define __CARL_LOG_WARN(channel, expr) __CARL_LOG(::carl::logging::LogLevel::LVL_WARN, channel, expr)
/// Log with level LVL_ERROR.
#define __CARL_LOG_ERROR(channel, expr) __CARL_LOG(::carl::logging::LogLevel::LVL_ERROR, channel, expr)
/// Log with level LVL_FATAL.
#define __CARL_LOG_FATAL(channel, expr) __CARL_LOG(::carl::logging::LogLevel::LVL_FATAL, channel, expr)

/// Log and assert the given condition, if the condition evaluates to false.
#define __CARL_LOG_ASSERT(channel, condition, expr) if (!(condition)) { __CARL_LOG_FATAL(channel, expr); assert(condition); }

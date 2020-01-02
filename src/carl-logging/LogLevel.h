#pragma once

#include <ostream>

namespace carl::logging {

/**
 * Indicated which log messages should be forwarded to some sink.
 * 
 * All messages which have a level that is equal or greater than the specified value will be forwarded.
 */
enum class LogLevel {
	/// All log messages.
	LVL_ALL,
	/// Finer-grained informational events than the DEBUG.
	LVL_TRACE,
	/// Fine-grained informational events that are most useful to debug an application.
	LVL_DEBUG,
	/// Highlight the progress of the application at coarse-grained level.
	LVL_INFO,
	/// Potentially harmful situations or undesired states.
	LVL_WARN,
	/// Error events that might still allow the application to continue running.
	LVL_ERROR,
	/// Severe error events that will presumably lead the application to terminate.
	LVL_FATAL,
	/// No messages.
	LVL_OFF,
	/// Default log level.
	LVL_DEFAULT = LVL_WARN
};

/**
 * Streaming operator for LogLevel.
 * @param os Output stream.
 * @param level LogLevel.
 * @return os.
 */
inline std::ostream& operator<<(std::ostream& os, LogLevel level) {
	switch (level) {
		case LogLevel::LVL_ALL:		return os << "ALL  ";
		case LogLevel::LVL_TRACE:	return os << "TRACE";
		case LogLevel::LVL_DEBUG:	return os << "DEBUG";
		case LogLevel::LVL_INFO:	return os << "INFO ";
		case LogLevel::LVL_WARN:	return os << "WARN ";
		case LogLevel::LVL_ERROR:	return os << "ERROR";
		case LogLevel::LVL_FATAL:	return os << "FATAL";
		case LogLevel::LVL_OFF:		return os << "OFF  ";
	}
	return os;
}

}

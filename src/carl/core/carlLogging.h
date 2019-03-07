#pragma once

#include "../io/streamingOperators.h"
#include "../util/Singleton.h"
#include "../util/Timer.h"
#include "../util/platform.h"
#include "carlLoggingHelper.h"
#include "config.h"

#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#ifdef THREAD_SAFE
#include <thread>
#endif
#include <utility>


namespace carl {

/**
 * Contains a custom logging facility.
 * 
 * This logging facility is fairly generic and is used as a simple and header-only alternative to more advanced solutions like log4cplus or boost::log.
 * 
 * The basic components are Sinks, Channels, Filters, RecordInfos, Formatters and the central Logger component.
 * 
 * A Sink represents a logging output like a terminal or a log file.
 * This implementation provides a FileSink and a StreamSink, but the basic Sink class can be extended as necessary.
 * 
 * A Channel is a string that identifies the context of the log message, usually something like the class name where the log message is emitted.
 * Channels are organized hierarchically where the levels are separated by dots. For example, `carl` is considered the parent of `carl.core`.
 * 
 * A Filter is associated with a Sink and makes sure that only a subset of all log messages is forwarded to the Sink.
 * Filter rules are pairs of a Channel and a minimum LogLevel, meaning that messages of this Channel and at least the given LogLevel are forwarded.
 * If a Filter does not contain any rule for some Channel, the parent Channel is considered. Each Filter contains a rule for the empty Channel, initialized with LVL_DEFAULT.
 * 
 * A RecordInfo stores auxiliary information of a log message like the filename, line number and function name where the log message was emitted.
 * 
 * A Formatter is associated with a Sink and produces the actual string that is sent to the Sink.
 * Usually, it adds auxiliary information like the current time, LogLevel, Channel and information from a RecordInfo to the string logged by the user.
 * The Formatter implements a reasonable default behaviour for log files, but it can be subclassed and modified as necessary.
 * 
 * The Logger class finally plugs all these components together.
 * It allows to configure multiple Sink objects which are identified by strings called `id` and offers a central `log()` method.
 * 
 * Initial configuration may look like this:
 * @code{.cpp}
 * carl::logging::logger().configure("logfile", "carl.log");
 * carl::logging::logger().filter("logfile")
 *     ("carl", carl::logging::LogLevel::LVL_INFO)
 *     ("carl.core", carl::logging::LogLevel::LVL_DEBUG)
 * ;
 * carl::logging::logger().resetFormatter();
 * @endcode
 * 
 * Macro facilitate the usage:
 * <ul>
 * <li>`CARLLOG_<LVL>(channel, msg)` produces a normal log message where channel should be string identifying the channel and msg is the message to be logged.</li>
 * <li>`CARLLOG_FUNC(channel, args)` produces a log message tailored for function calls. args should represent the function arguments.</li>
 * <li>`CARLLOG_ASSERT(channel, condition, msg)` checks the condition and if it fails calls `CARLLOG_FATAL(channel, msg)` and asserts the condition.</li>
 * </ul>
 * Any message (`msg` or `args`) can be an arbitrary expression that one would stream to an `std::ostream` like `stream << (msg);`. No final newline is needed.
 */
namespace logging {

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


/**
 * Base class for a logging sink. It only provides an interface to access some std::ostream.
 */
class Sink {
public:
	/**
	 * Abstract logging interface.
	 * The intended usage is to write any log output to the output stream returned by this function.
	 * @return Output stream.
	 */
	virtual std::ostream& log() noexcept = 0;
};
/**
 * Logging sink that wraps an arbitrary `std::ostream`.
 * It is meant to be used for streams like `std::cout` or `std::cerr`.
 */
class StreamSink final: public Sink {
	/// Output stream.
	std::ostream os;
public:
	/**
	 * Create a StreamSink from some output stream.
	 * @param _os Output stream.
	 */
	explicit StreamSink(std::ostream& _os): os(_os.rdbuf()) {}
	std::ostream& log() noexcept override { return os; }
};
/**
 * Logging sink for file output.
 */
class FileSink final: public Sink {
	/// File output stream.
	std::ofstream os;
public:
	/**
	 * Create a FileSink that logs to the specified file.
	 * The file is truncated upon construction.
	 * @param filename
	 */
	explicit FileSink(const std::string& filename): os(filename, std::ios::out) {}
	std::ostream& log() noexcept override { return os; }
};

/**
 * This class checks if some log message shall be forwarded to some sink.
 */
class Filter {
	/// Mapping from channels to (minimal) log levels.
	std::map<std::string, LogLevel> mData = {
		std::make_pair(std::string(""), LogLevel::LVL_DEFAULT)
	};
public:
	/**
	 * Returns the internal filter data.
	 */
	const auto& data() const {
		return mData;
	}
	/**
	 * Set the minimum log level for some channel.
	 * Returns `*this`, hence calls to this method can be chained arbitrarily.
	 * @param channel Channel name.
	 * @param level LogLevel.
	 * @return This object.
	 */
	Filter& operator()(const std::string& channel, LogLevel level) {
		mData[channel] = level;
		return *this;
	}
	/**
	 * Checks if the given log level is sufficient for the log message to be forwarded.
	 * @param channel Channel name.
	 * @param level LogLevel.
	 * @return If the message shall be forwarded.
	 */
	bool check(const std::string& channel, LogLevel level) const noexcept {
		auto tmp = channel;
		auto it = mData.find(tmp);
		while (!tmp.empty() && it == mData.end()) {
			auto n = tmp.rfind('.');
			tmp = (n == std::string::npos) ? "" : tmp.substr(0, n);
			it = mData.find(tmp);
		}
		if (it == mData.end()) {
			std::cout << "Did not find something for \"" << channel << "\"" << std::endl;
			return true;
		}
		assert(it != mData.end());
		return level >= it->second;
	}
	/**
	 * Streaming operator for a Filter.
	 * All the rules stored in the filter are printed in a human-readable fashion.
	 * @param os Output stream.
	 * @param f Filter.
	 * @return os.
	 */
	friend std::ostream& operator<<(std::ostream& os, const Filter& f) {
		os << "Filter:" << std::endl;
		for (const auto& it: f.mData) {
			os << "\t\"" << it.first << "\" -> " << it.second << std::endl;
		}
		return os;
	}
};

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

/**
 * Formats a log messages.
 */
class Formatter {
	/// Width of the longest channel.
	std::size_t channelwidth = 10;
public:
	/// Print information like log level, file etc.
	bool printInformation = true;

	virtual ~Formatter() noexcept = default;

	/**
	 * Extracts the maximum width of a channel to optimize the formatting.
	 * @param f Filter.
	 */
	virtual void configure(const Filter& f) noexcept {
		for (const auto& t: f.data()) {
			if (t.first.size() > channelwidth) channelwidth = t.first.size();
		}
	}
	/**
	 * Prints the prefix of a log message, i.e. everything that goes before the message given by the user, to the output stream.
	 * @param os Output stream.
	 * @param timer Timer holding program execution time.
	 * @param channel Channel name.
	 * @param level LogLevel.
	 * @param info Auxiliary information.
	 */
	virtual void prefix(std::ostream& os, const Timer& /*timer*/, const std::string& channel, LogLevel level, const RecordInfo& info) {
		if (!printInformation) return;
		os.fill(' ');
		//os << "[" << std::right << std::setw(5) << timer << "] ";
#ifdef THREAD_SAFE
		os << std::this_thread::get_id() << " ";
#endif
		os << level << " ";

		std::string filename(carl::basename(info.filename));
		std::size_t spacing = 1;
		if (channelwidth + 15 > channel.size() + filename.size()) {
			spacing = channelwidth + 15 - channel.size() - filename.size();
		}
		os << channel << std::string(spacing, ' ') << filename << ":" << std::left << std::setw(4) << info.line << " ";
		os << std::resetiosflags(std::ios::adjustfield);
		if (!info.func.empty()) {
			os << info.func << "(): ";
		}
	}

	/**
	 * Prints the suffix of a log message, i.e. everything that goes after the message given by the user, to the output stream.
	 * Usually, this is only a newline.
	 * @param os Output stream.
	 */
	virtual void suffix(std::ostream& os) {
		os << std::endl;
	}
};

/**
 * Main logger class.
 */
class Logger: public carl::Singleton<Logger> {
	friend carl::Singleton<Logger>;
	/// Mapping from channels to associated logging classes.
	std::map<std::string, std::tuple<std::shared_ptr<Sink>, Filter, std::shared_ptr<Formatter>>> mData;
	/// Logging mutex to ensure thread-safe logging.
	std::mutex mMutex;
	/// Timer to track program runtime.
	carl::Timer mTimer;

public:
	/**
	 * Check if a Sink with the given id has been installed.
	 * @param id Sink identifier.
	 * @return If a Sink with this id is present.
	 */
	bool has(const std::string& id) const noexcept {
		return mData.find(id) != mData.end();
	}
	/**
	 * Installs the given sink.
	 * If a Sink with this name is already present, it is overwritten.
	 * @param id Sink identifier.
	 * @param sink Sink.
	 */
	void configure(const std::string& id, std::shared_ptr<Sink> sink) {
		std::lock_guard<std::mutex> lock(mMutex);
		mData[id] = std::make_tuple(std::move(sink), Filter(), std::make_shared<Formatter>());
	}
	/**
	 * Installs a FileSink.
	 * @param id Sink identifier.
	 * @param filename Filename passed to the FileSink.
	 */
	void configure(const std::string& id, const std::string& filename) {
		configure(id, std::make_shared<FileSink>(filename));
	}
	/**
	 * Installs a StreamSink.
	 * @param id Sink identifier.
	 * @param os Output stream passed to the StreamSink.
	 */
	void configure(const std::string& id, std::ostream& os) {
		configure(id, std::make_shared<StreamSink>(os));
	}
	/**
	 * Retrieves the Filter for some Sink.
	 * @param id Sink identifier.
	 * @return Filter.
	 */
	Filter& filter(const std::string& id) noexcept {
		auto it = mData.find(id);
		assert(it != mData.end());
		return std::get<1>(it->second);
	}
	/**
	 * Retrieves the Formatter for some Sink.
	 * @param id Sink identifier.
	 * @return Formatter.
	 */
	const std::shared_ptr<Formatter>& formatter(const std::string& id) noexcept {
		auto it = mData.find(id);
		assert(it != mData.end());
		return std::get<2>(it->second);
	}
	/**
	 * Overwrites the Formatter for some Sink.
	 * @param id Sink identifier.
	 * @param fmt New Formatter.
	 */
	void formatter(const std::string& id, std::shared_ptr<Formatter> fmt) noexcept {
		auto it = mData.find(id);
		assert(it != mData.end());
		std::get<2>(it->second) = std::move(fmt);
		std::get<2>(it->second)->configure(std::get<1>(it->second));
	}
	/**
	 * Reconfigures all Formatter objects.
	 * This should be done once after all configuration is finished.
	 */
	void resetFormatter() noexcept {
		for (auto& t: mData) {
			std::get<2>(t.second)->configure(std::get<1>(t.second));
		}
	}
	/**
	 * Checks whether a log message would be visible for some sink.
	 * If this is not the case, we do not need to render it at all.
	 * @param level LogLevel.
	 * @param channel Channel name.
	 */
	bool visible(LogLevel level, const std::string& channel) const noexcept {
		for (const auto& t: mData) {
			if (std::get<1>(t.second).check(channel, level)) {
				return true;
			}
		}
		return false;
	}
	/**
	 * Logs a message.
	 * @param level LogLevel.
	 * @param channel Channel name.
	 * @param ss Message to be logged.
	 * @param info Auxiliary information.
	 */
	void log(LogLevel level, const std::string& channel, const std::stringstream& ss, const RecordInfo& info) {
		std::lock_guard<std::mutex> lock(mMutex);
		for (auto& t: mData) {
			if (!std::get<1>(t.second).check(channel, level)) continue;
			std::get<2>(t.second)->prefix(std::get<0>(t.second)->log(), mTimer, channel, level, info);
			std::get<0>(t.second)->log() << ss.str();
			std::get<2>(t.second)->suffix(std::get<0>(t.second)->log());
		}
	}
};

/**
 * Returns the single global instance of a Logger.
 * 
 * Calls `Logger::getInstance()`.
 * @return Logger object.
 */
inline Logger& logger() {
	return Logger::getInstance();
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
	if (::carl::logging::Logger::getInstance().visible(level, channel)) { \
		std::stringstream __ss; __ss << expr; ::carl::logging::Logger::getInstance().log(level, channel, __ss, __CARL_LOG_RECORD); \
	}}

/// Basic logging macro without function name.
#define __CARL_LOG_NOFUNC(level, channel, expr) { \
	if (::carl::logging::Logger::getInstance().visible(level, channel)) { \
		std::stringstream __ss; __ss << expr; ::carl::logging::Logger::getInstance().log(level, channel, __ss, __CARL_LOG_RECORD_NOFUNC); \
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

}
}

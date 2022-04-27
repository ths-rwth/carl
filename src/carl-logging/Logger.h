#pragma once

#include <carl-common/memory/Singleton.h>
#include "../carl/util/platform.h"
#include "logging_utils.h"
#include "config.h"

#include "Filter.h"
#include "Formatter.h"
#include "Sink.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
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
 * Main logger class.
 */
class Logger: public carl::Singleton<Logger> {
	friend carl::Singleton<Logger>;
	/// Mapping from channels to associated logging classes.
	std::map<std::string, std::tuple<std::shared_ptr<Sink>, Filter, std::shared_ptr<Formatter>>> mData;
	/// Logging mutex to ensure thread-safe logging.
	std::mutex mMutex;

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
			std::get<2>(t.second)->prefix(std::get<0>(t.second)->log(), channel, level, info);
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

}
}

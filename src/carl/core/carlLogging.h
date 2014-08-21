/**
 * @file carlLogging.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string.h>
#include <utility>

#include "../util/Singleton.h"
#include "carlLoggingHelper.h"

namespace carl {
namespace logging {

enum class LogLevel : unsigned {
	LVL_ALL, LVL_TRACE, LVL_DEBUG, LVL_INFO, LVL_WARN, LVL_ERROR, LVL_FATAL, LVL_OFF
};
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
		default:					return os << "???  ";
	}
}

/**
 * This classes provides an easy way to obtain the current number of milliseconds that the program has been running.
 */
struct Timer {
	typedef std::chrono::high_resolution_clock hrc;
	typedef std::chrono::duration<unsigned,std::milli> duration;
	hrc::time_point start;
	Timer(): start(hrc::now()) {}
	unsigned passed() const {
		hrc::duration d(hrc::now() - start);
		return std::chrono::duration_cast<duration>(d).count();
	}
	friend std::ostream& operator<<(std::ostream& os, const Timer& t) {
		return os << t.passed();
	}
};

/**
 * Base class for a logging sink. It only provides an interface to access some std::ostream.
 */
struct Sink {
	virtual ~Sink() {}
	virtual std::ostream& log() = 0;
};
/**
 * Logging sink that wraps an arbitrary std::ostream.
 */
struct StreamSink: public Sink {
	std::ostream os;
	StreamSink(std::ostream& os): os(os.rdbuf()) {}
	virtual ~StreamSink() {}
	virtual std::ostream& log() { return os; }
};
/**
 * Logging sink for file output.
 */
struct FileSink: public Sink {
	std::ofstream os;
	FileSink(const std::string& filename): os(filename, std::ios::out) {}
	virtual ~FileSink() { os.close(); }
	virtual std::ostream& log() { return os; }
};

/**
 * This class checks if some log message shall be forwarded to some sink.
 */
struct Filter {
	std::map<std::string, LogLevel> data;
	Filter(LogLevel level = LogLevel::LVL_WARN) {
		(*this)("", level);
	}
	/**
	 * Set the minimum log level for some channel.
     * @param channel
     * @param level
     */
	Filter& operator()(const std::string& channel, LogLevel level) {
		data[channel] = level;
		return *this;
	}
	/**
	 * Checks if the given log level is sufficient for the log message to be displayed in this channel.
     * @param channel
     * @param level
     * @return 
     */
	bool check(const std::string& channel, LogLevel level) {
		std::string curChan = channel;
		auto it = data.find(curChan);
		while (curChan.size() > 0 && it == data.end()) {
			auto n = curChan.rfind('.');
			curChan = (n == std::string::npos) ? "" : curChan.substr(0, n);
			it = data.find(curChan);
		}
		assert(it != data.end());
		return level >= it->second;
	}
	friend std::ostream& operator<<(std::ostream& os, const Filter& f) {
		os << "Filter:" << std::endl;
		for (auto it: f.data) os << "\t\"" << it.first << "\" -> " << it.second << std::endl;
		return os;
	}
};

/**
 * Additional information about some log record.
 */
struct RecordInfo {
	std::string filename;
	std::string func;
	unsigned line;
	RecordInfo(const std::string& filename, const std::string& func, unsigned line): 
		filename(filename), func(func), line(line) {}
};

/**
 * Formats a log messages.
 * @param os
 * @param channel
 * @param level
 * @param info
 */
struct Formatter {
	std::size_t channelwidth = 10;
	virtual void configure(const Filter& f) {
		for (auto t: f.data) {
			if (t.first.size() > channelwidth) channelwidth = t.first.size();
		}
	}
	virtual void prefix(std::ostream& os, const Timer& timer, const std::string& channel, LogLevel level, const RecordInfo& info) {
		os.fill(' ');
		os << "[" << std::right << std::setw(4) << timer << "] " << level << " ";
		std::string filename(carl::basename(info.filename));
		unsigned long spacing = 1;
		if (channelwidth + 15 > channel.size() + filename.size()) spacing = channelwidth + 15 - channel.size() - filename.size();
		os << channel << std::string(spacing, ' ') << filename << ":" << std::left << std::setw(4) << info.line << " ";
		if (!info.func.empty()) os << info.func << "(): ";
	}
	virtual void suffix(std::ostream& os) {
		os << std::endl;
	}
};

/**
 * Main logger class.
 */
class Logger: public carl::Singleton<Logger> {
	friend carl::Singleton<Logger>;
	std::map<std::string, std::tuple<std::shared_ptr<Sink>, Filter, std::shared_ptr<Formatter>>> data;
	std::mutex mutex;
	Timer timer;

	Logger() {
	}
public:
	~Logger() {
		data.clear();
	}
	bool has(const std::string& id) const {
		return data.find(id) != data.end();
	}
	void configure(const std::string& id, std::shared_ptr<Sink> sink, const Filter& fl = Filter()) {
		std::shared_ptr<Formatter> fm = std::make_shared<Formatter>();
		fm->configure(fl);
		this->data[id] = std::make_tuple(sink, fl, fm);
	}
	void configure(const std::string& id, const std::string& filename) {
		configure(id, std::make_shared<FileSink>(filename));
	}
	void configure(const std::string& id, std::ostream& os) {
		configure(id, std::make_shared<StreamSink>(os));
	}
	Filter& filter(const std::string& id) {
		auto it = data.find(id);
		assert(it != data.end());
		return std::get<1>(it->second);
	}
	std::shared_ptr<Formatter> formatter(const std::string& id) {
		auto it = data.find(id);
		assert(it != data.end());
		return std::get<2>(it->second);
	}
	void formatter(const std::string& id, std::shared_ptr<Formatter> fmt) {
		auto it = data.find(id);
		assert(it != data.end());
		std::get<2>(it->second) = fmt;
		std::get<2>(it->second)->configure(std::get<1>(it->second));
	}
	void resetFormatter() {
		for (auto& t: data) {
			std::get<2>(t.second)->configure(std::get<1>(t.second));
		}
	}
	void log(LogLevel level, const std::string& channel, const std::stringstream& ss, const RecordInfo& info) {
		std::lock_guard<std::mutex> lock(mutex);
		for (auto t: data) {
			if (!std::get<1>(t.second).check(channel, level)) continue;
			std::get<2>(t.second)->prefix(std::get<0>(t.second)->log(), timer, channel, level, info);
			std::get<0>(t.second)->log() << ss.str();
			std::get<2>(t.second)->suffix(std::get<0>(t.second)->log());
		}
	}
};

inline Logger& logger() {
	return Logger::getInstance();
}

#define __CARLLOG_RECORD carl::logging::RecordInfo(__FILE__, __func__, __LINE__)
#define __CARLLOG_RECORD_NOFUNC carl::logging::RecordInfo(__FILE__, "", __LINE__)
#define __CARLLOG(level, channel, expr) { std::stringstream ss; ss << expr; carl::logging::Logger::getInstance().log(level, channel, ss, __CARLLOG_RECORD); }
#define __CARLLOG_NOFUNC(level, channel, expr) { std::stringstream ss; ss << expr; carl::logging::Logger::getInstance().log(level, channel, ss, __CARLLOG_RECORD_NOFUNC); }

#define CARLLOG_FUNC(channel, args) __CARLLOG_NOFUNC(carl::logging::LogLevel::LVL_TRACE, channel, __func__ << "(" << args << ")");

#define CARLLOG_TRACE(channel, expr) __CARLLOG(carl::logging::LogLevel::LVL_TRACE, channel, expr)
#define CARLLOG_DEBUG(channel, expr) __CARLLOG(carl::logging::LogLevel::LVL_DEBUG, channel, expr)
#define CARLLOG_INFO(channel, expr) __CARLLOG(carl::logging::LogLevel::LVL_INFO, channel, expr)
#define CARLLOG_WARN(channel, expr) __CARLLOG(carl::logging::LogLevel::LVL_WARN, channel, expr)
#define CARLLOG_ERROR(channel, expr) __CARLLOG(carl::logging::LogLevel::LVL_ERROR, channel, expr)
#define CARLLOG_FATAL(channel, expr) __CARLLOG(carl::logging::LogLevel::LVL_FATAL, channel, expr)

#define CARLLOG_ASSERT(channel, condition, expr) if (!condition) { CARLLOG_FATAL(channel, expr); assert(condition); }

}
}

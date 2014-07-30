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

namespace carl {
namespace logging {

enum class LogLevel : unsigned {
	TRACE, INFO, WARN, ERROR, FATAL
};
inline std::ostream& operator<<(std::ostream& os, LogLevel level) {
	switch (level) {
		case LogLevel::TRACE: return os << "TRACE";
		case LogLevel::INFO:  return os << "INFO ";
		case LogLevel::WARN:  return os << "WARN ";
		case LogLevel::ERROR: return os << "ERROR";
		case LogLevel::FATAL: return os << "FATAL";
		default: return os << "???";
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
	Filter(LogLevel level = LogLevel::WARN) {
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
		os << "[" << std::right << std::setw(4) << timer << "] ";
		os << level << " ";
		os << std::left << std::setw((int)channelwidth) << channel << " ";
		os << std::right << std::setw(25) << basename(info.filename.c_str()) << ":" << std::left << std::setw(4) << info.line << " ";
		os << info.func << "() ";
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
	std::map<std::string, std::tuple<Sink*, Filter, Formatter>> data;
	std::mutex mutex;
	Timer timer;

	Logger() {
	}
public:
	~Logger() {
		for (auto t: data) delete std::get<0>(t.second);
		data.clear();
	}
	void configure(const std::string& id, Sink* sink, const Filter& fl = Filter()) {
		Formatter fm;
		fm.configure(fl);
		this->data[id] = std::make_tuple(sink, fl, fm);
	}
	void configure(const std::string& id, const std::string& filename) {
		configure(id, new FileSink(filename));
	}
	void configure(const std::string& id, std::ostream& os) {
		configure(id, new StreamSink(os));
	}
	Filter& filter(const std::string& id) {
		auto it = data.find(id);
		assert(it != data.end());
		return std::get<1>(it->second);
	}
	Formatter& formatter(const std::string& id) {
		auto it = data.find(id);
		assert(it != data.end());
		return std::get<2>(it->second);
	}
	void resetFormatter() {
		for (auto& t: data) {
			std::get<2>(t.second).configure(std::get<1>(t.second));
		}
	}
	void log(LogLevel level, const std::string& channel, const std::stringstream& ss, const RecordInfo& info) {
		std::lock_guard<std::mutex> lock(mutex);
		for (auto t: data) {
			if (!std::get<1>(t.second).check(channel, level)) continue;
			std::get<2>(t.second).prefix(std::get<0>(t.second)->log(), timer, channel, level, info);
			std::get<0>(t.second)->log() << ss.str();
			std::get<2>(t.second).suffix(std::get<0>(t.second)->log());
		}
	}
};

inline Logger& logger() {
	return Logger::getInstance();
}

#define __CARLLOG_RECORD carl::logging::RecordInfo(__FILE__, __func__, __LINE__)
#define __CARLLOG(level, channel, expr) { std::stringstream ss; ss << expr; carl::logging::Logger::getInstance().log(level, channel, ss, __CARLLOG_RECORD); }
#define CARLLOG_TRACE(channel, expr) __CARLLOG(carl::logging::LogLevel::TRACE, channel, expr)
#define CARLLOG_INFO(channel, expr) __CARLLOG(carl::logging::LogLevel::INFO, channel, expr)
#define CARLLOG_WARN(channel, expr) __CARLLOG(carl::logging::LogLevel::WARN, channel, expr)
#define CARLLOG_ERROR(channel, expr) __CARLLOG(carl::logging::LogLevel::ERROR, channel, expr)
#define CARLLOG_FATAL(channel, expr) __CARLLOG(carl::logging::LogLevel::FATAL, channel, expr)

}
}

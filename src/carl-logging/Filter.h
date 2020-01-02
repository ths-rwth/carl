#pragma once

#include "LogLevel.h"

#include <cassert>
#include <iostream>
#include <map>
#include <string>

namespace carl::logging {

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

}
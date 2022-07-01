#pragma once

#include "StatisticsCollector.h"
#include "Timing.h"

#include <map>
#include <sstream>
#include <algorithm>
#include <assert.h>

namespace carl {
namespace statistics {

class Statistics {
private:
	std::string mName;
	std::map<std::string, std::string> mCollected;
	bool has_illegal_chars(const std::string& val) const {
		return std::find_if(val.begin(), val.end(), [](char c) {
				return c == '(' || c == ')' || std::isspace(static_cast<unsigned char>(c));
			}) != val.end();
	}
protected:
	template<typename T>
	void addKeyValuePair(const std::string& key, const T& value) {
		assert(!has_illegal_chars(key) && "spaces, (, ) are not allowed here");
		if (has_illegal_chars(key)) return;
		if constexpr(std::is_same<T,std::string>::value) {
			assert(!has_illegal_chars(static_cast<std::string>(value)) && "spaces, (, ) are not allowed here");
			if (has_illegal_chars(value)) return;
			mCollected.emplace(key, value);
		} else if constexpr(std::is_same<T,timer>::value) {
			mCollected.emplace(key+"_count", std::to_string(static_cast<timer>(value).count()));
			mCollected.emplace(key+"_overall_ms", std::to_string(static_cast<timer>(value).overall_ms()));
		} else {
			std::stringstream ss;
			ss << value;
			mCollected.emplace(key, ss.str());
		}
	}
public:
	Statistics() = default;
	virtual ~Statistics() = default;
	Statistics(const Statistics&) = delete;
	Statistics(Statistics&&) = delete;
	Statistics& operator=(const Statistics&) = delete;
	Statistics& operator=(Statistics&&) = delete;

	void set_name(const std::string& name) {
		mName = name;
	}

	virtual bool enabled() const {
		return true;
	}
	virtual void collect() {}

	const auto& name() const {
		return mName;
	}
	const auto& collected() const {
		return mCollected;
	}
};

}
}

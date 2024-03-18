#pragma once

#include <map>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "StatisticsCollector.h"
#include "Serialization.h"
#include "Timing.h"
#include "Series.h"
#include "MultiCounter.h"

namespace carl {
namespace statistics {

class Statistics {
private:
	std::string mName;
	std::map<std::string, std::string> mCollected;
	bool has_illegal_chars(const std::string& val) const {
		return std::find_if(val.begin(), val.end(), [](char c) {
				return c == ':' || c == '(' || c == ')' || std::isspace(static_cast<unsigned char>(c));
			}) != val.end();
	}
protected:
	void addKeyValuePair(const std::string& key, const std::string& value) {
		assert(!has_illegal_chars(key) && "spaces, (, ), : are not allowed here");
		if (has_illegal_chars(key)) return;
		assert(!has_illegal_chars(static_cast<std::string>(value)) && "spaces, (, ), : are not allowed here");
		if (has_illegal_chars(value)) return;
		mCollected.emplace(key, value);
	}

	void addKeyValuePair(const std::string& key, const Timer& value) {
		value.collect(mCollected, key);
	}

	void addKeyValuePair(const std::string& key, const Series& value) {
		value.collect(mCollected, key);
	}

	template<typename T>
	void addKeyValuePair(const std::string& key, const MultiCounter<T>& value) {
		value.collect(mCollected, key);
	}

	template<typename T>
	void addKeyValuePair(const std::string& key, const T& value) {
		std::stringstream ss;
		serialize(ss, value);
		mCollected.emplace(key, ss.str());
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

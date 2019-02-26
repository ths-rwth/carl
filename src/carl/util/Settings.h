#pragma once

#include <any>
#include <chrono>
#include <map>
#include <string>

namespace carl::settings {

/**
 * Helper type to parse duration as std::chrono values with boost::program_options.
 * Intended usage:
 * - use boost to parse values as durations
 * - access values with `std::chrono::seconds(d)`
 */
struct duration: public std::chrono::nanoseconds {
	template<typename... Args>
	duration(Args&&... args): std::chrono::nanoseconds(std::forward<Args>(args)...) {}
	template<typename T>
	operator T() const {
		return static_cast<T>(*this);
	}
};

/// Streaming operator for duration. Auto-detects proper time suffix.
inline std::ostream& operator<<(std::ostream& os, const duration& d) {
	std::array<std::pair<std::string,long>,6> suffixes = {{
		{"ns", 1000}, {"µs", 1000}, {"ms", 1000},
		{"s", 60}, {"m", 60}, {"h", 1},
	}};
	std::size_t id = 0;
	long mult = 1;
	for (; id < suffixes.size()-1; ++id) {
		if (d.count() % (suffixes[id].second * mult) != 0) {
			break;
		}
		mult *= suffixes[id].second;
	}
	return os << (d.count() / mult) << suffixes[id].first;
}

struct Settings {
private:
	std::map<std::string,std::any> mSettings;
public:
	template<typename T>
	T& get(const std::string& name) {
		auto res = mSettings.emplace(name, T{});
		return std::any_cast<T&>(res.first->second);
	}
};

}
#pragma once

#include "Singleton.h"
#include "../config.h"

#include <chrono>
#include <iostream>
#include <map>

namespace carl {
namespace timing {

/// The clock type used here.
using clock = std::chrono::high_resolution_clock;
/// The duration type used here.
using duration = std::chrono::duration<std::size_t,std::milli>;
/// The type of a time point.
using time_point = clock::time_point;

/// Return the current time point.
inline auto now() {
	return clock::now();
}
/// Return the duration since the given start time point.
inline auto since(time_point start) {
	return std::chrono::duration_cast<duration>(clock::now() - start);
}
/// Return a zero duration.
inline auto zero() {
	return duration::zero();
}

}

class TimingCollector: public Singleton<TimingCollector> {
private:
	struct TimingInformation {
		std::size_t count = 0;
		timing::duration overall = timing::zero();
	};
	std::map<std::string,TimingInformation> mData;
public:
	const auto& data() const {
		return mData;
	}
	timing::time_point start() const {
		return timing::now();
	}
	void finish(const std::string& name, timing::time_point start) {
		auto& d = mData[name];
		++d.count;
		d.overall += timing::since(start);
	}
};

inline std::ostream& operator<<(std::ostream& os, const TimingCollector& tc) {
	os << "Timings:" << std::endl;
	for (const auto& d: tc.data()) {
		os << "\t" << d.first << ": " << d.second.overall.count() << "ms (ran " << d.second.count << " times)" << std::endl;
	}
	return os;
}

#ifdef TIMING
#define CARL_TIME_START() carl::TimingCollector::getInstance().start()
#define CARL_TIME_FINISH(name, start) carl::TimingCollector::getInstance().finish(name, start)
#else
#define CARL_TIME_START() carl::timing::time_point()
#define CARL_TIME_FINISH(name, start) static_cast<carl::timing::time_point>(start)
#endif

}

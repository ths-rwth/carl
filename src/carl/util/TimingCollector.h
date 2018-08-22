#pragma once

#include "Singleton.h"
#include "../config.h"

#include <chrono>
#include <iostream>
#include <map>

namespace carl {

class TimingCollector: public Singleton<TimingCollector> {
private:
	/// The clock type used jere.
	using clock = std::chrono::high_resolution_clock;
	/// The duration type used here.
	using duration = std::chrono::duration<std::size_t,std::milli>;
	struct TimingInformation {
		std::size_t count = 0;
		duration overall = duration::zero();
	};
	std::map<std::string,TimingInformation> mData;
public:
	const auto& data() const {
		return mData;
	}
	auto start() const {
		return clock::now();
	}
	void finish(const std::string& name, clock::time_point start) {
		auto diff = std::chrono::duration_cast<duration>(clock::now() - start);
		auto& d = mData[name];
		++d.count;
		d.overall += diff;
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
#define CARL_TIME_START() 0
#define CARL_TIME_FINISH(name, start) (void*)(start)
#endif

}

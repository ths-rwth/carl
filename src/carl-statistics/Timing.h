#pragma once

#include <chrono>

namespace carl {
namespace statistics {

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

class Timer {
    std::size_t m_count = 0;
    timing::duration m_overall = timing::zero();
    timing::time_point m_current_start;

public:
    static timing::time_point start() {
		return timing::now();
	}
    void finish(timing::time_point start) {
		++m_count;
		m_overall += timing::since(start);
	}
    void start_this() {
		m_current_start = start();
	}
    void finish() {
		finish(m_current_start);
	}
    auto count() const {
        return m_count;
    }
    auto overall_ms() const {
        return m_overall.count();
    }

    void collect(std::map<std::string, std::string>& data, const std::string& key) const {
        data.emplace(key+".count", std::to_string(count()));
        data.emplace(key+".overall_ms",   std::to_string(overall_ms()));
    }
};

}
}
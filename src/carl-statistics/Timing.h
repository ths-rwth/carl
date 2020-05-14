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

class timer {
    std::size_t m_count = 0;
    timing::duration m_overall = timing::zero();

public:
    static timing::time_point start() {
		return timing::now();
	}
    void finish(timing::time_point start) {
		++m_count;
		m_overall += timing::since(start);
	}
    auto count() const {
        return m_count;
    }
    auto overall_ms() const {
        return m_overall.count();
    }
};

}
}
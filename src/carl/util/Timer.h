/** 
 * @file   Timer.h
 * @author Sebastian Junges
 * @author Gereon Kremer
 *
 */

#pragma once

#include <chrono>
#include <iostream>

namespace carl
{
/**
 * This classes provides an easy way to obtain the current number of milliseconds that the program has been running.
 */
struct Timer {
	/// The clock type used jere.
	using clock = std::chrono::high_resolution_clock;
	/// The duration type used here.
	using duration = std::chrono::duration<std::size_t,std::milli>;
	/// Start of this timer.
	clock::time_point mStart;
	Timer() noexcept: mStart(clock::now()) {}
	/**
	 * Calculated the number of milliseconds since this object has been created.
     * @return Milliseconds passed.
     */
	std::size_t passed() const noexcept {
		clock::duration d(clock::now() - mStart);
		return std::chrono::duration_cast<duration>(d).count();
	}
	
	/**
	 * Reset the start point to now.
     */
	void reset() noexcept {
		mStart = clock::now();
	}
	/**
	 * Streaming operator for a Timer.
	 * Prints the result of `t.passed()`.
	 * @param os Output stream.
	 * @param t Timer.
	 * @return os.
	 */
	friend std::ostream& operator<<(std::ostream& os, const Timer& t) {
		return os << t.passed();
	}
};

}

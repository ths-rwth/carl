#pragma once

namespace carl {

/**
	 * Returns a down-rounded representation of the given numeric.
	 * @param o Number to round.
	 * @param overapproximate Flag if overapproximation shall be guaranteed.
	 * @return Double representation of o.
	 */
template<typename Rational>
double roundDown(const Rational& o, bool overapproximate = false) {
	using limits = std::numeric_limits<double>;
	double result = carl::toDouble(o);
	if (result == -limits::infinity()) {
		return result;
	}
	if (result == limits::infinity()) {
		return limits::max();
	}
	// If the cln::cl_RA cannot be represented exactly by a double, round.
	if (overapproximate || carl::rationalize<Rational>(result) != o) {
		if (result == -limits::max()) {
			return -limits::infinity();
		}
		return std::nextafter(result, -limits::infinity());
	} else {
		return result;
	}
}

/** Returns a up-rounded representation of the given numeric
	 * @param o
	 * @param overapproximate
	 * @return double representation of o (overapprox) Note, that it can return the double INFINITY.
	 */
template<typename Rational>
double roundUp(const Rational& o, bool overapproximate = false) {
	using limits = std::numeric_limits<double>;
	double result = carl::toDouble(o);
	if (result == limits::infinity()) {
		return result;
	}
	if (result == -limits::infinity()) {
		return -limits::max();
	}
	// If the cln::cl_RA cannot be represented exactly by a double, round.
	if (overapproximate || carl::rationalize<Rational>(result) != o) {
		if (result == limits::max()) {
			return limits::infinity();
		}
		return std::nextafter(result, limits::infinity());
	} else {
		return result;
	}
}
} // namespace carl

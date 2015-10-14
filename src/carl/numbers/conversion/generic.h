#pragma once

namespace carl {

	/**
	 * Returns a down-rounded representation of the given numeric.
	 * @param o Number to round.
	 * @param overapproximate Flag if overapproximation shall be guaranteed.
	 * @return Double representation of o.
	 */
	template<typename Rational>
	static double roundDown(const Rational& o, bool overapproximate = false) {
	    typedef std::numeric_limits<double> limits;
	    double result = carl::toDouble(o);
	    if (result == -limits::infinity()) return result;
	    if (result == limits::infinity()) return limits::max();
	    // If the cln::cl_RA cannot be represented exactly by a double, round.
	    if (overapproximate || carl::rationalize<Rational>(result) != o) {
	            if (result == -limits::max()) return -limits::infinity();
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
	static double roundUp(const Rational& o, bool overapproximate = false) {
	    typedef std::numeric_limits<double> limits;
	    double result = carl::toDouble(o);
	    if (result == limits::infinity()) return result;
	    if (result == -limits::infinity()) return -limits::max();
	    // If the cln::cl_RA cannot be represented exactly by a double, round.
	    if (overapproximate || carl::rationalize<Rational>(result) != o) {
	            if (result == limits::max()) return limits::infinity();
	            return std::nextafter(result, limits::infinity());
	    } else {
	            return result;
	    }
	}

	#ifdef USE_CLN_NUMBERS
	template<>
    inline cln::cl_RA convert<double, cln::cl_RA>(const double& n) {
    	return carl::rationalize<cln::cl_RA>(n);
    }
    #endif

    template<>
    inline mpq_class convert<double, mpq_class>(const double& n) {
    	return carl::rationalize<mpq_class>(n);
    }

    #ifdef USE_CLN_NUMBERS
    template<>
    inline double convert<cln::cl_RA, double>(const cln::cl_RA& n) {
    	return carl::toDouble(n);
    }
    #endif

    template<>
    inline double convert<mpq_class, double>(const mpq_class& n) {
    	return carl::toDouble(n);
    }

    template<>
    inline double convert<FLOAT_T<double>, double>(const FLOAT_T<double>& n) {
    	return n.value();
    }

    #ifdef USE_MPFR_FLOAT
    template<>
    inline double convert<FLOAT_T<mpfr_t>, double>(const FLOAT_T<mpfr_t>& n) {
    	return carl::toDouble(n);
    }
    #endif

    template<>
    inline FLOAT_T<double> convert<double, FLOAT_T<double>>(const double& n) {
    	return FLOAT_T<double>(n);
    }

    #ifdef USE_MPFR_FLOAT
    template<>
    inline FLOAT_T<mpfr_t> convert<double, FLOAT_T<mpfr_t>>(const double& n) {
    	return FLOAT_T<mpfr_t>(n);
    }
    #endif

}

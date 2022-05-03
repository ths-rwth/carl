#pragma once

#include "util.h"

#include <carl-common/config.h>
#include <carl/interval/Interval.h>
#include <carl/numbers/numbers.h>

#include <gtest/gtest.h>
#include <limits>
#include <type_traits>

using Rational = mpq_class;
//using Integer = mpz_class;

using IntegerTypes = testing::Types<
	#ifdef USE_CLN_NUMBERS
	cln::cl_I,
	#endif
	mpz_class
>;

using RationalTypes = testing::Types<
	#ifdef USE_CLN_NUMBERS
	cln::cl_RA,
	#endif
	mpq_class
>;

using NumberTypes = testing::Types<
	mpz_class,
	mpq_class,
	#ifdef USE_CLN_NUMBERS
	cln::cl_I,
	cln::cl_RA,
	#endif
	carl::sint
>;

using IntervalTypes = testing::Types<
	carl::Interval<mpz_class>,
	carl::Interval<mpq_class>,
	#ifdef USE_MPFR_FLOAT
	carl::Interval<carl::FLOAT_T<mpfr_t> >,
	#endif
	#ifdef USE_CLN_NUMBERS
	carl::Interval<cln::cl_I>,
	carl::Interval<cln::cl_RA>,
	#endif
	carl::Interval<int>,
	carl::Interval<double>
>;

/// Helper to check the result of operator<<().
template<typename T>
std::string getOutput(const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

/// Helper to work with boost::concept_check.
template<typename T, typename TT>
void require_type(const TT& tt) {
	T t = tt;
	(void)(t);
}

template<typename T>
constexpr T invalid_value() {
	return T(std::numeric_limits<typename std::underlying_type<T>::type>::max());
}

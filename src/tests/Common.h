#pragma once

#include <carl/interval/Interval.h>
#include <carl/numbers/numbers.h>

#include <gtest/gtest.h>

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

template<typename T>
std::string getOutput(const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

#pragma once

#include <carl/config.h>
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

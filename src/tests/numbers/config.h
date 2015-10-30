/**
 * Auto generated file config.h from config.h.in.
 */

#include "../../carl/interval/Interval.h"
#include "../../carl/numbers/numbers.h"

typedef testing::Types<
	mpz_class,
	#ifdef USE_CLN_NUMBERS
	cln::cl_I,
	#endif
	int
> IntegerTypes;

typedef testing::Types<
	#ifdef USE_CLN_NUMBERS
	cln::cl_RA,
	#endif
	mpq_class
> RationalTypes;

typedef testing::Types<
	mpz_class,
	mpq_class,
	#ifdef USE_CLN_NUMBERS
	cln::cl_I,
	cln::cl_RA,
	#endif
	int
> NumberTypes;

typedef testing::Types<
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
> IntervalTypes;

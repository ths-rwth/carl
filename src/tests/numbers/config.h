/**
 * Auto generated file config.h from config.h.in.
 */

#include "../../carl/interval/Interval.h"
#include "../../carl/numbers/numbers.h"

typedef testing::Types<int, mpz_class> IntegerTypes;
typedef testing::Types<mpq_class> RationalTypes;
typedef testing::Types<int, mpz_class, mpq_class> NumberTypes;

typedef testing::Types<
	carl::Interval<mpq_class>,
	carl::Interval<mpz_class>,
	#ifdef USE_MPFR_FLOAT
	carl::Interval<carl::FLOAT_T<mpfr_t> >,
	#endif
	#ifdef USE_CLN_NUMBERS
	carl::Interval<cln::cl_RA>,
	carl::Interval<cln::cl_I>,
	#endif
	carl::Interval<double>,
	carl::Interval<int>
> IntervalTypes;

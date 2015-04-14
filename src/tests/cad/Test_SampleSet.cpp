#include "gtest/gtest.h"

#include <list>

#include "carl/core/RealAlgebraicNumber.h"
#include "carl/cad/SampleSet.h"
#include "carl/util/platform.h"

using namespace carl;

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#elif defined(__VS)
#include <mpirxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

TEST(SampleSet, BasicOperations)
{  
	cad::SampleSet<Rational> s;

	auto pos = s.insert(RealAlgebraicNumberNR<Rational>::create(0));

	std::list<RealAlgebraicNumberPtr<Rational>> nums;
	nums.push_back(RealAlgebraicNumberNR<Rational>::create(1));
	nums.push_back(RealAlgebraicNumberNR<Rational>::create(2));
	s.insert(nums.begin(), nums.end());

	ASSERT_NO_THROW(s.remove(std::get<0>(pos)));
	ASSERT_FALSE(s.empty());
	ASSERT_NO_THROW(s.pop());
	ASSERT_NO_THROW(s.pop());
	ASSERT_TRUE(s.empty());
}

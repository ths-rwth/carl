#include "gtest/gtest.h"

#include <list>
#include <cln/cln.h>

#include "../../carl/core/RealAlgebraicNumber.h"
#include "carl/cad/SampleSet.h"

using namespace carl;

TEST(SampleSet, BasicOperations)
{  
	cad::SampleSet<cln::cl_RA> s;

	auto pos = s.insert(RealAlgebraicNumberNR<cln::cl_RA>::create(0));

	std::list<RealAlgebraicNumberPtr<cln::cl_RA>> nums;
	nums.push_back(RealAlgebraicNumberNR<cln::cl_RA>::create(1));
	nums.push_back(RealAlgebraicNumberNR<cln::cl_RA>::create(2));
	s.insert(nums.begin(), nums.end());

	ASSERT_NO_THROW(s.remove(pos.first));
	ASSERT_EQ(*nums.begin(), *s.begin());
	ASSERT_FALSE(s.empty());
	ASSERT_TRUE(s.emptyIR());
	ASSERT_NO_THROW(s.pop());
	ASSERT_NO_THROW(s.pop());
	ASSERT_TRUE(s.empty());
}

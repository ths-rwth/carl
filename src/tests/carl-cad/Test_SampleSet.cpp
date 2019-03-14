#include "gtest/gtest.h"

#include <list>

#include <carl/util/platform.h>
#include <carl-cad/SampleSet.h>

#include "../Common.h"

using namespace carl;

TEST(SampleSet, BasicOperations)
{
	cad::SampleSet<Rational> s;

	auto pos = s.insert(RealAlgebraicNumber<Rational>(0));

	std::list<RealAlgebraicNumber<Rational>> nums;
	nums.emplace_back(1);
	nums.emplace_back(2);
	s.insert(nums.begin(), nums.end());

	ASSERT_NO_THROW(s.remove(std::get<0>(pos)));
	ASSERT_FALSE(s.empty());
	ASSERT_NO_THROW(s.pop());
	ASSERT_NO_THROW(s.pop());
	ASSERT_TRUE(s.empty());
}

TEST(SampleSet, SampleComparator)
{
	cad::SampleSet<Rational>::SampleComparator comp(cad::SampleOrdering::IntRatRoot);
	
	ComparisonList<RealAlgebraicNumber<Rational>> samples;
	samples.emplace_back(RealAlgebraicNumber<Rational>(Rational(1)/4));
	samples.emplace_back(RealAlgebraicNumber<Rational>(Rational(1)/2));
	samples.emplace_back(RealAlgebraicNumber<Rational>(Rational(0)));
	samples.emplace_back(RealAlgebraicNumber<Rational>(Rational(1)));
	samples.emplace_back(RealAlgebraicNumber<Rational>(Rational(2)));
	
	expectRightOrder(samples, comp);
}

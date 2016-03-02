#include "gtest/gtest.h"

#include "carl/interval/Interval.h"

#include "../numbers/config.h"

using namespace carl;


template<typename T>
class SampleTest: public testing::Test {};

TYPED_TEST_CASE(SampleTest, RationalTypes);

TYPED_TEST(SampleTest, Sample)
{
	Interval<TypeParam> I(3, 5);
	EXPECT_EQ(I.sample(), 4);
}

TYPED_TEST(SampleTest, SampleSB)
{
	Interval<TypeParam> I(3, 5);
	EXPECT_EQ(I.sample(), 4);
}

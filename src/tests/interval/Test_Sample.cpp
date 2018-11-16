#include "gtest/gtest.h"

#include "carl/interval/Interval.h"
#include "../Common.h"

using namespace carl;


template<typename T>
class SampleTest: public testing::Test {};

TYPED_TEST_CASE(SampleTest, RationalTypes);

TYPED_TEST(SampleTest, Sample)
{
	EXPECT_EQ(4, sample(Interval<TypeParam>(3, BoundType::STRICT, 5, BoundType::STRICT), false));
	EXPECT_EQ(4, sample(Interval<TypeParam>(3, BoundType::STRICT, 6, BoundType::STRICT), false));
	
	EXPECT_EQ(TypeParam(7)/2, sample(Interval<TypeParam>(3, BoundType::STRICT, 4, BoundType::STRICT), false));
	EXPECT_EQ(TypeParam(55)/14, sample(Interval<TypeParam>(TypeParam(27)/7, BoundType::STRICT, 4, BoundType::STRICT), false));
}

TYPED_TEST(SampleTest, SampleSB)
{
	EXPECT_EQ(4, sample_stern_brocot(Interval<TypeParam>(3, BoundType::STRICT, 5, BoundType::STRICT), false));
	EXPECT_EQ(4, sample_stern_brocot(Interval<TypeParam>(3, BoundType::STRICT, 6, BoundType::STRICT), false));
	
	EXPECT_EQ(TypeParam(7)/2, sample_stern_brocot(Interval<TypeParam>(3, BoundType::STRICT, 4, BoundType::STRICT), false));
	EXPECT_EQ(TypeParam(31)/8, sample_stern_brocot(Interval<TypeParam>(TypeParam(27)/7, BoundType::STRICT, 4, BoundType::STRICT), false));
}

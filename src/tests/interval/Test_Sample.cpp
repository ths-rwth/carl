#include "gtest/gtest.h"

#include "carl/interval/Interval.h"

#include "../numbers/config.h"

using namespace carl;


template<typename T>
class SampleTest: public testing::Test {};

TYPED_TEST_CASE(SampleTest, RationalTypes);

TYPED_TEST(SampleTest, Sample)
{
	EXPECT_EQ(4, Interval<TypeParam>(3, BoundType::STRICT, 5, BoundType::STRICT).sample(false));
	EXPECT_EQ(4, Interval<TypeParam>(3, BoundType::STRICT, 6, BoundType::STRICT).sample(false));
	
	EXPECT_EQ(TypeParam(7)/2, Interval<TypeParam>(3, BoundType::STRICT, 4, BoundType::STRICT).sample(false));
	EXPECT_EQ(TypeParam(55)/14, Interval<TypeParam>(TypeParam(27)/7, BoundType::STRICT, 4, BoundType::STRICT).sample(false));
}

TYPED_TEST(SampleTest, SampleSB)
{
	EXPECT_EQ(4, Interval<TypeParam>(3, BoundType::STRICT, 5, BoundType::STRICT).sampleSB(false));
	EXPECT_EQ(4, Interval<TypeParam>(3, BoundType::STRICT, 6, BoundType::STRICT).sampleSB(false));
	
	EXPECT_EQ(TypeParam(7)/2, Interval<TypeParam>(3, BoundType::STRICT, 4, BoundType::STRICT).sampleSB(false));
	EXPECT_EQ(TypeParam(55)/14, Interval<TypeParam>(TypeParam(27)/7, BoundType::STRICT, 4, BoundType::STRICT).sample(false));
}

/**
 * Tests for double intervals using the templated version.
 * 
 * @file Test_Interval_Double.h
 * @author Benedikt Seidl
 * @since 2014-01-12
 * @version 2014-01-12
 */

#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
#include "carl/numbers/FLOAT_T.h"

using namespace carl;

class IntervalDoubleTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        i1 = Interval<double>(2.0, 3.5);
		i2 = Interval<double>(-2.3, 8.7);
		i3 = Interval<double>(10.412, 24.873);
    }

    virtual void TearDown()
    {
    }

    Interval<double> i1;
    Interval<double> i2;
    Interval<double> i3;
};

TEST(IntervalDouble, Constructor)
{
	Interval<double> i1();
	Interval<double> i2(2.0);
	Interval<double> i3(3.0, 5.0);

	ASSERT_EQ(Interval<double>::BoostInterval(), i1.content());
	ASSERT_EQ(Interval<double>::BoostInterval(2.0), i2.content());
	ASSERT_EQ(Interval<double>::BoostInterval(3.0, 5.0), i3.content());
}

TEST_F(IntervalDoubleTest, Getter)
{
	ASSERT_EQ(Interval<double>::BoostInterval(2.0, 3.5), i1.content());
	ASSERT_EQ(Interval<double>::BoostInterval(-2.3, 8.7), i2.content());
	ASSERT_EQ(Interval<double>::BoostInterval(10.412, 24.873), i3.content());

	ASSERT_EQ(2.0, i1.lower());
	ASSERT_EQ(3.5, i1.upper());
	ASSERT_EQ(-2.3, i2.lower());
	ASSERT_EQ(8.7, i2.upper());
	ASSERT_EQ(10.412, i3.lower());
	ASSERT_EQ(24.873, i3.upper());
}
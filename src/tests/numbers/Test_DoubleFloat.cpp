/**
 * @file Test_DoubleFloat.cpp
 * @author Benedikt Seidl
 */

#include "gtest/gtest.h"
#include <carl/numbers/numbers.h>

TEST(doubleFloatTest, Constructor)
{ 
    // double constructor tests
    double dVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(dVal));
    dVal = 0;
    EXPECT_EQ(0, carl::FLOAT_T<double>(dVal).value());
    
    // float constructor tests
    float fVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(fVal));
    fVal = 0;
    EXPECT_EQ(0, carl::FLOAT_T<double>(fVal).value());
    
    // integer constructor tests
    int iVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(double(iVal)));
    iVal = 0;
    EXPECT_EQ(0, carl::FLOAT_T<double>(double(iVal)).value());
    
    // mpfr constructor tests
    dVal = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899;
    carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(dVal);
    
    // copy constructor test
    //ASSERT_EQ(hf1, carl::FLOAT_T<double>(hf1));
}

TEST(doubleFloatTest, Hash)
{
	std::hash<carl::FLOAT_T<double>> hasher;
	hasher(carl::FLOAT_T<double>(2.0));
	SUCCEED();
}

/*
 * Test the following operations
 * 7 > 2
 * 2 < 7
 * 7 != 2
 * 7 >= 7
 * 7 <= 7
 * 7 == 7
 */
TEST(doubleFloatTest, BooleanOperators)
{
    double v1 = 7;
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    double v2 = 2;
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    double v3 = 7;
    carl::FLOAT_T<double> f3 = carl::FLOAT_T<double>(v3);
    
    ASSERT_GT(f1, f2);
    ASSERT_LT(f2, f1);
    ASSERT_NE(f1, f2);
    ASSERT_GE(f1, f3);
    ASSERT_LE(f1, f3);
    ASSERT_EQ(f1, f3);
}

/*
 * Test the following operations
 * 7 + 2 = 9
 * 7 + -3 = 4
 * 7 + 0 = 7
 * 7 + -0 = 7
 */
TEST(doubleFloatTest, Addition)
{
    double v1 = 7;
    double v2 = 2;
    double v3 = 7;
    double v4 = -3;
    double v5 = 7;
    double v6 = 0;
    double v7 = 7;
    double v8 = -0;
    double vResult1 = 9;
    double vResult2 = 4;
    double vResult3 = 7;
    double vResult4 = 7;
    
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    carl::FLOAT_T<double> f3 = carl::FLOAT_T<double>(v3);
    carl::FLOAT_T<double> f4 = carl::FLOAT_T<double>(v4);
    carl::FLOAT_T<double> f5 = carl::FLOAT_T<double>(v5);
    carl::FLOAT_T<double> f6 = carl::FLOAT_T<double>(v6);
    carl::FLOAT_T<double> f7 = carl::FLOAT_T<double>(v7);
    carl::FLOAT_T<double> f8 = carl::FLOAT_T<double>(v8);
    carl::FLOAT_T<double> result1 = carl::FLOAT_T<double>(vResult1);
    carl::FLOAT_T<double> result2 = carl::FLOAT_T<double>(vResult2);
    carl::FLOAT_T<double> result3 = carl::FLOAT_T<double>(vResult3);
    carl::FLOAT_T<double> result4 = carl::FLOAT_T<double>(vResult4);
    
    
    f1.add_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);
    
    f3.add_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);
    
    f5.add_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);
    
    f7.add_assign(f8, carl::CARL_RND::N);
    ASSERT_EQ(result4, f7);
}

/*
 * Test the following operations
 * 9 - 5 = 4
 * 9 - -4 = 13
 * 9 - 0 = 9
 * 9 - -0 = 9
 */
TEST(doubleFloatTest, Subtraction)
{
    double v1 = 9;
    double v2 = 5;
    double v3 = 9;
    double v4 = -4;
    double v5 = 9;
    double v6 = 0;
    double v7 = 9;
    double v8 = -0;
    double vResult1 = 4;
    double vResult2 = 13;
    double vResult3 = 9;
    double vResult4 = 9;
    
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    carl::FLOAT_T<double> f3 = carl::FLOAT_T<double>(v3);
    carl::FLOAT_T<double> f4 = carl::FLOAT_T<double>(v4);
    carl::FLOAT_T<double> f5 = carl::FLOAT_T<double>(v5);
    carl::FLOAT_T<double> f6 = carl::FLOAT_T<double>(v6);
    carl::FLOAT_T<double> f7 = carl::FLOAT_T<double>(v7);
    carl::FLOAT_T<double> f8 = carl::FLOAT_T<double>(v8);
    carl::FLOAT_T<double> result1 = carl::FLOAT_T<double>(vResult1);
    carl::FLOAT_T<double> result2 = carl::FLOAT_T<double>(vResult2);
    carl::FLOAT_T<double> result3 = carl::FLOAT_T<double>(vResult3);
    carl::FLOAT_T<double> result4 = carl::FLOAT_T<double>(vResult4);
    
    f1.sub_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);
    
    f3.sub_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);
    
    f5.sub_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);
    
    f7.sub_assign(f8, carl::CARL_RND::N);
    ASSERT_EQ(result4, f7);
}

/*
 * Test the following operations
 * 4 * 3 = 12
 * 4 * -5 = -20
 * 4 * 0 = 0
 * 4 * -0 = 0
 */
TEST(doubleFloatTest, Multiplication)
{
    double v1 = 4;
    double v2 = 3;
    double v3 = 4;
    double v4 = -5;
    double v5 = 4;
    double v6 = 0;
    double v7 = 4;
    double v8 = -0;
    double vResult1 = 12;
    double vResult2 = -20;
    double vResult3 = 0;
    double vResult4 = 0;
    
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    carl::FLOAT_T<double> f3 = carl::FLOAT_T<double>(v3);
    carl::FLOAT_T<double> f4 = carl::FLOAT_T<double>(v4);
    carl::FLOAT_T<double> f5 = carl::FLOAT_T<double>(v5);
    carl::FLOAT_T<double> f6 = carl::FLOAT_T<double>(v6);
    carl::FLOAT_T<double> f7 = carl::FLOAT_T<double>(v7);
    carl::FLOAT_T<double> f8 = carl::FLOAT_T<double>(v8);
    carl::FLOAT_T<double> result1 = carl::FLOAT_T<double>(vResult1);
    carl::FLOAT_T<double> result2 = carl::FLOAT_T<double>(vResult2);
    carl::FLOAT_T<double> result3 = carl::FLOAT_T<double>(vResult3);
    carl::FLOAT_T<double> result4 = carl::FLOAT_T<double>(vResult4);
    
    f1.mul_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);
    
    f3.mul_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);
    
    f5.mul_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);
    
    f7.mul_assign(f8, carl::CARL_RND::N);
    ASSERT_EQ(result4, f7);
}

/*
 * Test the following operations
 * 8 / 4 = 2
 * 8 / -2 = -4
 * -8 / -4 = 2
 * 8 / 0 dies
 */
TEST(doubleFloatTest, Division)
{
    double v1 = 8;
    double v2 = 4;
    double v3 = 8;
    double v4 = -2;
    double v5 = -8;
    double v6 = -4;
    double v7 = 8;
    double v8 = 0;
    double vResult1 = 2;
    double vResult2 = -4;
    double vResult3 = 2;
    
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    carl::FLOAT_T<double> f3 = carl::FLOAT_T<double>(v3);
    carl::FLOAT_T<double> f4 = carl::FLOAT_T<double>(v4);
    carl::FLOAT_T<double> f5 = carl::FLOAT_T<double>(v5);
    carl::FLOAT_T<double> f6 = carl::FLOAT_T<double>(v6);
    carl::FLOAT_T<double> f7 = carl::FLOAT_T<double>(v7);
    carl::FLOAT_T<double> f8 = carl::FLOAT_T<double>(v8);
    carl::FLOAT_T<double> result1 = carl::FLOAT_T<double>(vResult1);
    carl::FLOAT_T<double> result2 = carl::FLOAT_T<double>(vResult2);
    carl::FLOAT_T<double> result3 = carl::FLOAT_T<double>(vResult3);
    
    f1.div_assign(f2, carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);
    
    f3.div_assign(f4, carl::CARL_RND::N);
    ASSERT_EQ(result2, f3);
    
    f5.div_assign(f6, carl::CARL_RND::N);
    ASSERT_EQ(result3, f5);
    
#ifndef NDEBUG
	// This may not fail if we are not compiling in DEBUG mode.
    ASSERT_DEATH(f7.div_assign(f8, carl::CARL_RND::N), ".*");
#endif
}

/*
 * Test the following operations
 * sqrt 16 = 4
 * cbrt 27 = 3
 * root 256 8 = 2
 * sqrt -7 throws
 */
TEST(doubleFloatTest, Roots)
{
    double v1 = 16;
    double v2 = 27;
    double v3 = 256;
    double v4 = -7;
    double vResult1 = 4;
    double vResult2 = 3;
    double vResult3 = 2;
    
    
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    carl::FLOAT_T<double> f3 = carl::FLOAT_T<double>(v3);
    carl::FLOAT_T<double> f4 = carl::FLOAT_T<double>(v4);
    carl::FLOAT_T<double> result1 = carl::FLOAT_T<double>(vResult1);
    carl::FLOAT_T<double> result2 = carl::FLOAT_T<double>(vResult2);
    carl::FLOAT_T<double> result3 = carl::FLOAT_T<double>(vResult3);

    f1.sqrt_assign(carl::CARL_RND::N);
    ASSERT_EQ(result1, f1);

    // Todo: comparison doesn't work
//    f2.cbrt_assign(carl::CARL_RND::N);
//    ASSERT_EQ(result2, f2);

    // Todo: yet to be implemented!
//    f3.root_assign(i1, carl::CARL_RND::N);
//    ASSERT_EQ(result3, f3);
#ifndef NDEBUG
	// This may not fail if we are not compiling in DEBUG mode.
    ASSERT_DEATH(f4.sqrt_assign(carl::CARL_RND::N), ".*");
#endif
}

TEST(doubleFloatTest, ConversionOperators)
{
}

TEST(doubleFloatTest, Precision)
{
}

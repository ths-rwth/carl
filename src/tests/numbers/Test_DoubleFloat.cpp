#include "gtest/gtest.h"
#include "../../carl/numbers/FLOAT_T.h"

TEST(doubleFloatTest, Constructor)
{ 
    // double constructor tests
    double dVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(dVal));
    dVal = 0;
    EXPECT_EQ(0, carl::FLOAT_T<double>(dVal).getValue());
    
    // float constructor tests
    float fVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(fVal));
    fVal = 0;
    EXPECT_EQ(0, carl::FLOAT_T<double>(fVal).getValue());
    
    // integer constructor tests
    int iVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(iVal));
    iVal = 0;
    EXPECT_EQ(0, carl::FLOAT_T<double>(iVal).getValue());
    
    // mpfr constructor tests
    dVal = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899;
    carl::FLOAT_T<double> hf1 = carl::FLOAT_T<double>(dVal);
    
    // copy constructor test
    //ASSERT_EQ(hf1, carl::FLOAT_T<double>(hf1));
}

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

TEST(doubleFloatTest, Addition)
{
	double v1 = 7;
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    double v2 = 2;
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    
    double vResult = 9;
    carl::FLOAT_T<double> result = carl::FLOAT_T<double>(vResult);
    
    f1.add_assign(f2, carl::CARL_RNDN);
    ASSERT_EQ(result, f1);
}

TEST(doubleFloatTest, Subtraction)
{
	double v1 = 9;
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    double v2 = 5;
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    
    double vResult = 4;
    carl::FLOAT_T<double> result = carl::FLOAT_T<double>(vResult);
    
    f1.sub_assign(f2, carl::CARL_RNDN);
    ASSERT_EQ(result, f1);
}

TEST(doubleFloatTest, Multiplication)
{
    double v1 = 4;
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    double v2 = 3;
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    
    double vResult = 12;
    carl::FLOAT_T<double> result = carl::FLOAT_T<double>(vResult);
    
    f1.mul_assign(f2, carl::CARL_RNDN);
    ASSERT_EQ(result, f1);
}

TEST(doubleFloatTest, Division)
{
    double v1 = 8;
    carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
    double v2 = 4;
    carl::FLOAT_T<double> f2 = carl::FLOAT_T<double>(v2);
    
    double vResult = 2;
    carl::FLOAT_T<double> result = carl::FLOAT_T<double>(vResult);
    
    f1.div_assign(f2, carl::CARL_RNDN);
    ASSERT_EQ(result, f1);
}

TEST(doubleFloatTest, Roots)
{
	double v1 = 16;
	carl::FLOAT_T<double> f1 = carl::FLOAT_T<double>(v1);
}

TEST(doubleFloatTest, ConversionOperators)
{
}

TEST(doubleFloatTest, Precision)
{}

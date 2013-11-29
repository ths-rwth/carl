#include "gtest/gtest.h"
#include "../../carl/numbers/FLOAT_T.h"
#ifdef USE_MPFR_FLOAT

TEST(mpfrFloatTest, Constructor)
{ 
    // double constructor tests
    double dVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(dVal));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf2 = carl::FLOAT_T<mpfr_t>(dVal,10));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf3 = carl::FLOAT_T<mpfr_t>(dVal,10,carl::CARL_RNDA));
    dVal = 0;
    EXPECT_TRUE(mpfr_zero_p(carl::FLOAT_T<mpfr_t>(dVal).getValue()) != 0);
    
    // float constructor tests
    float fVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(fVal));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf2 = carl::FLOAT_T<mpfr_t>(fVal,10));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf3 = carl::FLOAT_T<mpfr_t>(fVal,10,carl::CARL_RNDA));
    fVal = 0;
    EXPECT_TRUE(mpfr_zero_p(carl::FLOAT_T<mpfr_t>(fVal).getValue()) != 0);
    
    // integer constructor tests
    int iVal = 4;
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(iVal));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf2 = carl::FLOAT_T<mpfr_t>(iVal,10));
    ASSERT_NO_THROW(carl::FLOAT_T<mpfr_t> hf3 = carl::FLOAT_T<mpfr_t>(iVal,10,carl::CARL_RNDA));
    iVal = 0;
    EXPECT_TRUE(mpfr_zero_p(carl::FLOAT_T<mpfr_t>(iVal).getValue()) != 0);
    
    // mpfr constructor tests
    mpfr_t mVal;
    mpfr_init(mVal);
    mpfr_set_d(mVal,  3.14159265358979323846264338327950288419716939937510582097494459230781640628620899, MPFR_RNDNA);
    carl::FLOAT_T<mpfr_t> hf1 = carl::FLOAT_T<mpfr_t>(mVal);
    std::cout << hf1 << std::endl;
    hf1.setPrec(10);
    EXPECT_EQ(10, hf1.getPrec());
    std::cout << hf1 << std::endl;
    
    // copy constructor test
    ASSERT_EQ(hf1, carl::FLOAT_T<mpfr_t>(hf1));
    
    SUCCEED();
}

TEST(mpfrFloatTest, BooleanOperators)
{
}

TEST(mpfrFloatTest, Addition)
{
    
}

TEST(mpfrFloatTest, Subtraction)
{
    
}

TEST(mpfrFloatTest, Multiplication)
{
    double v1 = 4;
    carl::FLOAT_T<mpfr_t> f1 = carl::FLOAT_T<mpfr_t>(v1);
    double v2 = 3;
    carl::FLOAT_T<mpfr_t> f2 = carl::FLOAT_T<mpfr_t>(v2);
    
    carl::FLOAT_T<mpfr_t> result;
    
    result = f1.mul_assign(f2,carl::CARL_RNDN);
    std::cout << f1 << std::endl;
}

TEST(mpfrFloatTest, Division)
{
    
}

TEST(mpfrFloatTest, Roots)
{
	
}

TEST(mpfrFloatTest, ConversionOperators)
{
}

TEST(mpfrFloatTest, Precision)
{}
#endif
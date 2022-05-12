#include "gtest/gtest.h"
#include <carl/numbers/numbers.h>

#include <type_traits>

using namespace carl;

TEST(GaloisField, integers)
{
	

    GaloisField<mpz_class> gf3(3,1);
    GaloisFieldManager<mpz_class>& gfm = GaloisFieldManager<mpz_class>::getInstance();
    
    const GaloisField<mpz_class>* gf5 = gfm.getField(5,1);
    GFNumber<mpz_class> a0(0,gf5);
    GFNumber<mpz_class> a1(1,gf5);
    GFNumber<mpz_class> a2(2,gf5);
    GFNumber<mpz_class> a3(3,gf5);
    GFNumber<mpz_class> a4(4,gf5);
    
    EXPECT_EQ(mpz_class(0),a0.representingInteger());
    EXPECT_EQ(mpz_class(1),a1.representingInteger());
    EXPECT_EQ(mpz_class(2),a2.representingInteger());
    EXPECT_EQ(mpz_class(-2),a3.representingInteger());
    EXPECT_EQ(mpz_class(-1),a4.representingInteger());
    
    EXPECT_TRUE(a0.isZero());
    EXPECT_FALSE(a2.isZero());
    EXPECT_TRUE(a1.isUnit());
    EXPECT_FALSE(a3.isUnit());
    
    EXPECT_EQ(a0, a0 + a0);
    EXPECT_EQ(a1, a1 + a0);
    EXPECT_EQ(a2, a2 + a0);
    EXPECT_EQ(a3, a3 + a0);
    EXPECT_EQ(a4, a4 + a0);
    
    EXPECT_EQ(a1, a0 + a1);
    EXPECT_EQ(a2, a1 + a1);
    EXPECT_EQ(a3, a2 + a1);
    EXPECT_EQ(a4, a3 + a1);
    EXPECT_EQ(a0, a4 + a1);
    
    EXPECT_EQ(a2, a0 + a2);
    EXPECT_EQ(a3, a1 + a2);
    EXPECT_EQ(a4, a2 + a2);
    EXPECT_EQ(a0, a3 + a2);
    EXPECT_EQ(a1, a4 + a2);
    
    EXPECT_EQ(a0, a0 * a0);
    EXPECT_EQ(a0, a1 * a0);
    EXPECT_EQ(a0, a2 * a0);
    EXPECT_EQ(a0, a3 * a0);
    EXPECT_EQ(a0, a4 * a0);
    
    EXPECT_EQ(a0, a0 * a1);
    EXPECT_EQ(a1, a1 * a1);
    EXPECT_EQ(a2, a2 * a1);
    EXPECT_EQ(a3, a3 * a1);
    EXPECT_EQ(a4, a4 * a1);
    
    EXPECT_EQ(a1, a1.inverse());
    EXPECT_EQ(a3, a2.inverse());
    EXPECT_EQ(a2, a3.inverse());
    EXPECT_EQ(a4, a4.inverse());
}




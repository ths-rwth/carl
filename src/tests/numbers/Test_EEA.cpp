#include "gtest/gtest.h"
#include <carl/numbers/numbers.h>
#include <carl/numbers/adaption_native/EEA.h>


using namespace carl;

TEST(Euclidean, RecursiveAlgorithm)
{
    std::pair<mpz_class, mpz_class> res;
    res = EEA<mpz_class>::calculate(19,8);
    EXPECT_EQ(3,res.first);
    EXPECT_EQ(-7,res.second);
    
    res = EEA<mpz_class>::calculate(65,40);
    EXPECT_EQ(-3,res.first);
    EXPECT_EQ(5,res.second);
}

#include <gmpxx.h>
#include "gtest/gtest.h"
#include "carl/numbers/numbers.h"
#include "carl/numbers/GFNumber.h"
#include "carl/util/SFINAE.h"

#include <type_traits>
#include <typeinfo>

using namespace carl;

TEST(Numbers, IntegralT)
{
	static_assert(std::is_same<mpz_class, IntegralT<mpq_class>::type>::value, "IntegralType of mpq_class should be mpz_class");
	static_assert(std::is_same<mpz_class, IntegralT<GFNumber<mpz_class>>::type>::value, "Should be the same");
	static_assert(is_instantiation_of<GFNumber,GFNumber<mpz_class>>::value, "Check whether the is_instantiaton works correctly");
	
}

TEST(Numbers, PrimeFactorizations)
{
    for(unsigned num = 0; num<=100; ++num)
    {
        std::vector<unsigned> pf = calculateFactorization(num);
        unsigned prod = (num == 0 ? 0 : 1);
        for(auto fac = pf.begin(); fac != pf.end(); ++fac)
        {
            prod *= *fac;
            EXPECT_EQ((unsigned)0,num%(*fac));
        }
        EXPECT_EQ(prod,num);
    }
}
	
 
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

TEST(Numbers, AllFactors)
{
    for(int num = 0; num<=100; ++num)
    {
        std::vector<int> pf = calculateAllFactors(num);
        for(auto fac = pf.begin(); fac != pf.end(); ++fac)
        {
            EXPECT_EQ(0,num%(*fac));
        }
    }
}
	
 
#include <gmpxx.h>
#include "gtest/gtest.h"
#include "carl/numbers/GFNumber.h"
#include "carl/util/SFINAE.h"

#include <type_traits>
#include <typeinfo>

using namespace carl;

TEST(Numbers, IntegralT)
{
	static_assert(is_instantiation_of<GFNumber,GFNumber<mpz_class>>::value, "Check whether the is_instantiaton works correctly");	
}
	
 

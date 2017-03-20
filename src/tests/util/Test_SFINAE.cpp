#include "../Common.h"

#include <carl/numbers/numbers.h>
#include <carl/util/SFINAE.h>

#include <type_traits>
#include <typeinfo>

using namespace carl;

TEST(Numbers, IntegralType)
{
	static_assert(is_instantiation_of<GFNumber,GFNumber<mpz_class>>::value, "Check whether the is_instantiaton works correctly");	
}
	
 

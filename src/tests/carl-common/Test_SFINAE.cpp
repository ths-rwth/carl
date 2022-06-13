#include "../Common.h"

#include <carl-arith/numbers/numbers.h>
#include <carl-common/meta/SFINAE.h>

#include <type_traits>
#include <typeinfo>

TEST(Numbers, IntegralType)
{
	static_assert(carl::is_instantiation_of<carl::GFNumber,carl::GFNumber<mpz_class>>::value, "Check whether the is_instantiaton works correctly");
}
	
 

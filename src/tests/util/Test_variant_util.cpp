#include "../Common.h"

#include <boost/variant.hpp>
#include <carl/util/variant_util.h>

struct A {};
struct B {};
struct C {};

TEST(VariantUtil, variant_is_type)
{
	boost::variant<A,B,C> v = A();
	EXPECT_TRUE(carl::variant_is_type<A>(v));
	EXPECT_FALSE(carl::variant_is_type<B>(v));
	EXPECT_FALSE(carl::variant_is_type<C>(v));
}

TEST(VariantUtil, variant_extend)
{
	boost::variant<A,B> v = A();
	boost::variant<A,B,C> res = carl::variant_extend<boost::variant<A,B,C>>(v);
	EXPECT_TRUE(carl::variant_is_type<A>(v));
	EXPECT_TRUE(carl::variant_is_type<A>(res));
}

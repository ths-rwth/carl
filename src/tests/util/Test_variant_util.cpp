#include <boost/variant.hpp>
#include <carl/util/variant_util.h>
#include <gtest/gtest.h>

struct A {};
struct B {};
struct C {};
struct D: A {};
struct E: D {};
struct F: B {};

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

TEST(VariantUtil, is_from_variant)
{
	constexpr bool ba = carl::is_from_variant<A, boost::variant<A,B,E>>::value;
	constexpr bool bb = carl::is_from_variant<B, boost::variant<A,B,E>>::value;
	constexpr bool bc = carl::is_from_variant<C, boost::variant<A,B,E>>::value;
	constexpr bool bd = carl::is_from_variant<D, boost::variant<A,B,E>>::value;
	constexpr bool be = carl::is_from_variant<E, boost::variant<A,B,E>>::value;
	constexpr bool bf = carl::is_from_variant<F, boost::variant<A,B,E>>::value;
	EXPECT_TRUE(ba);
	EXPECT_TRUE(bb);
	EXPECT_FALSE(bc);
	EXPECT_FALSE(bd);
	EXPECT_TRUE(be);
	EXPECT_FALSE(bf);
}

TEST(VariantUtil, convertible_to_variant)
{
	constexpr bool ba = carl::convertible_to_variant<A, boost::variant<A,B,E>>::value;
	constexpr bool bb = carl::convertible_to_variant<B, boost::variant<A,B,E>>::value;
	constexpr bool bc = carl::convertible_to_variant<C, boost::variant<A,B,E>>::value;
	constexpr bool bd = carl::convertible_to_variant<D, boost::variant<A,B,E>>::value;
	constexpr bool be = carl::convertible_to_variant<E, boost::variant<A,B,E>>::value;
	constexpr bool bf = carl::convertible_to_variant<F, boost::variant<A,B,E>>::value;
	EXPECT_TRUE(ba);
	EXPECT_TRUE(bb);
	EXPECT_FALSE(bc);
	EXPECT_TRUE(bd);
	EXPECT_TRUE(be);
	EXPECT_TRUE(bf);
}

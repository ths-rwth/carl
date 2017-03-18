#include "gtest/gtest.h"

#include <carl/numbers/numbers.h>

using namespace carl;

TEST(typetraits, is_field)
{
	EXPECT_FALSE(carl::is_field<bool>::value);
	EXPECT_FALSE(carl::is_field<int>::value);
	EXPECT_FALSE(carl::is_field<double>::value);
	EXPECT_FALSE(carl::is_field<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_field<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_field<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_field<mpz_class>::value);
	EXPECT_TRUE(carl::is_field<mpq_class>::value);
}

TEST(typetraits, is_finite)
{
	EXPECT_TRUE(carl::is_finite<bool>::value);
	EXPECT_TRUE(carl::is_finite<int>::value);
	EXPECT_TRUE(carl::is_finite<double>::value);
	EXPECT_FALSE(carl::is_finite<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_finite<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_finite<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_finite<mpz_class>::value);
	EXPECT_FALSE(carl::is_finite<mpq_class>::value);
}

TEST(typetraits, is_float)
{
	EXPECT_FALSE(carl::is_float<bool>::value);
	EXPECT_FALSE(carl::is_float<int>::value);
	EXPECT_TRUE(carl::is_float<double>::value);
	EXPECT_FALSE(carl::is_float<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_float<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_float<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_float<mpz_class>::value);
	EXPECT_FALSE(carl::is_float<mpq_class>::value);
}

TEST(typetraits, is_fundamental)
{
	EXPECT_TRUE(std::is_fundamental<bool>::value);
	EXPECT_TRUE(std::is_fundamental<int>::value);
	EXPECT_TRUE(std::is_fundamental<double>::value);
	EXPECT_FALSE(std::is_fundamental<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(std::is_fundamental<cln::cl_I>::value);
	EXPECT_FALSE(std::is_fundamental<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(std::is_fundamental<mpz_class>::value);
	EXPECT_FALSE(std::is_fundamental<mpq_class>::value);
}

TEST(typetraits, is_integer)
{
	EXPECT_FALSE(carl::is_integer<bool>::value);
	EXPECT_FALSE(carl::is_integer<int>::value);
	EXPECT_FALSE(carl::is_integer<double>::value);
	EXPECT_FALSE(carl::is_integer<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_TRUE(carl::is_integer<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_integer<cln::cl_RA>::value);
	#endif
	EXPECT_TRUE(carl::is_integer<mpz_class>::value);
	EXPECT_FALSE(carl::is_integer<mpq_class>::value);
}

TEST(typetraits, is_subset_of_integers)
{
	EXPECT_FALSE(carl::is_subset_of_integers<bool>::value);
	EXPECT_TRUE(carl::is_subset_of_integers<int>::value);
	EXPECT_FALSE(carl::is_subset_of_integers<double>::value);
	EXPECT_FALSE(carl::is_subset_of_integers<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_TRUE(carl::is_subset_of_integers<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_subset_of_integers<cln::cl_RA>::value);
	#endif
	EXPECT_TRUE(carl::is_subset_of_integers<mpz_class>::value);
	EXPECT_FALSE(carl::is_subset_of_integers<mpq_class>::value);
}

TEST(typetraits, is_number)
{
	EXPECT_FALSE(carl::is_number<bool>::value);
	EXPECT_TRUE(carl::is_number<int>::value);
	EXPECT_TRUE(carl::is_number<double>::value);
	EXPECT_FALSE(carl::is_number<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_TRUE(carl::is_number<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_number<cln::cl_RA>::value);
	#endif
	EXPECT_TRUE(carl::is_number<mpz_class>::value);
	EXPECT_TRUE(carl::is_number<mpq_class>::value);
}

TEST(typetraits, is_rational)
{
	EXPECT_FALSE(carl::is_rational<bool>::value);
	EXPECT_FALSE(carl::is_rational<int>::value);
	EXPECT_FALSE(carl::is_rational<double>::value);
	EXPECT_FALSE(carl::is_rational<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_rational<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_rational<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_rational<mpz_class>::value);
	EXPECT_TRUE(carl::is_rational<mpq_class>::value);
}

TEST(typetraits, is_subset_of_rationals)
{
	EXPECT_FALSE(carl::is_subset_of_rationals<bool>::value);
	EXPECT_FALSE(carl::is_subset_of_rationals<int>::value);
	EXPECT_FALSE(carl::is_subset_of_rationals<double>::value);
	EXPECT_FALSE(carl::is_subset_of_rationals<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_subset_of_rationals<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_subset_of_rationals<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_subset_of_rationals<mpz_class>::value);
	EXPECT_TRUE(carl::is_subset_of_rationals<mpq_class>::value);
}

TEST(typetraits, IntegralType)
{
	// Note: These checks are performed at compile time.
	::testing::StaticAssertTypeEq<sint, carl::IntegralType<bool>::type>();
	::testing::StaticAssertTypeEq<sint, carl::IntegralType<int>::type>();
	::testing::StaticAssertTypeEq<sint, carl::IntegralType<double>::type>();
	::testing::StaticAssertTypeEq<sint, carl::IntegralType<std::string>::type>();
	#ifdef USE_CLN_NUMBERS
	::testing::StaticAssertTypeEq<cln::cl_I, carl::IntegralType<cln::cl_I>::type>();
	::testing::StaticAssertTypeEq<cln::cl_I, carl::IntegralType<cln::cl_RA>::type>();
	#endif
	::testing::StaticAssertTypeEq<mpz_class, carl::IntegralType<mpz_class>::type>();
	::testing::StaticAssertTypeEq<mpz_class, carl::IntegralType<mpq_class>::type>();
}

TEST(typetraits, UnderlyingNumberType)
{
	// Note: These checks are performed at compile time.
	::testing::StaticAssertTypeEq<bool, carl::UnderlyingNumberType<bool>::type>();
	::testing::StaticAssertTypeEq<int, carl::UnderlyingNumberType<int>::type>();
	::testing::StaticAssertTypeEq<double, carl::UnderlyingNumberType<double>::type>();
	#ifdef USE_CLN_NUMBERS
	::testing::StaticAssertTypeEq<cln::cl_I, carl::UnderlyingNumberType<cln::cl_I>::type>();
	::testing::StaticAssertTypeEq<cln::cl_RA, carl::UnderlyingNumberType<cln::cl_RA>::type>();
	#endif
	::testing::StaticAssertTypeEq<mpz_class, carl::UnderlyingNumberType<mpz_class>::type>();
	::testing::StaticAssertTypeEq<mpq_class, carl::UnderlyingNumberType<mpq_class>::type>();
}

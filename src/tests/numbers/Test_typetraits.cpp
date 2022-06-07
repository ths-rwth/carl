#include <carl-arith/numbers/numbers.h>
#include <gtest/gtest.h>

using namespace carl;

TEST(typetraits, is_field_type)
{
	EXPECT_FALSE(carl::is_field_type<bool>::value);
	EXPECT_FALSE(carl::is_field_type<int>::value);
	EXPECT_FALSE(carl::is_field_type<double>::value);
	EXPECT_FALSE(carl::is_field_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_field_type<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_field_type<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_field_type<mpz_class>::value);
	EXPECT_TRUE(carl::is_field_type<mpq_class>::value);
}

TEST(typetraits, is_finite_type)
{
	EXPECT_TRUE(carl::is_finite_type<bool>::value);
	EXPECT_TRUE(carl::is_finite_type<int>::value);
	EXPECT_TRUE(carl::is_finite_type<double>::value);
	EXPECT_FALSE(carl::is_finite_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_finite_type<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_finite_type<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_finite_type<mpz_class>::value);
	EXPECT_FALSE(carl::is_finite_type<mpq_class>::value);
}

TEST(typetraits, is_float_type)
{
	EXPECT_FALSE(carl::is_float_type<bool>::value);
	EXPECT_FALSE(carl::is_float_type<int>::value);
	EXPECT_TRUE(carl::is_float_type<double>::value);
	EXPECT_FALSE(carl::is_float_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_float_type<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_float_type<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_float_type<mpz_class>::value);
	EXPECT_FALSE(carl::is_float_type<mpq_class>::value);
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

TEST(typetraits, is_integer_type)
{
	EXPECT_FALSE(carl::is_integer_type<bool>::value);
	EXPECT_FALSE(carl::is_integer_type<int>::value);
	EXPECT_FALSE(carl::is_integer_type<double>::value);
	EXPECT_FALSE(carl::is_integer_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_TRUE(carl::is_integer_type<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_integer_type<cln::cl_RA>::value);
	#endif
	EXPECT_TRUE(carl::is_integer_type<mpz_class>::value);
	EXPECT_FALSE(carl::is_integer_type<mpq_class>::value);
}

TEST(typetraits, is_subset_of_integers_type)
{
	EXPECT_FALSE(carl::is_subset_of_integers_type<bool>::value);
	EXPECT_TRUE(carl::is_subset_of_integers_type<int>::value);
	EXPECT_FALSE(carl::is_subset_of_integers_type<double>::value);
	EXPECT_FALSE(carl::is_subset_of_integers_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_TRUE(carl::is_subset_of_integers_type<cln::cl_I>::value);
	EXPECT_FALSE(carl::is_subset_of_integers_type<cln::cl_RA>::value);
	#endif
	EXPECT_TRUE(carl::is_subset_of_integers_type<mpz_class>::value);
	EXPECT_FALSE(carl::is_subset_of_integers_type<mpq_class>::value);
}

TEST(typetraits, is_number_type)
{
	EXPECT_FALSE(carl::is_number_type<bool>::value);
	EXPECT_TRUE(carl::is_number_type<int>::value);
	EXPECT_TRUE(carl::is_number_type<double>::value);
	EXPECT_FALSE(carl::is_number_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_TRUE(carl::is_number_type<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_number_type<cln::cl_RA>::value);
	#endif
	EXPECT_TRUE(carl::is_number_type<mpz_class>::value);
	EXPECT_TRUE(carl::is_number_type<mpq_class>::value);
}

TEST(typetraits, is_rational_type)
{
	EXPECT_FALSE(carl::is_rational_type<bool>::value);
	EXPECT_FALSE(carl::is_rational_type<int>::value);
	EXPECT_FALSE(carl::is_rational_type<double>::value);
	EXPECT_FALSE(carl::is_rational_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_rational_type<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_rational_type<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_rational_type<mpz_class>::value);
	EXPECT_TRUE(carl::is_rational_type<mpq_class>::value);
}

TEST(typetraits, is_subset_of_rationals_type)
{
	EXPECT_FALSE(carl::is_subset_of_rationals_type<bool>::value);
	EXPECT_FALSE(carl::is_subset_of_rationals_type<int>::value);
	EXPECT_FALSE(carl::is_subset_of_rationals_type<double>::value);
	EXPECT_FALSE(carl::is_subset_of_rationals_type<std::string>::value);
	#ifdef USE_CLN_NUMBERS
	EXPECT_FALSE(carl::is_subset_of_rationals_type<cln::cl_I>::value);
	EXPECT_TRUE(carl::is_subset_of_rationals_type<cln::cl_RA>::value);
	#endif
	EXPECT_FALSE(carl::is_subset_of_rationals_type<mpz_class>::value);
	EXPECT_TRUE(carl::is_subset_of_rationals_type<mpq_class>::value);
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

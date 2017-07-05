#include "../Common.h"

#include <carl/numbers/FunctionSelector.h>

TEST(FunctionSelector, TypeSelector)
{
	static_assert(std::is_same<carl::UnderlyingNumberType<mpq_class>::type, mpq_class>::value, "Test1");
	static_assert(std::is_same<carl::UnderlyingNumberType<carl::MultivariatePolynomial<mpq_class>>::type, mpq_class>::value, "Test2");
	
	using TypeSelector = carl::function_selector::NaryTypeSelector;
	
	static_assert(std::is_same<TypeSelector::template type<mpq_class>, mpq_class>::value, "Test1");
	static_assert(std::is_same<TypeSelector::template type<carl::MultivariatePolynomial<mpq_class>>, mpq_class>::value, "Test2");
}

TEST(FunctionSelector, basic)
{
	using types = carl::function_selector::wrap_types<mpz_class,mpq_class>;
	using TypeSelector = carl::function_selector::NaryTypeSelector;
	auto s = carl::createFunctionSelector<TypeSelector, types>(
		[](const mpz_class& n){ return carl::isZero(n); },
		[](const mpq_class& n){ return carl::isZero(n + 1); }
	);
	
	EXPECT_EQ(false, s(mpz_class(-1)));
	EXPECT_EQ(true, s(mpz_class(0)));
	EXPECT_EQ(false, s(mpz_class(1)));
	EXPECT_EQ(true, s(mpq_class(-1)));
	EXPECT_EQ(false, s(mpq_class(0)));
	EXPECT_EQ(false, s(mpq_class(1)));
}

TEST(FunctionSelector, binary)
{
	using TypeSelector = carl::function_selector::NaryTypeSelector;
	using types = carl::function_selector::wrap_types<mpz_class,mpq_class>;
	auto s = carl::createFunctionSelector<TypeSelector, types>(
		[](const mpz_class& n1, const mpz_class& n2){ return carl::isZero(mpz_class(n1 + n2)); },
		[](const mpq_class& n1, const mpq_class& n2){ return carl::isZero(n1 + n2 + 2); }
	);
	
	EXPECT_EQ(false, s(mpz_class(-1), mpz_class(-1)));
	EXPECT_EQ(true, s(mpz_class(0), mpz_class(0)));
	EXPECT_EQ(false, s(mpz_class(1), mpz_class(1)));
	EXPECT_EQ(true, s(mpq_class(-1), mpq_class(-1)));
	EXPECT_EQ(false, s(mpq_class(0), mpq_class(0)));
	EXPECT_EQ(false, s(mpq_class(1), mpq_class(1)));
}

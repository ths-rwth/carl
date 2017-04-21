#include "../Common.h"

#include <carl/numbers/FunctionSelector.h>

TEST(FunctionSelector, basic)
{
	using types = carl::function_selector::wrap_types<mpz_class,mpq_class>;
	using TypeSelector = carl::function_selector::NaryTypeSelector;
	auto s = carl::createFunctionSelector<TypeSelector, types>(
		[](const mpz_class& n){ return carl::isZero(n); },
		[](const mpq_class& n){ return carl::isZero(n + 1); }
	);
	
	std::cout << mpz_class(-1) << " -> " << s(mpz_class(-1)) << std::endl;
	std::cout << mpz_class(0) << " -> " << s(mpz_class(0)) << std::endl;
	std::cout << mpz_class(1) << " -> " << s(mpz_class(1)) << std::endl;
	std::cout << mpq_class(-1) << " -> " << s(mpq_class(-1)) << std::endl;
	std::cout << mpq_class(0) << " -> " << s(mpq_class(0)) << std::endl;
	std::cout << mpq_class(1) << " -> " << s(mpq_class(1)) << std::endl;
}

TEST(FunctionSelector, binary)
{
	using TypeSelector = carl::function_selector::NaryTypeSelector;
	using types = carl::function_selector::wrap_types<mpz_class,mpq_class>;
	auto s = carl::createFunctionSelector<TypeSelector, types>(
		[](const mpz_class& n1, const mpz_class& n2){ return carl::isZero(mpz_class(n1 + n2)); },
		[](const mpq_class& n1, const mpq_class& n2){ return carl::isZero(n1 + n2 + 2); }
	);
	
	std::cout << mpz_class(-1) << " -> " << s(mpz_class(-1), mpz_class(-1)) << std::endl;
	std::cout << mpz_class(0) << " -> " << s(mpz_class(0), mpz_class(0)) << std::endl;
	std::cout << mpz_class(1) << " -> " << s(mpz_class(1), mpz_class(1)) << std::endl;
	std::cout << mpq_class(-1) << " -> " << s(mpq_class(-1), mpq_class(-1)) << std::endl;
	std::cout << mpq_class(0) << " -> " << s(mpq_class(0), mpq_class(0)) << std::endl;
	std::cout << mpq_class(1) << " -> " << s(mpq_class(1), mpq_class(1)) << std::endl;
}

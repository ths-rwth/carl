#include <gtest/gtest.h>

#include "../../carl/config.h"
#include "../Common.h"
#include "carl/core/VariablePool.h"

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

#include "carl/converter/LibpolyFunctions.h"
#include "carl/poly/lp/LPContext.h"
#include "carl/poly/lp/LPPolynomial.h"

using namespace carl;

TEST(LPPOLYNOMIAL, createContext) {
	auto x = freshRealVariable("x");
	auto y = freshRealVariable("y");
	auto z = freshRealVariable("z");
	std::vector<Variable> var_order1 = {x, y, z};
	std::vector<Variable> var_order2 = {x, y};
	std::vector<Variable> var_order3 = {x, y};

	LPContext ctx1(var_order1);
	LPContext ctx2(var_order2);
	LPContext ctx2_2(var_order2);
	LPContext ctx3(var_order3);

	EXPECT_EQ(ctx1.getVariableOrder(), var_order1);
	EXPECT_EQ(ctx2.getVariableOrder(), var_order2);

	EXPECT_FALSE(ctx1 == ctx2);
	EXPECT_TRUE(ctx2 == ctx3);
	EXPECT_TRUE(ctx2 == ctx2_2);
}

TEST(LPPOLYNOMIAL, createPoly) {
	auto x = freshRealVariable("x");
	auto y = freshRealVariable("y");
	auto z = freshRealVariable("z");
	std::vector<Variable> var_order1 = {x, y, z};
	LPContext ctx(var_order1);

	std::initializer_list<mpz_class> list = {1, 0, 0, -3, 1, 0, 3, 0};
	std::map<std::uint64_t, mpz_class> map = {
		{0, 0},
		{1, 3},
		{2, 0},
		{3, 1},
		{4, -3},
		{5, 0},
		{6, 0},
		{7, 1},

	};

	std::cout << LPPolynomial(ctx) << std::endl;
	std::cout << LPPolynomial(long(17), ctx) << std::endl;
	std::cout << LPPolynomial(ctx, x) << std::endl;
	std::cout << LPPolynomial(ctx, x, 7, 2) << std::endl;
	std::cout << LPPolynomial(x, ctx, list) << std::endl;
	std::cout << LPPolynomial(x, ctx, std::vector<mpz_class>(list)) << std::endl;
	std::cout << LPPolynomial(x, ctx, map) << std::endl;

	EXPECT_EQ(LPPolynomial(x, ctx, list), LPPolynomial(x, ctx, std::vector<mpz_class>(list)));
    EXPECT_EQ(LPPolynomial(long(17), ctx), mpz_class(17)) ;
	EXPECT_EQ(LPPolynomial(x, ctx, list), LPPolynomial(x, ctx, map)) ;
}   

#endif
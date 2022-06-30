#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <gtest/gtest.h>

#include "../Common.h"
#include <carl-arith/core/VariablePool.h>

#include <carl-arith/ran/ran.h>
#include <carl-arith/ran/real_roots.h>

using namespace carl;

TEST(LIBPOLY, createVars) {
	poly::Variable var_x = poly::Variable("x");
	poly::Variable var_y = poly::Variable("y");
	EXPECT_FALSE(var_x == var_y);
}

TEST(LIBPOLY, convertVariables) {
	auto carl_var = fresh_real_variable("x");
	auto var_x = VariableMapper::getInstance().getLibpolyVariable(carl_var);
	auto var_y = VariableMapper::getInstance().getLibpolyVariable(fresh_real_variable("y"));
	auto test = VariableMapper::getInstance().getCarlVariable(var_x); //convert back

	EXPECT_FALSE(var_x == var_y);
	EXPECT_TRUE(carl_var == test);
}

TEST(LIBPOLY, convertFromLibpolyMultivariate) {
	auto x = fresh_real_variable("x");
	auto y = fresh_real_variable("y");
	auto lp_x = VariableMapper::getInstance().getLibpolyVariable(x);
	auto lp_y = VariableMapper::getInstance().getLibpolyVariable(y);

	poly::Polynomial poly_x(lp_x);
	poly::Polynomial poly_y(lp_y);

	auto lp_poly1 = poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly1({(mpq_class)1 * x * y * y, (mpq_class)1 * x});

	EXPECT_EQ(carl_poly1, to_carl_multivariate_polynomial(lp_poly1));

	auto lp_poly2 = poly::Polynomial(poly::Integer(24), lp_x, (unsigned)3) * poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly2({(mpq_class)24 * x * x * x * x * y * y, (mpq_class)1 * x});
	EXPECT_EQ(carl_poly2, to_carl_multivariate_polynomial(lp_poly2));

	mpz_class denom(12);
	EXPECT_LE(to_carl_multivariate_polynomial(lp_poly2, denom), carl_poly2);
}

TEST(LIBPOLY, convertToLibpolyMultivariate) {
	auto x = fresh_real_variable("x");
	auto y = fresh_real_variable("y");
	auto lp_x = VariableMapper::getInstance().getLibpolyVariable(x);
	auto lp_y = VariableMapper::getInstance().getLibpolyVariable(y);

	poly::Polynomial poly_x(lp_x);
	poly::Polynomial poly_y(lp_y);

	auto lp_poly1 = poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly1({(mpq_class)1 * x * y * y, (mpq_class)1 * x});

	EXPECT_EQ(lp_poly1, to_libpoly_polynomial(carl_poly1));

	auto lp_poly2 = poly::Polynomial(poly::Integer(24), lp_x, (unsigned)3) * poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly2({(mpq_class)24 * x * x * x * x * y * y, (mpq_class)1 * x});

	EXPECT_EQ(lp_poly2, to_libpoly_polynomial(carl_poly2));

	carl::MultivariatePolynomial<mpq_class> carl_poly3({mpq_class(1, 14) * x * x * y * y, mpq_class(1, 2) * x, mpq_class(1, 8) * y});
	mpz_class den;
	auto lp_poly3 = to_libpoly_polynomial(carl_poly3, den);

	EXPECT_EQ(den, mpz_class(56));

	carl::MultivariatePolynomial<Rational> carl_poly4(2);

	EXPECT_EQ(to_libpoly_polynomial(carl_poly4), poly::Polynomial(poly::Integer(2)));

	carl::MultivariatePolynomial<Rational> carl_poly5(mpq_class(1, 4));

	EXPECT_EQ(to_libpoly_polynomial(carl_poly5), poly::Polynomial(poly::Integer(1)));
}

TEST(LIBPOLY, convertToLibpolyUnivariate) {
	auto x = fresh_real_variable("x");

	UnivariatePolynomial<Rational> carl_pol1(x, {Rational(-6), Rational(-5), Rational(1)});
	poly::UPolynomial lp_pol1({-6, -5, 1});

	EXPECT_EQ(lp_pol1, to_libpoly_upolynomial(carl_pol1));

	UnivariatePolynomial<Rational> carl_pol2(x, {mpq_class(2, 13)});
	mpz_class den;
	to_libpoly_upolynomial(carl_pol2, den);
	EXPECT_EQ(den, mpz_class(13));
}

TEST(LIBPOLY, convertToCarlUnivariate) {
	auto x = fresh_real_variable("x");

	UnivariatePolynomial<Rational> carl_pol1(x, {Rational(-6), Rational(-5), Rational(1)});
	poly::UPolynomial lp_pol1({-6, -5, 1});

	EXPECT_EQ(carl_pol1, to_carl_univariate_polynomial(lp_pol1, x));
}

#endif
#include <gtest/gtest.h>

#include "../Common.h"
#include <carl/core/VariablePool.h>

#if defined(USE_LIBPOLY) || defined(RAN_USE_LIBPOLY)

#include <carl/converter/LibpolyFunctions.h>
#include <carl/ran/ran.h>
#include <carl/ran/real_roots.h>

using namespace carl;

TEST(LIBPOLY, createVars) {
	poly::Variable var_x = poly::Variable("x");
	poly::Variable var_y = poly::Variable("y");
	EXPECT_FALSE(var_x == var_y);
}

TEST(LIBPOLY, convertVariables) {
	auto carl_var = freshRealVariable("x");
	auto var_x = VariableMapper::getInstance().getLibpolyVariable(carl_var);
	auto var_y = VariableMapper::getInstance().getLibpolyVariable(freshRealVariable("y"));
	auto test = VariableMapper::getInstance().getCarlVariable(var_x); //convert back

	EXPECT_FALSE(var_x == var_y);
	EXPECT_TRUE(carl_var == test);
}

TEST(LIBPOLY, convertFromLibpolyMultivariate) {
	auto x = freshRealVariable("x");
	auto y = freshRealVariable("y");
	auto lp_x = VariableMapper::getInstance().getLibpolyVariable(x);
	auto lp_y = VariableMapper::getInstance().getLibpolyVariable(y);

	poly::Polynomial poly_x(lp_x);
	poly::Polynomial poly_y(lp_y);

	auto lp_poly1 = poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly1({(mpq_class)1 * x * y * y, (mpq_class)1 * x});

	EXPECT_EQ(carl_poly1, LibpolyConverter::getInstance().toCarlMultivariatePolynomial(lp_poly1));

	auto lp_poly2 = poly::Polynomial(poly::Integer(24), lp_x, (unsigned)3) * poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly2({(mpq_class)24 * x * x * x * x * y * y, (mpq_class)1 * x});
	EXPECT_EQ(carl_poly2, LibpolyConverter::getInstance().toCarlMultivariatePolynomial(lp_poly2));

	mpz_class denom(12);
	EXPECT_LE(LibpolyConverter::getInstance().toCarlMultivariatePolynomial(lp_poly2, denom), carl_poly2);
}

TEST(LIBPOLY, convertToLibpolyMultivariate) {
	auto x = freshRealVariable("x");
	auto y = freshRealVariable("y");
	auto lp_x = VariableMapper::getInstance().getLibpolyVariable(x);
	auto lp_y = VariableMapper::getInstance().getLibpolyVariable(y);

	poly::Polynomial poly_x(lp_x);
	poly::Polynomial poly_y(lp_y);

	auto lp_poly1 = poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly1({(mpq_class)1 * x * y * y, (mpq_class)1 * x});

	EXPECT_EQ(lp_poly1, LibpolyConverter::getInstance().toLibpolyPolynomial(carl_poly1));

	auto lp_poly2 = poly::Polynomial(poly::Integer(24), lp_x, (unsigned)3) * poly_x * poly_y * poly_y + poly_x;
	carl::MultivariatePolynomial<mpq_class> carl_poly2({(mpq_class)24 * x * x * x * x * y * y, (mpq_class)1 * x});

	EXPECT_EQ(lp_poly2, LibpolyConverter::getInstance().toLibpolyPolynomial(carl_poly2));

	carl::MultivariatePolynomial<mpq_class> carl_poly3({mpq_class(1, 14) * x * x * y * y, mpq_class(1, 2) * x, mpq_class(1, 8) * y});
	mpz_class den;
	auto lp_poly3 = LibpolyConverter::getInstance().toLibpolyPolynomial(carl_poly3, den);

	EXPECT_EQ(den, mpz_class(56));

	carl::MultivariatePolynomial<Rational> carl_poly4(2);

	EXPECT_EQ(LibpolyConverter::getInstance().toLibpolyPolynomial(carl_poly4), poly::Polynomial(poly::Integer(2)));

	carl::MultivariatePolynomial<Rational> carl_poly5(mpq_class(1, 4));

	EXPECT_EQ(LibpolyConverter::getInstance().toLibpolyPolynomial(carl_poly5), poly::Polynomial(poly::Integer(1)));
}

TEST(LIBPOLY, convertToLibpolyUnivariate) {
	auto x = freshRealVariable("x");

	UnivariatePolynomial<Rational> carl_pol1(x, {Rational(-6), Rational(-5), Rational(1)});
	poly::UPolynomial lp_pol1({-6, -5, 1});

	EXPECT_EQ(lp_pol1, LibpolyConverter::getInstance().toLibpolyUPolynomial(carl_pol1));

	UnivariatePolynomial<Rational> carl_pol2(x, {mpq_class(2, 13)});
	mpz_class den;
	LibpolyConverter::getInstance().toLibpolyUPolynomial(carl_pol2, den);
	EXPECT_EQ(den, mpz_class(13));
}

TEST(LIBPOLY, convertToCarlUnivariate) {
	auto x = freshRealVariable("x");

	UnivariatePolynomial<Rational> carl_pol1(x, {Rational(-6), Rational(-5), Rational(1)});
	poly::UPolynomial lp_pol1({-6, -5, 1});

	EXPECT_EQ(carl_pol1, LibpolyConverter::getInstance().toCarlUnivariatePolynomial(lp_pol1, x));
}

TEST(LIBPOLY, variableOrder) {
	auto x = freshRealVariable("x");
	auto y = freshRealVariable("y");
	auto lp_x = VariableMapper::getInstance().getLibpolyVariable(x);
	auto lp_y = VariableMapper::getInstance().getLibpolyVariable(y);

	poly::Polynomial poly_x(lp_x);
	poly::Polynomial poly_y(lp_y);

	auto lp_poly1 = poly_x * poly_y * poly_y + poly_x;

	lp_polynomial_set_external(lp_poly1.get_internal());

	VariableMapper::getInstance().setLibpolyMainVariable(y);
	EXPECT_EQ(lp_y, poly::main_variable(lp_poly1)) ;

	VariableMapper::getInstance().setLibpolyMainVariable(y);
	EXPECT_EQ(lp_y, poly::main_variable(lp_poly1));

	VariableMapper::getInstance().setLibpolyMainVariable(x);
	EXPECT_EQ(lp_x, poly::main_variable(lp_poly1)); 

	carl::MultivariatePolynomial<mpq_class> carl_poly2({mpq_class(1, 14) * x * x * y * y, mpq_class(1, 2) * x, mpq_class(1, 8) * y});
	auto lp_poly2 = LibpolyConverter::getInstance().toLibpolyPolynomial(carl_poly2);

	lp_polynomial_ensure_order(lp_poly2.get_internal());

	EXPECT_EQ(lp_x, poly::main_variable(lp_poly2)); 

	VariableMapper::getInstance().setLibpolyMainVariable(y);
	lp_polynomial_ensure_order(lp_poly2.get_internal());

	EXPECT_EQ(VariableMapper::getInstance().getLibpolyVariable(y), poly::main_variable(lp_poly2)); 
}

TEST(LIBPOLY, nullificationBug){
	auto x0 = freshRealVariable("x0");
	auto x1 = freshRealVariable("x1");

	carl::MultivariatePolynomial<mpq_class> carl_poly({ carl::Term<mpq_class>(1) , mpq_class(-1) * x0 * x1});
	std::map<Variable, real_algebraic_number_libpoly<mpq_class>> assignment ; 
	assignment[x0] = real_algebraic_number_libpoly<mpq_class>(0) ;

	auto roots = carl::ran::real_roots(carl::to_univariate_polynomial(carl_poly, x1), assignment);
	
	EXPECT_FALSE(roots.is_nullified());
	EXPECT_EQ(roots.roots().size(), 0);

}

TEST(LIBPOLY, nullificationBug2){
	auto skoX = freshRealVariable("skoX");
	auto skoY = freshRealVariable("skoY");
	auto skoZ = freshRealVariable("skoZ");

	// -1*skoX + -1*skoY + -1*skoZ + skoX*skoY*skoZ 
	carl::MultivariatePolynomial<mpq_class> carl_poly({mpq_class(-1) * skoX , mpq_class(-1) * skoY , mpq_class(-1) * skoZ , mpq_class(1) * skoX * skoY * skoZ });
	//{skoX : -3/4, skoY : -1/2}
	std::map<Variable, real_algebraic_number_libpoly<mpq_class>> assignment ; 
	assignment[skoX] = real_algebraic_number_libpoly<mpq_class>(mpq_class(-3,4)) ;
	assignment[skoY] = real_algebraic_number_libpoly<mpq_class>(mpq_class(-1,2)) ;


	auto roots = carl::ran::real_roots(carl::to_univariate_polynomial(carl_poly, skoZ), assignment);
	
	std::cout << roots.roots() << std::endl;

	EXPECT_FALSE(roots.is_nullified());
	EXPECT_FALSE(roots.roots().empty());

}

#endif
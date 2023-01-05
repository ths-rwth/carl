#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <gtest/gtest.h>

#include <random>

#include "../Common.h"
#include <carl-arith/core/VariablePool.h>

#include <carl-arith/ran/ran.h>

#include <carl-arith/ran/Conversion.h>

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
    auto test = VariableMapper::getInstance().getCarlVariable(var_x); // convert back

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

TEST(LIBPOLY, convertRan) {
    Variable x = fresh_real_variable("x");
    std::vector<Variable> var_order = {x};
    LPContext context(var_order);

    LPPolynomial linA(context, x, {(mpz_class)-2, (mpz_class)5});
    LPPolynomial linB(context, x, {(mpz_class)1, (mpz_class)3});
    LPPolynomial linC(context, x, {(mpz_class)2014, (mpz_class)68});
    LPPolynomial linD(context, x, {(mpz_class)-13, (mpz_class)4});
    LPPolynomial linE(context, x, {(mpz_class)-20, (mpz_class)5});
    LPPolynomial quaA(context, x, {(mpz_class)-2, (mpz_class)0, (mpz_class)1});
    LPPolynomial quaB(context, x, {(mpz_class)3, (mpz_class)6, (mpz_class)9});

    LPPolynomial polA(context, x, {(mpz_class)-2, (mpz_class)5, (mpz_class)-5, (mpz_class)3});
    LPPolynomial polB = linA * linA;
    LPPolynomial polC = linA * linA * linA;
    LPPolynomial polD = linA * linB * linC * linD * linE;
    LPPolynomial polE = quaA * linC * linD * linE;
    LPPolynomial polF = linA * quaB * linE;
    LPPolynomial polG = quaA * quaB * linD * linE;
    LPPolynomial polH = polA * quaB * linD;
    LPPolynomial polI = linA * linA * quaA * quaA * quaB * quaB * quaB * quaB;

    std::vector<LPPolynomial> polys = {linA, linC, linE, quaA, quaB, polA, polB, polC, polD, polE, polF, polG, polH, polI};

    unsigned seed = 21344325;
    std::default_random_engine generator(seed);
    for (int e = 2; e <= 20; ++e) {
        std::cauchy_distribution<double> distribution(0.0, 3.0 + 10.0 * std::pow(0.9, e));
        for (int j = 0; j < 10; ++j) {
            std::vector<mpz_class> coeffs;
            for (int i = 0; i <= e; ++i) {
                double ran = distribution(generator);
                int coe = (ran > INT_MAX || ran < INT_MIN) ? 0 : (int)ran;
                coeffs.push_back((mpz_class)coe);
            }
            if (coeffs.back() == (mpz_class)0) {
                coeffs.back() = (mpz_class)1;
            }
            LPPolynomial randomPol(context, x, coeffs);
            polys.push_back(randomPol);
        }
    }

    for (const carl::LPPolynomial& pol : polys) {
        std::vector<LPRealAlgebraicNumber> roots = real_roots(pol).roots();
		CARL_LOG_DEBUG("carl.ran", "Roots of Poly: " << pol << ":   " << roots);

        for (LPRealAlgebraicNumber& root : roots) {
            carl::IntRepRealAlgebraicNumber<mpq_class> conv = convert<carl::IntRepRealAlgebraicNumber<mpq_class>>(root);
            carl::LPRealAlgebraicNumber converted_back = convert<carl::LPRealAlgebraicNumber>(conv);
            EXPECT_EQ(root, converted_back);
        }
    }
}

#endif
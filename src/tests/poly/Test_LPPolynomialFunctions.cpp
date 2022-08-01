#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <gtest/gtest.h>

#include "../Common.h"
#include <random>
#include <carl-arith/core/VariablePool.h>
#include <carl-arith/poly/libpoly/LPContext.h>
#include <carl-arith/poly/libpoly/LPPolynomial.h>
#include <carl-arith/poly/libpoly/Functions.h>
#include <carl-arith/ran/ran.h>
#include <carl-arith/converter/LibpolyConverter.h>
#include <carl-arith/poly/umvpoly/functions/Resultant.h>
#include <carl-arith/poly/umvpoly/functions/Factorization.h>

using namespace carl;

/*
TEST(LPPOLYNOMIAL, Resultant) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");

    std::vector varOrder = {x, y};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);

    auto lp_poly1 = polyX * polyY - 12;
    auto lp_poly2 = polyX * polyX * (polyY - 12) - polyY * polyY;

    auto carl_poly1 = to_carl_multivariate_polynomial(lp_poly1.get_polynomial());
    auto carl_poly2 = to_carl_multivariate_polynomial(lp_poly2.get_polynomial());

    auto carl_resultant = resultant(to_univariate_polynomial(carl_poly1, x), to_univariate_polynomial(carl_poly2, x));
    auto lp_resultant = resultant(lp_poly1, lp_poly2);

    EXPECT_EQ(to_univariate_polynomial(to_carl_multivariate_polynomial(lp_resultant.get_polynomial()), x), carl_resultant);
}
*/

/*
TEST(LPPOLYNOMIAL, RealRootsLP) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");

    std::vector varOrder = {x, y};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);

    auto res = polyX * polyY - 12;

    auto res_uni = polyX * polyX - 12;

    auto res_carl = to_carl_univariate_polynomial(res.get_polynomial());
    auto res_uni_carl = to_carl_univariate_polynomial(res_uni.get_polynomial());

    std::map<Variable, LPRealAlgebraicNumber> assignment;
    assignment[y] = LPRealAlgebraicNumber(123312 / 123312);

    std::map<Variable, IntRepRealAlgebraicNumber<mpq_class>> assignment_interval;
    assignment_interval[y] = IntRepRealAlgebraicNumber<mpq_class>(123312 / 123312);

    std::cout << "RealRootsLP: " << carl::real_roots(res_uni).roots() << std::endl;
    std::cout << "RealRootsLP: " << carl::real_roots(res, assignment).roots() << std::endl;

    std::cout << "RealRootsCarl: " << carl::real_roots(res_uni_carl).roots() << std::endl;
    std::cout << "RealRootsCarl: " << carl::real_roots(res_carl, assignment_interval).roots() << std::endl;
}
*/

TEST(LPPOLYNOMIAL, Evaluate) {
    auto x = fresh_real_variable("x");

    std::vector varOrder = {x};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);


    auto res = polyX * polyX - 12;

    auto res_carl = to_carl_multivariate_polynomial(res.get_polynomial());

    std::map<Variable, LPRealAlgebraicNumber> assignment;
    assignment[x] = LPRealAlgebraicNumber(123312 / 123312);

    std::map<Variable, IntRepRealAlgebraicNumber<mpq_class>> assignment_interval;
    assignment_interval[x] = IntRepRealAlgebraicNumber<mpq_class>(123312 / 123312);

    std::cout << "EvaluateLP: " << carl::evaluate(res, assignment) << std::endl;
    std::cout << "EvaluateCarl: " << carl::evaluate(res_carl, assignment_interval) << std::endl;

}

TEST(LPPOLYNOMIAL, factorization) {
    auto startTime = std::chrono::high_resolution_clock::now(); 
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

    for (const auto& pol : polys) {
        const auto& factors = carl::factorization(pol);
        LPPolynomial productOfFactors = LPPolynomial(context, x, (mpz_class)1);
        for (const auto& factor : factors) {
            EXPECT_NE((unsigned)0, factor.second);
            for (unsigned i = 0; i < factor.second; ++i) {
                productOfFactors *= factor.first;
            }
        }
        EXPECT_EQ(pol, productOfFactors);
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    std::cout << "Factorization without premade adaptor took " << duration.count() << " milliseconds." << std::endl;


    startTime = std::chrono::high_resolution_clock::now();

    CoCoAAdaptorLP adaptor(context);
    for (const auto& pol : polys) {
        const auto& factors = carl::factorization(pol, adaptor);
        LPPolynomial productOfFactors = LPPolynomial(context, x, (mpz_class)1);
        for (const auto& factor : factors) {
            EXPECT_NE((unsigned)0, factor.second);
            for (unsigned i = 0; i < factor.second; ++i) {
                productOfFactors *= factor.first;
            }
        }
        EXPECT_EQ(pol, productOfFactors);
    }
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
    std::cout << "Factorization with premade adaptor took " << duration.count() << " milliseconds." << std::endl;
}


TEST(LPPOLYNOMIAL, TotalDegree){
     auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");

    std::vector varOrder = {x, y};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);

    auto res = polyX * polyY - 12;  

    EXPECT_EQ(res.total_degree(), 2+3);
}

TEST(LPPOLYNOMIAL, Degree){
     auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");

    std::vector varOrder = {x, y};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);

    auto res = polyX * polyY - 12;  

    EXPECT_EQ(res.degree(x), 2);
    EXPECT_EQ(res.degree(y), 3);
}


TEST(LPPOLYNOMIAL, Coeff){
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");

    std::vector varOrder = {x, y};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);

    auto res = polyX * polyY - 12;

    auto res_carl = to_carl_multivariate_polynomial(res.get_polynomial());

    EXPECT_EQ(to_carl_multivariate_polynomial(res.coeff(x,2).get_polynomial()), res_carl.coeff(x,2));
    
}

#endif
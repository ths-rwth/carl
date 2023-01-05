#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <gtest/gtest.h>
#include <random>

#include "../Common.h"
#include <carl-arith/core/VariablePool.h>
#include <carl-arith/poly/libpoly/LPContext.h>
#include <carl-arith/poly/libpoly/LPPolynomial.h>
#include <carl-arith/poly/Conversion.h>

using namespace carl;

TEST(LPPOLYNOMIAL, createContext) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    std::vector<Variable> var_order1 = {x, y, z};
    std::vector<Variable> var_order2 = {x, y};
    std::vector<Variable> var_order3 = {x, y};

    LPContext ctx1(var_order1);
    LPContext ctx2(var_order2);
    LPContext ctx2_2(var_order2);
    LPContext ctx3(var_order3);
    LPContext ctx3_2(ctx3.context());

    EXPECT_EQ(ctx1.variable_ordering(), var_order1);
    EXPECT_EQ(ctx2.variable_ordering(), var_order2);
    EXPECT_EQ(ctx3.variable_ordering(), var_order2);
    EXPECT_EQ(ctx3_2.variable_ordering(), var_order2);
    EXPECT_EQ(ctx3, ctx3_2);

    EXPECT_FALSE(ctx1 == ctx2);
    EXPECT_TRUE(ctx2 == ctx3);
    EXPECT_TRUE(ctx2 == ctx2_2);
    EXPECT_TRUE(ctx3 == ctx3_2);
}

TEST(LPPOLYNOMIAL, createPoly) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    std::vector<Variable> var_order1 = {x, y, z};
    LPContext ctx(var_order1);

    std::initializer_list<mpz_class> list = {1, 0, 0, -3, 1, 0, 3, 0};
    std::map<unsigned int, mpz_class> map = {
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
    std::cout << LPPolynomial(ctx, long(17)) << std::endl;
    std::cout << LPPolynomial(ctx, x) << std::endl;
    std::cout << LPPolynomial(ctx, x, 7, 2) << std::endl;
    std::cout << LPPolynomial(ctx, x, list) << std::endl;
    std::cout << LPPolynomial(ctx, x, std::vector<mpz_class>(list)) << std::endl;
    std::cout << LPPolynomial(ctx, x, map) << std::endl;

    EXPECT_EQ(LPPolynomial(ctx, x, list), LPPolynomial(ctx, x, std::vector<mpz_class>(list)));
    EXPECT_EQ(LPPolynomial(ctx, long(17)), mpz_class(17));
    EXPECT_EQ(LPPolynomial(ctx, x, list), LPPolynomial(ctx, x, map));
}

/*
TEST(LPPOLYNOMIAL, Operators) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    std::vector varOrder = {x, y, z};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);
    LPPolynomial polyZ(context, z, 3, 4);
    LPPolynomial constant(context, long(12));

    auto res = polyX + polyY + polyZ + constant;

    std::cout << "polyX: " << polyX << " Main var: " << polyX.main_var() << std::endl;
    std::cout << "polyY: " << polyY << " Main var: " << polyY.main_var() << std::endl;
    std::cout << "polyZ: " << polyZ << " Main var: " << polyZ.main_var() << std::endl;
    std::cout << "constant: " << constant << std::endl;

    std::cout << res << std::endl;

    std::cout << context << std::endl;

    EXPECT_EQ(res.main_var(), x);

    EXPECT_EQ(mpz_class(17) < polyX, true);
}
*/

TEST(LPPOLYNOMIAL, LeadingCoefficient) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    std::vector varOrder = {x, y, z};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 1, 1);
    LPPolynomial polyY(context, y, 1, 1);
    LPPolynomial polyZ(context, z, 1, 1);
    LPPolynomial constant(context, long(12));
    LPPolynomial test(context, x,1,1)  ; 

    auto res = polyX * polyX + constant * polyX * polyY ;

    auto carl_res = to_carl_multivariate_polynomial(res.get_polynomial()) ;

    std::cout << "Carl_MainVar: " << carl_res << std::endl;
    std::cout << "Carl_MainVar: " << res.main_var() << std::endl;

    std::cout << "Carl_Polynomial: " << carl_res << std::endl ;
    std::cout << "LP_Polynomial: " << res << std::endl;

    // std::cout << "Poly lterm: " << res.lterm() << std::endl;
    std::cout << "Poly lcoeff: " << res.lcoeff() << std::endl;

    std::cout << "Carl lcoeff: "  << carl_res.lcoeff() << std::endl;
    std::cout << "Carl: lterm: "   << carl_res.lterm() << std::endl;

    //exit(1) ; 

    //carl::MultivariatePolynomials and LibPoly::Polynomial have different monomial orders and thus also different leading coefficients/terms
}

TEST(LPPOLYNOMIAL, ConstantPart) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    std::vector varOrder = {x, y, z};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);
    LPPolynomial polyZ(context, z, 3, 4);
    long con = 12 ; 
    LPPolynomial constant(context, con);

    auto res_constant = polyX * polyY * polyZ + constant + polyX * polyZ*12345;
    auto res_no_constant = polyX * polyY * polyZ * constant + polyX * polyZ*12345;

    EXPECT_EQ(res_constant.constant_part(), constant.constant_part());
    EXPECT_EQ(res_constant.constant_part(), con);
    EXPECT_EQ(res_no_constant.constant_part(), 0);
}

/*
TEST(LPPOLYNOMIAL, MainVar) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    std::vector varOrder = {x, y, z};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);
    LPPolynomial polyZ(context, z, 3, 4);
    long con = 12 ; 
    LPPolynomial constant(context, con);

    auto res_constant = polyX * polyY * polyZ + constant + polyX * polyZ*12345;


    EXPECT_EQ(res_constant.main_var(), x);
    EXPECT_EQ(res_constant.main_var(), polyX.main_var());
    EXPECT_EQ(polyX.main_var(), x);
    EXPECT_EQ(polyY.main_var(), y);
    EXPECT_EQ(polyZ.main_var(), z);
}
*/

TEST(LPPOLYNOMIAL, hasVariable) {
     auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    auto a  = fresh_real_variable("a");
    std::vector varOrder = {x, y, z};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);
    LPPolynomial polyZ(context, z, 3, 4);
    long con = 12 ; 
    LPPolynomial constant(context, con);

    auto res = polyX * polyY * polyZ + constant + polyX * polyZ*12345;


    EXPECT_TRUE(res.has(x));
    EXPECT_TRUE(res.has(y));
    EXPECT_TRUE(res.has(z));
    EXPECT_FALSE(res.has(a));
    EXPECT_FALSE(constant.has(x));
}

TEST(LPPOLYNOMIAL, CoPrimeFactor) {
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    // auto a  = fresh_real_variable("a");
    std::vector varOrder = {x, y, z};
    LPContext context(varOrder);

    LPPolynomial polyX(context, x);
    LPPolynomial polyY(context, y);
    LPPolynomial polyZ(context, z);
    long con = 12 ; 
    LPPolynomial constant(context, con);

    auto res1 = 5*polyX + 10* polyY + 15*polyZ + 20*constant;
    auto res2 = res1 + 17*polyX ;


    EXPECT_EQ(polyX.coprime_factor(), mpz_class(1)) ;
    EXPECT_EQ(constant.coprime_factor(), mpz_class(con)) ;
    EXPECT_EQ(res1.coprime_factor(), mpz_class(5)) ; 
    EXPECT_EQ(res2.coprime_factor(), mpz_class(1)) ; 


    std::cout << res1.coprime_coefficients() << std::endl ; 
    std::cout << res2.coprime_coefficients() << std::endl ;
}

TEST(LPPOLYNOMIAL, ConvertToMultivariate){
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");

    LPContext ctx({x, y});

    LPPolynomial lp_polyX(ctx, x);
    LPPolynomial lp_polyY(ctx, y);

    carl::MultivariatePolynomial<mpq_class> carl_polyX(x);
    carl::MultivariatePolynomial<mpq_class> carl_polyY(y);

    LPPolynomial lp_poly = lp_polyX * lp_polyY * lp_polyY + lp_polyX;
    carl::MultivariatePolynomial<mpq_class> carl_poly = carl_polyX * carl_polyY * carl_polyY + carl_polyX;

    carl::MultivariatePolynomial<mpq_class> converted_lp = convert<carl::MultivariatePolynomial<mpq_class>>(lp_poly);
    LPPolynomial converted_carl = convert<LPPolynomial>(ctx, carl_poly);

    EXPECT_EQ(converted_lp, carl_poly);
    EXPECT_EQ(converted_carl, lp_poly);
}

/*
TEST(LPPOLYNOMIAL, Normalized){
    auto x = fresh_real_variable("x");
    auto y = fresh_real_variable("y");
    auto z = fresh_real_variable("z");
    std::vector<Variable> var_order = {x,y,z};
    LPContext context(var_order);

    LPPolynomial linA(context, x, {(mpz_class)-2, (mpz_class)5});
    LPPolynomial linB(context, y, {(mpz_class)1, (mpz_class)3});
    LPPolynomial linC(context, z, {(mpz_class)2014, (mpz_class)68});
    LPPolynomial linD(context, x, {(mpz_class)-13, (mpz_class)4});
    LPPolynomial linE(context, x, {(mpz_class)-20, (mpz_class)5});
    LPPolynomial quaA(context, x, {(mpz_class)-2, (mpz_class)0, (mpz_class)1});
    LPPolynomial quaB(context, y, {(mpz_class)3, (mpz_class)6, (mpz_class)9});

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
        carl::MultivariatePolynomial<mpz_class> carl_pol = convert<carl::MultivariatePolynomial<mpz_class>>(pol);
        carl::UnivariatePolynomial<carl::MultivariatePolynomial<mpz_class>> carl_pol_uni = carl::to_univariate_polynomial(carl_pol,x);

        EXPECT_EQ(pol.is_normal(), carl_pol_uni.is_normal());
        EXPECT_EQ(pol.unit_part(), carl_pol_uni.unit_part().constant_part());
        LPPolynomial lp_normalized = pol.normalized() ; 
        auto carl_normalized = carl_pol_uni.normalized() ;

        EXPECT_EQ(carl::MultivariatePolynomial(carl_normalized), convert<carl::MultivariatePolynomial<mpz_class>>(lp_normalized));
    }
}
*/




#endif
#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <gtest/gtest.h>

#include "../Common.h"
#include <carl-arith/core/VariablePool.h>
#include <carl-arith/poly/lp/LPContext.h>
#include <carl-arith/poly/lp/LPPolynomial.h>

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

    EXPECT_EQ(ctx1.variable_order(), var_order1);
    EXPECT_EQ(ctx2.variable_order(), var_order2);
    EXPECT_EQ(ctx3.variable_order(), var_order2);
    EXPECT_EQ(ctx3_2.variable_order(), var_order2);
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


#endif
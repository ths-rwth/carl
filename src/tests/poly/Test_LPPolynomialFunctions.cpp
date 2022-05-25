#include <gtest/gtest.h>

#include "../Common.h"
#include <carl/core/VariablePool.h>


#include <carl/poly/lp/LPContext.h>
#include <carl/poly/lp/LPPolynomial.h>

#include <carl/poly/lp/LPFunctions.h>

#include <carl/ran/real_roots.h>

#include <carl/converter/LibpolyConverter.h>

#include <carl/poly/umvpoly/functions/Resultant.h>

using namespace carl;


TEST(LPPOLYNOMIAL, Resultant){
    auto x = fresh_real_variable("x") ; 
    auto y = fresh_real_variable("y") ;

    std::vector varOrder = {x, y};
    LPContext context(varOrder);


    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);

    auto lp_poly1 = polyX * polyY - 12;
    auto lp_poly2 = polyX * polyX * (polyY -12) - polyY * polyY; 

    auto carl_poly1 = LibpolyConverter::getInstance().toCarlMultivariatePolynomial(lp_poly1.getPolynomial()) ; 
    auto carl_poly2 = LibpolyConverter::getInstance().toCarlMultivariatePolynomial(lp_poly2.getPolynomial()) ;

    auto carl_resultant = resultant(to_univariate_polynomial(carl_poly1, x), to_univariate_polynomial(carl_poly2, x)) ; 
    auto lp_resultant = resultant(lp_poly1, lp_poly2) ; 

    EXPECT_EQ(to_univariate_polynomial(LibpolyConverter::getInstance().toCarlMultivariatePolynomial(lp_resultant.getPolynomial()), x), carl_resultant) ;
}



TEST(LPPOLYNOMIAL, RealRootsLP){
    auto x = fresh_real_variable("x") ; 
    auto y = fresh_real_variable("y") ;

    std::vector varOrder = {x, y};
    LPContext context(varOrder);


    LPPolynomial polyX(context, x, 7, 2);
    LPPolynomial polyY(context, y, 5, 3);

    auto res = polyX * polyY - 12;

    auto res_uni = polyX * polyX - 12;

    std::map<Variable, RealAlgebraicNumberLibpoly<mpq_class>> assignment ; 
	assignment[y] = RealAlgebraicNumberLibpoly<mpq_class>(123312/123312) ;



    std::cout << "RealRootsLP: " << carl::real_roots_libpoly<mpq_class>(res_uni).roots() << std::endl ;
    std::cout << "RealRootsLP: " << carl::real_roots_libpoly<mpq_class>(res, assignment).roots() << std::endl ;
}



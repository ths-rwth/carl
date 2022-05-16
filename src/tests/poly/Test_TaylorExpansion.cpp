#include "gtest/gtest.h"
#include <carl/poly/umvpoly/functions/TaylorExpansion.h>

#include "../Common.h"

using namespace carl;

#ifdef USE_CLN_NUMBERS

TEST(TaylorExpansion, ideal_adic_coefficient) {

    // the Field Z_5
    GaloisFieldManager<cln::cl_I>& gfm = GaloisFieldManager<cln::cl_I>::getInstance();
    const GaloisField<cln::cl_I>* gf5 = gfm.field(5,1);

    // the five numbers from Z_5
    GFNumber<cln::cl_I> a0(0, gf5);
    GFNumber<cln::cl_I> a1(1, gf5);
    GFNumber<cln::cl_I> a2(2, gf5);
    GFNumber<cln::cl_I> a3(3, gf5);
    GFNumber<cln::cl_I> a4(4, gf5);


    Variable x_1 = carl::fresh_integer_variable("x_1");
    Variable x_2 = carl::fresh_integer_variable("x_2");

    MultivariatePolynomial<GFNumber<cln::cl_I>> p({a1 * x_1, a2 * x_2 * x_2, a1 * x_1 * x_2});

    //std::cout << TaylorExpansion<cln::cl_I>::ideal_adic_coeff(p, x_2, a1, 2) << std::endl;

}

#endif

#include "gtest/gtest.h"
#include "carl/core/TaylorExpansion.h"
#include <cln/cln.h>

using namespace carl;

TEST(TaylorExpansion, ideal_adic_coefficient) {
    
    // the Field Z_5
    GaloisFieldManager<cln::cl_I>& gfm = GaloisFieldManager<mpz_class>::getInstance();
    const GaloisField<cln::cl_I>* gf5 = gfm.getField(5,1);
    
}

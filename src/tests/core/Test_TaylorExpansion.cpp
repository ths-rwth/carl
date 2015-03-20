#include "gtest/gtest.h"
#include "carl/core/TaylorExpansion.h"
#include <cln/cln.h>

using namespace carl;

TEST(TaylorExpansion, ideal_adic_coefficient) {
    const GaloisField<cln::cl_I>* mGf_pk;
    mGf_pk(GaloisFieldManager<cln::cl_I>::getInstance().getField(p,k));
}

#include <gmpxx.h>
#include "gtest/gtest.h"
#include "carl/numbers/GF.h"


using namespace carl;

TEST(GaloisField, integers)
{
    mpz_class p(3);
    
    GaloisField<mpz_class> gf3(p,1);
    GaloisFieldManager<mpz_class>& gfm = GaloisFieldManager<mpz_class>::getInstance();
    
    const GaloisField<mpz_class>* gf5 = gfm.getField(5,1);
    
}




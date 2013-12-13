#include <gmpxx.h>
#include "gtest/gtest.h"

#include "carl/numbers/typetraits.h"
#include "carl/numbers/operations.h"
#include "carl/numbers/operations_gmp.h"

#include <cln/cln.h>

using namespace carl;

TEST(Numbers, constructors)
{
    // GMPXX 
    mpq_class a(mpz_class(2), mpz_class(3));
    EXPECT_EQ(a.get_den(), 3);
    EXPECT_EQ(a.get_num(), 2);
    
    // CLN
    cln::cl_RA b = cln::cl_I(2)/cln::cl_I(3);
    EXPECT_EQ(cln::denominator(b), 3);
    EXPECT_EQ(cln::numerator(b), 2);
}

TEST(Numbers, operations)
{
	ASSERT_TRUE(carl::floor(cln::rationalize(1.5)) == (cln::cl_I)1);
	ASSERT_TRUE(carl::ceil(cln::rationalize(1.5)) == (cln::cl_I)2);
	
	ASSERT_TRUE(carl::gcd((cln::cl_I)15, (cln::cl_I)20) == (cln::cl_I)5);
}

TEST(Numbers, squareroot)
{
    // GMPXX
    mpq_class a(mpz_class(2), mpz_class(3));
    std::pair<mpq_class, mpq_class> resultA;
    resultA = sqrt(a);
    EXPECT_EQ(resultA.first.get_den(), 2);
    EXPECT_EQ(resultA.first.get_num(), 1);
    EXPECT_EQ(resultA.second.get_den(), 1);
    EXPECT_EQ(resultA.second.get_num(), 2);
    
    // CLN
    cln::cl_RA b = cln::cl_I(2)/cln::cl_I(3);
    std::pair<cln::cl_RA, cln::cl_RA> resultB;
    resultB = carl::sqrt(b);
    resultB.first.debug_print();
    resultB.second.debug_print();
}
#include "gtest/gtest.h"

#include "carl/numbers/typetraits.h"
#include "carl/numbers/operations.h"
#include "carl/numbers/Numeric.h"

using namespace carl;

TEST(Numeric, constructors)
{
    Numeric n1();
//    Numeric<true> n2( 2 );
    Numeric n2( 4 );
    Numeric n3( cln::cl_RA( 5 ) );
    Numeric n4( n2 );
    Numeric n5( n3 );
}

TEST(Numeric, operations)
{
//    Numeric n = Numeric(1) + Numeric(5);
//    EXPECT_EQ( cln::cl_RA(1)+cln::cl_RA(5), n.toRational());
//    
//    n = Numeric(12) - Numeric(47);
//    EXPECT_EQ( cln::cl_RA(12)-cln::cl_RA(47), n.toRational());
//    
//    n = Numeric(-761) * Numeric(3);
//    EXPECT_EQ( cln::cl_RA(-761)+cln::cl_RA(3), n.toRational());
//    
//    n = Numeric(120) / Numeric(24);
//    EXPECT_EQ( cln::cl_RA(120)/cln::cl_RA(24), n.toRational());
}

TEST(Numeric, gcd)
{
}

TEST(Numeric, lcm)
{
}

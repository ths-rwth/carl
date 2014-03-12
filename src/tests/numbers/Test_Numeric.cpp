#include "gtest/gtest.h"

#include "carl/numbers/typetraits.h"
#include "carl/numbers/operations.h"
#include "carl/numbers/Numeric.h"
#include <bitset>

using namespace carl;

TEST(Numeric, constructors)
{
    std::cout << "HIGHTEST_INTEGER_VALUE = " << HIGHTEST_INTEGER_VALUE << std::endl;
    std::cout << std::bitset<64>( HIGHTEST_INTEGER_VALUE ) << std::endl;
    std::cout << std::bitset<64>( (-HIGHTEST_INTEGER_VALUE) ) << std::endl;
    std::cout << std::bitset<64>( (ContentType) 1 ) << std::endl;
    std::cout << std::bitset<64>( (ContentType) -1 ) << std::endl;
    std::cout << "std::numeric_limits<ContentType>::max() = " << std::numeric_limits<ContentType>::max() << std::endl;
    std::cout << std::bitset<64>( std::numeric_limits<ContentType>::max() ) << std::endl;
    Numeric n0;
    EXPECT_EQ( cln::cl_RA(0), n0.toRational());
    Numeric n1( 2, true );
    EXPECT_EQ( cln::cl_RA(2), n1.toRational());
    Numeric n2( 4 );
    EXPECT_EQ( cln::cl_RA(4), n2.toRational());
    Numeric n3( cln::cl_RA( 5 ) );
    EXPECT_EQ( cln::cl_RA(5), n3.toRational());
    Numeric n4( n2 );
    EXPECT_EQ( cln::cl_RA(4), n4.toRational());
    Numeric n5( n3 );
    EXPECT_EQ( cln::cl_RA(5), n5.toRational());
}

TEST(Numeric, operations)
{
    Numeric n = Numeric(1) + Numeric(5);
    std::cout << n << std::endl;
    EXPECT_EQ( cln::cl_RA(1)+cln::cl_RA(5), n.toRational());
    
    n = Numeric(12) - Numeric(47);
    std::cout << n << std::endl;
    EXPECT_EQ( cln::cl_RA(12)-cln::cl_RA(47), n.toRational());
    
    n = Numeric(-761) * Numeric(3);
    std::cout << n << std::endl;
    EXPECT_EQ( cln::cl_RA(-761)*cln::cl_RA(3), n.toRational());
    
    n = Numeric(120) / Numeric(24);
    std::cout << n << std::endl;
    EXPECT_EQ( cln::cl_RA(120)/cln::cl_RA(24), n.toRational());
    
    n += Numeric( 102345 );
    EXPECT_EQ( cln::cl_RA( 102350 ), n.toRational());
    
    n -= Numeric( 1023452 );
    EXPECT_EQ( cln::cl_RA( -921102 ), n.toRational());
}

TEST(Numeric, gcd)
{
}

TEST(Numeric, lcm)
{
}

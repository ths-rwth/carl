#include "gtest/gtest.h"

#include "carl/numbers/typetraits.h"
#include "carl/numbers/operations.h"
#include "carl/numbers/Numeric.h"
#include <bitset>

using namespace carl;

TEST(Numeric, constructors)
{
    size_t num_of_allocated_elements = Numeric::mRationalPool.size();
    Numeric n0;
    EXPECT_EQ( cln::cl_RA(0), n0.toRational());
    Numeric n1( 2, true );
    EXPECT_EQ( cln::cl_RA(2), n1.toRational());
    Numeric n2( 4 );
    EXPECT_EQ( cln::cl_RA(4), n2.toRational());
    Numeric n3( cln::cl_RA( 5 ) );
    EXPECT_EQ( num_of_allocated_elements, Numeric::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA(5), n3.toRational());
    Numeric n4( n2 );
    EXPECT_EQ( cln::cl_RA(4), n4.toRational());
    Numeric n5( n3 );
    EXPECT_EQ( num_of_allocated_elements, Numeric::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( (ContentType) 5 ), n5.toRational());
    Numeric n6( HIGHTEST_INTEGER_VALUE );
    EXPECT_EQ( num_of_allocated_elements, Numeric::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( HIGHTEST_INTEGER_VALUE ), n6.toRational());
    Numeric n7( HIGHTEST_INTEGER_VALUE + 1 );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( HIGHTEST_INTEGER_VALUE + 1 ), n7.toRational());
    Numeric n8( -HIGHTEST_INTEGER_VALUE );
    EXPECT_EQ( cln::cl_RA( -HIGHTEST_INTEGER_VALUE ), n8.toRational());
    Numeric n9( -HIGHTEST_INTEGER_VALUE - 1 );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( -HIGHTEST_INTEGER_VALUE - 1 ), n9.toRational());
    Numeric n10( (cln::cl_RA( 1 )/cln::cl_RA( 3 )) );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric::mRationalPool.size() );
    EXPECT_EQ( (cln::cl_RA( 1 )/cln::cl_RA( 3 )), n10.toRational());
}

TEST(Numeric, operations)
{
    Numeric n = Numeric(1) + Numeric(5);
    EXPECT_EQ( cln::cl_RA(1)+cln::cl_RA(5), n.toRational());
    
    n = Numeric(12) - Numeric(47);
    EXPECT_EQ( cln::cl_RA(12)-cln::cl_RA(47), n.toRational());
    
    n = Numeric(-761) * Numeric(3);
    EXPECT_EQ( cln::cl_RA(-761)*cln::cl_RA(3), n.toRational());
    
    n = Numeric(120) / Numeric(24);
    EXPECT_EQ( cln::cl_RA(120)/cln::cl_RA(24), n.toRational());
    
    n += Numeric( 102345 );
    EXPECT_EQ( cln::cl_RA( 102350 ), n.toRational());
    
    n -= Numeric( 1023452 );
    EXPECT_EQ( cln::cl_RA( -921102 ), n.toRational());
    
    n -= Numeric( HIGHTEST_INTEGER_VALUE );
    EXPECT_EQ( cln::cl_RA( ( -921102 - HIGHTEST_INTEGER_VALUE ) ), n.toRational());
    
    Numeric n2;
    n2 -= Numeric( HIGHTEST_INTEGER_VALUE + 1 );
    EXPECT_EQ( cln::cl_RA( ( -1 - HIGHTEST_INTEGER_VALUE ) ), n2.toRational());
    
    n -= n2;
    EXPECT_EQ( cln::cl_RA( ( -921102 - HIGHTEST_INTEGER_VALUE - (- 1 - HIGHTEST_INTEGER_VALUE) ) ), n.toRational());
    
    Numeric n3( HIGHTEST_INTEGER_VALUE );
    n3 *= Numeric( 2 );
    EXPECT_EQ( cln::cl_RA( HIGHTEST_INTEGER_VALUE ) * cln::cl_RA( 2 ), n3.toRational());
    
    Numeric n4( (cln::cl_RA(1)/cln::cl_RA(3)) );
    n4 *= Numeric( 9 );
    EXPECT_EQ( cln::cl_RA( 3 ), n4.toRational() );
    EXPECT_TRUE( IS_INT( n4.content() ) );
    
    Numeric n5( 2 );
    n5 *= Numeric( (cln::cl_RA(1)/cln::cl_RA(3)) );
    EXPECT_EQ( (cln::cl_RA( 2 )/cln::cl_RA( 3 )), n5.toRational() );
    EXPECT_FALSE( IS_INT( n5.content() ) );
    
    Numeric n6( HIGHTEST_INTEGER_VALUE + 2 );
    n6 /= Numeric( 3 );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE + 2 )/cln::cl_RA( 3 )), n6.toRational() );
    
    Numeric n7( 44 );
    n7 /= Numeric( 11 );
    EXPECT_EQ( cln::cl_RA( 4 ), n7.toRational() );
    EXPECT_TRUE( IS_INT( n7.content() ) );
    
    Numeric n8( HIGHTEST_INTEGER_VALUE - 2 );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE - 2 ) ), carl::abs( n8 ).toRational() ); 
    ++n8;
    EXPECT_TRUE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_TRUE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_FALSE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_FALSE( IS_INT( n8.content() ) );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE + 2 ) ), carl::abs( n8 ).toRational() ); 
    
    Numeric n9( -HIGHTEST_INTEGER_VALUE + 2 );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE - 2 ) ), carl::abs( n9 ).toRational() ); 
    --n9;
    EXPECT_TRUE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_TRUE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_FALSE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_FALSE( IS_INT( n9.content() ) );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE + 2 ) ), carl::abs( n9 ).toRational() ); 
    
    Numeric c1;
    Numeric c2( 12 );
    Numeric c3( -HIGHTEST_INTEGER_VALUE );
    Numeric c4( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE );
    Numeric c5( (cln::cl_RA(1)/cln::cl_RA(3)) );
    Numeric c6( (cln::cl_RA(-90)/cln::cl_RA(5)) );
    EXPECT_TRUE( IS_INT( c6.content() ) );
    Numeric c7( c4 );
    EXPECT_FALSE( c1 < c1 );
    EXPECT_TRUE( c1 == c1 );
    EXPECT_TRUE( c1 >= c1 );
    EXPECT_TRUE( c1 < c2 );
    EXPECT_TRUE( c2 >= c3 );
    EXPECT_FALSE( c3 != c3 );
    EXPECT_TRUE( c4 == c4 );
    EXPECT_FALSE( c4 == c5 );
    EXPECT_FALSE( c4 <= c5 );
    EXPECT_TRUE( c4 > c5 );
    EXPECT_FALSE( c4 == c6 );
    EXPECT_TRUE( c4 > c6 );
    EXPECT_TRUE( c7 == c4 );
    EXPECT_FALSE( c7 != c4 );
    EXPECT_FALSE( c7 > c4 );
    
    Numeric c8( 144 );
    Numeric c9( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE );
    Numeric c10( 3 );
    EXPECT_EQ( carl::div( c2, c2 ), (c2 / c2) );
    EXPECT_EQ( carl::div( c8, c2 ), (c8 / c2) );
    EXPECT_EQ( carl::div( c9, c10 ), (c9 / c10) );
    EXPECT_EQ( carl::div( c9, c3 ), (c9 / c3) );
    Numeric d1( c2 );
    d1.divideBy( c2 );
    EXPECT_EQ( carl::div( c2, c2 ), d1 );
    Numeric d2( c8 );
    d2.divideBy( c2 );
    EXPECT_EQ( carl::div( c8, c2 ), d2 );
    Numeric d3( c9 );
    d3.divideBy( c10 );
    EXPECT_EQ( carl::div( c9, c10 ), d3 );
    Numeric d4( c9 );
    d4.divideBy( c3 );
    EXPECT_EQ( carl::div( c9, c3 ), d4 );
}

TEST(Numeric, gcd)
{
    Numeric a( 2*2*3*11*17 );
    Numeric b( 2*5*5*11*23 );
    Numeric c( 2*11 );
    Numeric d( 2*2*3*5*5*11*17*23 );
    Numeric e( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE );
    Numeric f( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE );
    EXPECT_EQ( c, carl::gcd( a, b ) );
    EXPECT_EQ( c, carl::gcd( b, a ) );
    EXPECT_EQ( c, carl::gcd( -a, b ) );
    EXPECT_EQ( c, carl::gcd( -b, a ) );
    EXPECT_EQ( c, carl::gcd( a, -b ) );
    EXPECT_EQ( c, carl::gcd( b, -a ) );
    EXPECT_EQ( d, carl::lcm( a, b ) );
    EXPECT_EQ( d, carl::lcm( b, a ) );
    EXPECT_EQ( d, carl::lcm( -a, b ) );
    EXPECT_EQ( d, carl::lcm( -b, a ) );
    EXPECT_EQ( d, carl::lcm( a, -b ) );
    EXPECT_EQ( d, carl::lcm( b, -a ) );
    EXPECT_EQ( carl::gcd( e.toRational(), d.toRational() ), carl::gcd( e, d ).toRational() );
    EXPECT_EQ( carl::gcd( e.toRational(), d.toRational() ), carl::gcd( d, e ).toRational() );
    EXPECT_EQ( carl::gcd( e.toRational(), d.toRational() ), carl::gcd( -e, d ).toRational() );
    EXPECT_EQ( carl::gcd( e.toRational(), d.toRational() ), carl::gcd( -d, e ).toRational() );
    EXPECT_EQ( carl::gcd( e.toRational(), d.toRational() ), carl::gcd( e, -d ).toRational() );
    EXPECT_EQ( carl::gcd( e.toRational(), d.toRational() ), carl::gcd( d, -e ).toRational() );
    EXPECT_EQ( carl::lcm( e.toRational(), d.toRational() ), carl::lcm( e, d ).toRational() );
    EXPECT_EQ( carl::lcm( e.toRational(), d.toRational() ), carl::lcm( d, e ).toRational() );
    EXPECT_EQ( carl::lcm( e.toRational(), d.toRational() ), carl::lcm( -e, d ).toRational() );
    EXPECT_EQ( carl::lcm( e.toRational(), d.toRational() ), carl::lcm( -d, e ).toRational() );
    EXPECT_EQ( carl::lcm( e.toRational(), d.toRational() ), carl::lcm( e, -d ).toRational() );
    EXPECT_EQ( carl::lcm( e.toRational(), d.toRational() ), carl::lcm( d, -e ).toRational() );
    
    Numeric g( 997002998000 );
    Numeric h( 996005994003 );
    Numeric j = g * h;
    Numeric i( -997002998000 );
    EXPECT_EQ( carl::gcd( j.toRational(), i.toRational() ), carl::gcd( j, i ).toRational() );
}

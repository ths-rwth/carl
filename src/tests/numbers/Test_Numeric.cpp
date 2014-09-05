#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"
#include "carl/numbers/Numeric.h"
#include <bitset>

using namespace carl;

TEST(Numeric, constructors)
{
    size_t num_of_allocated_elements = Numeric<cln::cl_RA>::mRationalPool.size();
    Numeric<cln::cl_RA> n0;
    EXPECT_EQ( cln::cl_RA(0), n0.toRational());
    Numeric<cln::cl_RA> n1( 2, true );
    EXPECT_EQ( cln::cl_RA(2), n1.toRational());
    Numeric<cln::cl_RA> n2( 4 );
    EXPECT_EQ( cln::cl_RA(4), n2.toRational());
    Numeric<cln::cl_RA> n3( cln::cl_RA( 5 ) );
    EXPECT_EQ( num_of_allocated_elements, Numeric<cln::cl_RA>::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA(5), n3.toRational());
    Numeric<cln::cl_RA> n4( n2 );
    EXPECT_EQ( cln::cl_RA(4), n4.toRational());
    Numeric<cln::cl_RA> n5( n3 );
    EXPECT_EQ( num_of_allocated_elements, Numeric<cln::cl_RA>::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( (ContentType) 5 ), n5.toRational());
    Numeric<cln::cl_RA> n6( (HIGHTEST_INTEGER_VALUE - 1) );
    EXPECT_EQ( num_of_allocated_elements, Numeric<cln::cl_RA>::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( (HIGHTEST_INTEGER_VALUE - 1) ), n6.toRational());
    Numeric<cln::cl_RA> n7( HIGHTEST_INTEGER_VALUE );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric<cln::cl_RA>::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( HIGHTEST_INTEGER_VALUE ), n7.toRational());
    Numeric<cln::cl_RA> n8( -(HIGHTEST_INTEGER_VALUE - 1) );
    EXPECT_EQ( cln::cl_RA( -(HIGHTEST_INTEGER_VALUE - 1) ), n8.toRational());
    Numeric<cln::cl_RA> n9( -HIGHTEST_INTEGER_VALUE );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric<cln::cl_RA>::mRationalPool.size() );
    EXPECT_EQ( cln::cl_RA( -HIGHTEST_INTEGER_VALUE ), n9.toRational());
    Numeric<cln::cl_RA> n10( (cln::cl_RA( 1 )/cln::cl_RA( 3 )) );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric<cln::cl_RA>::mRationalPool.size() );
    EXPECT_EQ( (cln::cl_RA( 1 )/cln::cl_RA( 3 )), n10.toRational());
}

TEST(Numeric, operations)
{
    Numeric<cln::cl_RA> n = Numeric<cln::cl_RA>(1) + Numeric<cln::cl_RA>(5);
    EXPECT_EQ( cln::cl_RA(1)+cln::cl_RA(5), n.toRational());
    
    n = Numeric<cln::cl_RA>(12) - Numeric<cln::cl_RA>(47);
    EXPECT_EQ( cln::cl_RA(12)-cln::cl_RA(47), n.toRational());
    
    n = Numeric<cln::cl_RA>(-761) * Numeric<cln::cl_RA>(3);
    EXPECT_EQ( cln::cl_RA(-761)*cln::cl_RA(3), n.toRational());
    
    n = Numeric<cln::cl_RA>(120) / Numeric<cln::cl_RA>(24);
    EXPECT_EQ( cln::cl_RA(120)/cln::cl_RA(24), n.toRational());
    
    n += Numeric<cln::cl_RA>( 102345 );
    EXPECT_EQ( cln::cl_RA( 102350 ), n.toRational());
    
    n -= Numeric<cln::cl_RA>( 1023452 );
    EXPECT_EQ( cln::cl_RA( -921102 ), n.toRational());
    
    n -= Numeric<cln::cl_RA>( (HIGHTEST_INTEGER_VALUE - 1) );
    EXPECT_EQ( cln::cl_RA( ( -921102 - (HIGHTEST_INTEGER_VALUE - 1) ) ), n.toRational());
    
    Numeric<cln::cl_RA> n2;
    n2 -= Numeric<cln::cl_RA>( HIGHTEST_INTEGER_VALUE );
    EXPECT_EQ( cln::cl_RA( ( -HIGHTEST_INTEGER_VALUE ) ), n2.toRational());
    
    n -= n2;
    EXPECT_EQ( cln::cl_RA( ( -921102 - (HIGHTEST_INTEGER_VALUE - 1) - (-HIGHTEST_INTEGER_VALUE) ) ), n.toRational());
    
    Numeric<cln::cl_RA> n3( (HIGHTEST_INTEGER_VALUE - 1) );
    n3 *= Numeric<cln::cl_RA>( 2 );
    EXPECT_EQ( cln::cl_RA( (HIGHTEST_INTEGER_VALUE - 1) ) * cln::cl_RA( 2 ), n3.toRational());
    
    Numeric<cln::cl_RA> n4( (cln::cl_RA(1)/cln::cl_RA(3)) );
    n4 *= Numeric<cln::cl_RA>( 9 );
    EXPECT_EQ( cln::cl_RA( 3 ), n4.toRational() );
    EXPECT_TRUE( IS_INT( n4.content() ) );
    
    Numeric<cln::cl_RA> n5( 2 );
    n5 *= Numeric<cln::cl_RA>( (cln::cl_RA(1)/cln::cl_RA(3)) );
    EXPECT_EQ( (cln::cl_RA( 2 )/cln::cl_RA( 3 )), n5.toRational() );
    EXPECT_FALSE( IS_INT( n5.content() ) );
    
    Numeric<cln::cl_RA> n6( HIGHTEST_INTEGER_VALUE + 1 );
    n6 /= Numeric<cln::cl_RA>( 3 );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE + 1 )/cln::cl_RA( 3 )), n6.toRational() );
    
    Numeric<cln::cl_RA> n7( 44 );
    n7 /= Numeric<cln::cl_RA>( 11 );
    EXPECT_EQ( cln::cl_RA( 4 ), n7.toRational() );
    EXPECT_TRUE( IS_INT( n7.content() ) );
    
    Numeric<cln::cl_RA> n8( HIGHTEST_INTEGER_VALUE - 3 );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE - 3 ) ), carl::abs( n8 ).toRational() ); 
    ++n8;
    EXPECT_TRUE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_TRUE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_FALSE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_FALSE( IS_INT( n8.content() ) );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE + 1 ) ), carl::abs( n8 ).toRational() ); 
    
    Numeric<cln::cl_RA> n9( -HIGHTEST_INTEGER_VALUE + 3 );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE - 3 ) ), carl::abs( n9 ).toRational() ); 
    --n9;
    EXPECT_TRUE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_TRUE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_FALSE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_FALSE( IS_INT( n9.content() ) );
    EXPECT_EQ( (cln::cl_RA( HIGHTEST_INTEGER_VALUE + 1 ) ), carl::abs( n9 ).toRational() ); 
    
    Numeric<cln::cl_RA> c1;
    Numeric<cln::cl_RA> c2( 12 );
    Numeric<cln::cl_RA> c3( -(HIGHTEST_INTEGER_VALUE - 1) );
    Numeric<cln::cl_RA> c4( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-2 );
    Numeric<cln::cl_RA> c5( (cln::cl_RA(1)/cln::cl_RA(3)) );
    Numeric<cln::cl_RA> c6( (cln::cl_RA(-90)/cln::cl_RA(5)) );
    EXPECT_TRUE( IS_INT( c6.content() ) );
    Numeric<cln::cl_RA> c7( c4 );
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
    
    Numeric<cln::cl_RA> c8( 144 );
    Numeric<cln::cl_RA> c9( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-3 );
    Numeric<cln::cl_RA> c10( 3 );
    EXPECT_EQ( carl::div( c2, c2 ), (c2 / c2) );
    EXPECT_EQ( carl::div( c8, c2 ), (c8 / c2) );
    EXPECT_EQ( carl::div( c9, c10 ), (c9 / c10) );
    EXPECT_EQ( carl::div( c9, c3 ), (c9 / c3) );
    Numeric<cln::cl_RA> d1( c2 );
    EXPECT_EQ( carl::div( c2, c2 ), carl::div_here( d1, c2 ) );
    Numeric<cln::cl_RA> d2( c8 );
    EXPECT_EQ( carl::div( c8, c2 ), carl::div_here( d2, c2 ) );
    Numeric<cln::cl_RA> d3( c9 );
    EXPECT_EQ( carl::div( c9, c10 ), carl::div_here( d3, c10 ) );
    Numeric<cln::cl_RA> d4( c9 );
    EXPECT_EQ( carl::div( c9, c3 ), carl::div_here( d4, c3 ) );
}

TEST(Numeric, gcd)
{
    Numeric<cln::cl_RA> a( 2*2*3*11*17 );
    Numeric<cln::cl_RA> b( 2*5*5*11*23 );
    Numeric<cln::cl_RA> c( 2*11 );
    Numeric<cln::cl_RA> d( 2*2*3*5*5*11*17*23 );
    Numeric<cln::cl_RA> e( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-3 );
    Numeric<cln::cl_RA> f( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-4 );
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
    
    Numeric<cln::cl_RA> g( 997002998000 );
    Numeric<cln::cl_RA> h( 996005994003 );
    Numeric<cln::cl_RA> j = g * h;
    Numeric<cln::cl_RA> i( -997002998000 );
    EXPECT_EQ( carl::gcd( j.toRational(), i.toRational() ), carl::gcd( j, i ).toRational() );
}

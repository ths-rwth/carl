#include "gtest/gtest.h"

#include "carl/numbers/numbers.h"
#include "carl/numbers/Numeric.h"
#include "carl/util/platform.h"

#include "../Common.h"

using namespace carl;


TEST(Numeric, constructors)
{
    std::size_t num_of_allocated_elements = Numeric<Rational>::mRationalPool.size();
    Numeric<Rational> n0;
    EXPECT_EQ( Rational(0), n0.toRational());
    Numeric<Rational> n1( 2, true );
    EXPECT_EQ( Rational(2), n1.toRational());
    Numeric<Rational> n2( 4 );
    EXPECT_EQ( Rational(4), n2.toRational());
    Numeric<Rational> n3( Rational( 5 ) );
    EXPECT_EQ( num_of_allocated_elements, Numeric<Rational>::mRationalPool.size() );
    EXPECT_EQ( Rational(5), n3.toRational());
    Numeric<Rational> n4( n2 );
    EXPECT_EQ( Rational(4), n4.toRational());
    Numeric<Rational> n5( n3 );
    EXPECT_EQ( num_of_allocated_elements, Numeric<Rational>::mRationalPool.size() );
    EXPECT_EQ( Rational(5), n5.toRational());
    Numeric<Rational> n6 = carl::fromInt<Rational>(HIGHTEST_INTEGER_VALUE - 1);
    EXPECT_EQ( num_of_allocated_elements, Numeric<Rational>::mRationalPool.size() );
    EXPECT_EQ( carl::fromInt<Rational>(HIGHTEST_INTEGER_VALUE - 1), n6.toRational());
    Numeric<Rational> n7( HIGHTEST_INTEGER_VALUE );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric<Rational>::mRationalPool.size() );
    EXPECT_EQ( carl::fromInt<Rational>( HIGHTEST_INTEGER_VALUE ), n7.toRational());
    Numeric<Rational> n8( -(HIGHTEST_INTEGER_VALUE - 1) );
    EXPECT_EQ( carl::fromInt<Rational>( -(HIGHTEST_INTEGER_VALUE - 1) ), n8.toRational());
    Numeric<Rational> n9( -HIGHTEST_INTEGER_VALUE );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric<Rational>::mRationalPool.size() );
    EXPECT_EQ( carl::fromInt<Rational>( -HIGHTEST_INTEGER_VALUE ), n9.toRational());
    Numeric<Rational> n10( (Rational( 1 )/Rational( 3 )) );
    ++num_of_allocated_elements;
    EXPECT_EQ( num_of_allocated_elements, Numeric<Rational>::mRationalPool.size() );
    EXPECT_EQ( (Rational( 1 )/Rational( 3 )), n10.toRational());
}

TEST(Numeric, operations)
{
    Numeric<Rational> n = Numeric<Rational>(1) + Numeric<Rational>(5);
    EXPECT_EQ( Rational(1)+Rational(5), n.toRational());

    n = Numeric<Rational>(12) - Numeric<Rational>(47);
    EXPECT_EQ( Rational(12)-Rational(47), n.toRational());

    n = Numeric<Rational>(-761) * Numeric<Rational>(3);
    EXPECT_EQ( Rational(-761)*Rational(3), n.toRational());

    n = Numeric<Rational>(120) / Numeric<Rational>(24);
    EXPECT_EQ( Rational(120)/Rational(24), n.toRational());

    n += Numeric<Rational>( 102345 );
    EXPECT_EQ( Rational( 102350 ), n.toRational());

    n -= Numeric<Rational>( 1023452 );
    EXPECT_EQ( Rational( -921102 ), n.toRational());

    n -= Numeric<Rational>( (HIGHTEST_INTEGER_VALUE - 1) );
    EXPECT_EQ( carl::fromInt<Rational>( ( -921102 - (HIGHTEST_INTEGER_VALUE - 1) ) ), n.toRational());

    Numeric<Rational> n2;
    n2 -= Numeric<Rational>( HIGHTEST_INTEGER_VALUE );
    EXPECT_EQ( carl::fromInt<Rational>( ( -HIGHTEST_INTEGER_VALUE ) ), n2.toRational());

    n -= n2;
    EXPECT_EQ( carl::fromInt<Rational>( ( -921102 - (HIGHTEST_INTEGER_VALUE - 1) - (-HIGHTEST_INTEGER_VALUE) ) ), n.toRational());

    Numeric<Rational> n3( (HIGHTEST_INTEGER_VALUE - 1) );
    n3 *= Numeric<Rational>( 2 );
    EXPECT_EQ( Rational( (HIGHTEST_INTEGER_VALUE - 1) ) * Rational( 2 ), n3.toRational());

    Numeric<Rational> n4( (Rational(1)/Rational(3)) );
    n4 *= Numeric<Rational>( 9 );
    EXPECT_EQ( Rational( 3 ), n4.toRational() );
    EXPECT_TRUE( IS_INT( n4.content() ) );

    Numeric<Rational> n5( 2 );
    n5 *= Numeric<Rational>( (Rational(1)/Rational(3)) );
    EXPECT_EQ( (Rational( 2 )/Rational( 3 )), n5.toRational() );
    EXPECT_FALSE( IS_INT( n5.content() ) );

    Numeric<Rational> n6( HIGHTEST_INTEGER_VALUE + 1 );
    n6 /= Numeric<Rational>( 3 );
    EXPECT_EQ( (Rational( HIGHTEST_INTEGER_VALUE + 1 )/Rational( 3 )), n6.toRational() );

    Numeric<Rational> n7( 44 );
    n7 /= Numeric<Rational>( 11 );
    EXPECT_EQ( Rational( 4 ), n7.toRational() );
    EXPECT_TRUE( IS_INT( n7.content() ) );

    Numeric<Rational> n8( HIGHTEST_INTEGER_VALUE - 3 );
    EXPECT_EQ( (Rational( HIGHTEST_INTEGER_VALUE - 3 ) ), carl::abs( n8 ).toRational() );
    ++n8;
    EXPECT_TRUE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_TRUE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_FALSE( IS_INT( n8.content() ) );
    ++n8;
    EXPECT_FALSE( IS_INT( n8.content() ) );
    EXPECT_EQ( (Rational( HIGHTEST_INTEGER_VALUE + 1 ) ), carl::abs( n8 ).toRational() );

    Numeric<Rational> n9( -HIGHTEST_INTEGER_VALUE + 3 );
    EXPECT_EQ( (Rational( HIGHTEST_INTEGER_VALUE - 3 ) ), carl::abs( n9 ).toRational() );
    --n9;
    EXPECT_TRUE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_TRUE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_FALSE( IS_INT( n9.content() ) );
    --n9;
    EXPECT_FALSE( IS_INT( n9.content() ) );
    EXPECT_EQ( (Rational( HIGHTEST_INTEGER_VALUE + 1 ) ), carl::abs( n9 ).toRational() );

    Numeric<Rational> c1;
    Numeric<Rational> c2( 12 );
    Numeric<Rational> c3( -(HIGHTEST_INTEGER_VALUE - 1) );
    Numeric<Rational> c4( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-2 );
    Numeric<Rational> c5( (Rational(1)/Rational(3)) );
    Numeric<Rational> c6( (Rational(-90)/Rational(5)) );
    EXPECT_TRUE( IS_INT( c6.content() ) );
    Numeric<Rational> c7( c4 );
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

    Numeric<Rational> c8( 144 );
    Numeric<Rational> c9( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-3 );
    Numeric<Rational> c10( 3 );
    EXPECT_EQ( carl::div( c2, c2 ), (c2 / c2) );
    EXPECT_EQ( carl::div( c8, c2 ), (c8 / c2) );
    EXPECT_EQ( carl::div( c9, c10 ), (c9 / c10) );
    EXPECT_EQ( carl::div( c9, c3 ), (c9 / c3) );
    Numeric<Rational> d1( c2 );
    EXPECT_EQ( carl::div( c2, c2 ), carl::div_assign( d1, c2 ) );
    Numeric<Rational> d2( c8 );
    EXPECT_EQ( carl::div( c8, c2 ), carl::div_assign( d2, c2 ) );
    Numeric<Rational> d3( c9 );
    EXPECT_EQ( carl::div( c9, c10 ), carl::div_assign( d3, c10 ) );
    Numeric<Rational> d4( c9 );
    EXPECT_EQ( carl::div( c9, c3 ), carl::div_assign( d4, c3 ) );
}

TEST(Numeric, gcd)
{
    Numeric<Rational> a( 2*2*3*11*17 );
    Numeric<Rational> b( 2*5*5*11*23 );
    Numeric<Rational> c( 2*11 );
    Numeric<Rational> d( 2*2*3*5*5*11*17*23 );
    Numeric<Rational> e( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-3 );
    Numeric<Rational> f( HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE+HIGHTEST_INTEGER_VALUE-4 );
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

    Numeric<Rational> g( 997002998000 );
    Numeric<Rational> h( 996005994003 );
    Numeric<Rational> j = g * h;
    Numeric<Rational> i( -997002998000 );
    EXPECT_EQ( carl::gcd( j.toRational(), i.toRational() ), carl::gcd( j, i ).toRational() );
}

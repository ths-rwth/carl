#include "gtest/gtest.h"
#include "carl/interval/FloatInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include <gmpxx.h>

using namespace carl;

TEST(FloatInterval, Constructor)
{
    FloatInterval<mpfr_t> test1 = FloatInterval<mpfr_t>(-1, BoundType::WEAK, 1, BoundType::WEAK);
    FloatInterval<mpfr_t> test2 = FloatInterval<mpfr_t>(-1, BoundType::STRICT, 1, BoundType::STRICT);
    FloatInterval<mpfr_t> test3 = FloatInterval<mpfr_t>(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(FloatInterval<mpfr_t>::emptyInterval(), FloatInterval<mpfr_t>(1, BoundType::WEAK, -1, BoundType::WEAK));
    FloatInterval<mpfr_t> tmp = FloatInterval<mpfr_t>(1, BoundType::WEAK, -1, BoundType::WEAK);
    tmp.dbgprint();
    FloatInterval<mpfr_t> tmp2 = FloatInterval<mpfr_t>::emptyInterval();
    tmp2.dbgprint();
    EXPECT_EQ(tmp, FloatInterval<mpfr_t>(1, BoundType::WEAK, -1, BoundType::WEAK));
    std::cout << "Equal: " << (tmp == tmp2) << std::endl; 
    FloatInterval<mpfr_t> test5 = FloatInterval<mpfr_t>::unboundedInterval();
    FloatInterval<mpfr_t> test6 = FloatInterval<mpfr_t>::emptyInterval();
//    FloatInterval<mpfr_t> test7 = FloatInterval<mpfr_t>((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::WEAK);
    FloatInterval<mpfr_t> test8 = FloatInterval<mpfr_t>(2, BoundType::STRICT, 0, BoundType::INFTY);
    SUCCEED();
}

TEST(FloatInterval, Getters)
{
    FloatInterval<mpfr_t> test1 = FloatInterval<mpfr_t>(-1.0, BoundType::WEAK, 1.0, BoundType::STRICT);
    EXPECT_EQ(FLOAT_T<mpfr_t>(-1.0).toString(), test1.left().toString());
    EXPECT_EQ(1, test1.right());
    EXPECT_EQ(BoundType::WEAK, test1.leftType());
    EXPECT_EQ(BoundType::STRICT, test1.rightType());
    FloatInterval<mpfr_t> test2 = FloatInterval<mpfr_t>(-1, BoundType::WEAK, 1, BoundType::STRICT);
    EXPECT_EQ(-1, test2.left());
    EXPECT_EQ(1, test2.right());
    EXPECT_EQ(BoundType::WEAK, test2.leftType());
    EXPECT_EQ(BoundType::STRICT, test2.rightType());
//    FloatInterval<mpfr_t> test3 = FloatInterval<mpfr_t>((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::STRICT);
//    EXPECT_EQ(-1, test3.left());
//    EXPECT_EQ(1, test3.right());
//    EXPECT_EQ(BoundType::WEAK, test3.leftType());
//    EXPECT_EQ(BoundType::STRICT, test3.rightType());
//    FloatInterval<mpfr_t> test4 = FloatInterval<mpfr_t>((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::STRICT);
//    EXPECT_EQ(-1, test4.left());
//    EXPECT_EQ(1, test4.right());
//    EXPECT_EQ(BoundType::WEAK, test4.leftType());
//    EXPECT_EQ(BoundType::STRICT, test4.rightType());
}

//TEST(FloatInterval<mpfr_t>, Addition)
//{
//    FloatInterval<mpfr_t> a0 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> a1 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> a2 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY);
//    FloatInterval<mpfr_t> a3 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY);
//    
//    FloatInterval<mpfr_t> b0 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> b1 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> b2 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY);
//    FloatInterval<mpfr_t> b3 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY);
//    
//    FloatInterval<mpfr_t> result;
//    
//    result = a0.add(b0);
//    EXPECT_EQ( FloatInterval<mpfr_t>(-1, BoundType::INFTY, 4, BoundType::WEAK), result);
//    result = a0.add(b1);
//    EXPECT_EQ( FloatInterval<mpfr_t>(-1, BoundType::INFTY, 4, BoundType::WEAK), result);
//    result = a0.add(b2);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a0.add(b3);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    
//    result = a1.add(b0);
//    EXPECT_EQ( FloatInterval<mpfr_t>(-1, BoundType::INFTY, 4, BoundType::WEAK), result);
//    result = a1.add(b1);
//    EXPECT_EQ( FloatInterval<mpfr_t>(-2, BoundType::WEAK, 4, BoundType::WEAK), result);
//    result = a1.add(b2);
//    EXPECT_EQ( FloatInterval<mpfr_t>(-2, BoundType::WEAK, 1, BoundType::INFTY), result);
//    result = a1.add(b3);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    
//    result = a2.add(b0);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a2.add(b1);
//    EXPECT_EQ( FloatInterval<mpfr_t>(-2, BoundType::WEAK, 1, BoundType::INFTY), result);
//    result = a2.add(b2);
//    EXPECT_EQ( FloatInterval<mpfr_t>(-2, BoundType::WEAK, 1, BoundType::INFTY), result);
//    result = a2.add(b3);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    
//    result = a3.add(b0);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a3.add(b1);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a3.add(b2);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a3.add(b3);
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//}
//
//
//TEST(FloatInterval<mpfr_t>, Subtraction)
//{
//    FloatInterval<mpfr_t> a0 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> a1 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> a2 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY);
//    FloatInterval<mpfr_t> a3 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY);
//    
//    FloatInterval<mpfr_t> b0 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> b1 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 2, BoundType::WEAK);
//    FloatInterval<mpfr_t> b2 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY);
//    FloatInterval<mpfr_t> b3 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY);
//    
//    FloatInterval<mpfr_t> result;
//    
//    result = a0.add(b0.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a0.add(b1.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
//    result = a0.add(b2.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
//    result = a0.add(b3.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    
//    result = a1.add(b0.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
//    result = a1.add(b1.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>(-3, BoundType::WEAK, 3, BoundType::WEAK), result);
//    result = a1.add(b2.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
//    result = a1.add(b3.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    
//    result = a2.add(b0.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
//    result = a2.add(b1.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
//    result = a2.add(b2.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a2.add(b3.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    
//    result = a3.add(b0.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a3.add(b1.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a3.add(b2.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//    result = a3.add(b3.inverse());
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result);
//}
//
//
//TEST(FloatInterval<mpfr_t>, Multiplication)
//{
//    FloatInterval<mpfr_t> c0  = FloatInterval<mpfr_t>( -2, BoundType::WEAK, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> c1  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> c2  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 2, BoundType::WEAK );
//    FloatInterval<mpfr_t> c3  = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> c4  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> c5  = FloatInterval<mpfr_t>( -2, BoundType::INFTY, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> c6  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> c7  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> c8  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY );
//
//    FloatInterval<mpfr_t> d0  = FloatInterval<mpfr_t>( -2, BoundType::WEAK, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> d1  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 2, BoundType::WEAK );
//    FloatInterval<mpfr_t> d2  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> d3  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 1, BoundType::INFTY );
//
//    FloatInterval<mpfr_t> d4  = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> d5  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> d6  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> d7  = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> d8  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> d9  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> d10 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> d11 = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> d12 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> result;
//
//    result = c0.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.right() * d0.right(), BoundType::WEAK, c0.left() * d0.left(), BoundType::WEAK ),
//                          result );
//    result = c0.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.right() * d5.right(), BoundType::WEAK, c0.left() * d5.left(), BoundType::WEAK ),
//                          result );
//    result = c1.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c1.right() * d0.left(), BoundType::WEAK, c1.left() * d0.left(), BoundType::WEAK ),
//                          result );
//    result = c1.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c1.right() * d5.left(), BoundType::WEAK, c1.left() * d5.left(), BoundType::WEAK ),
//                          result );
//    result = c2.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.right() * d0.left(), BoundType::WEAK, c2.left() * d0.right(), BoundType::WEAK ),
//                          result );
//    result = c2.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.right() * d5.left(), BoundType::WEAK, c2.left() * d5.right(), BoundType::WEAK ),
//                          result );
//    result = c3.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
//    result = c3.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
//    result = c4.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c4.right() * d0.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c4.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c4.right() * d5.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c5.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c5.right() * d0.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c5.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c5.right() * d5.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c6.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c6.left() * d0.left(), BoundType::WEAK ), result );
//    result = c6.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c6.left() * d5.left(), BoundType::WEAK ), result );
//    result = c7.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c7.left() * d0.right(), BoundType::WEAK ), result );
//    result = c7.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c7.left() * d5.right(), BoundType::WEAK ), result );
//    result = c8.mul( d0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//
//    result = c0.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.left() * d6.right(), BoundType::WEAK, c0.left() * d6.left(), BoundType::WEAK ),
//                          result );
//    result = c1.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( ( c1.left() * d6.right() < c1.right() * d6.left() ? c1.left() * d6.right() : c1.right() * d6.left()),
//                                         BoundType::WEAK,
//                                         c1.left() * d6.left() > c1.right() * d6.right() ? c1.left() * d6.left() : c1.right() * d6.right(),
//                                         BoundType::WEAK ),
//                          result );
//    result = c2.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.right() * d6.left(), BoundType::WEAK, c2.right() * d6.right(), BoundType::WEAK ),
//                          result );
//    result = c3.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c5.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c7.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//
//    result = c0.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.left() * d1.right(), BoundType::WEAK, c0.right() * d1.left(), BoundType::WEAK ),
//                          result );
//    result = c0.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.left() * d7.right(), BoundType::WEAK, c0.right() * d7.left(), BoundType::WEAK ),
//                          result );
//    result = c1.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c1.left() * d1.right(), BoundType::WEAK, c1.right() * d1.right(), BoundType::WEAK ),
//                          result );
//    result = c1.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c1.left() * d7.right(), BoundType::WEAK, c1.right() * d7.right(), BoundType::WEAK ),
//                          result );
//    result = c2.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.left() * d1.left(), BoundType::WEAK, c2.right() * d1.right(), BoundType::WEAK ),
//                          result );
//    result = c2.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.left() * d7.left(), BoundType::WEAK, c2.right() * d7.right(), BoundType::WEAK ),
//                          result );
//    result = c3.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c3.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c4.right() * d1.left(), BoundType::WEAK ), result );
//    result = c4.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c4.right() * d7.left(), BoundType::WEAK ), result );
//    result = c5.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c5.right() * d1.right(), BoundType::WEAK ), result );
//    result = c5.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c5.right() * d7.right(), BoundType::WEAK ), result );
//    result = c6.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c6.left() * d1.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c6.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c6.left() * d7.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c7.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c7.left() * d1.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c7.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c7.left() * d7.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c8.mul( d1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//
//    result = c0.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c1.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c2.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c3.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );;
//    result = c5.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c6.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c7.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c8.mul( d4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//
//    result = c0.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.right() * d2.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c0.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.right() * d8.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c1.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c1.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c2.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c2.left() * d2.right(), BoundType::WEAK ), result );
//    result = c2.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c2.left() * d8.right(), BoundType::WEAK ), result );
//    result = c3.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c3.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c4.right() * d2.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c4.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c4.right() * d8.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c5.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c5.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c7.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c7.left() * d2.right(), BoundType::WEAK ), result );
//    result = c7.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c7.left() * d8.right(), BoundType::WEAK ), result );
//    result = c8.mul( d2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//
//    result = c0.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.left() * d9.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c0.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c0.left() * d8.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c1.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c1.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c2.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c2.right() * d9.right(), BoundType::WEAK ), result );
//    result = c2.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c2.right() * d8.right(), BoundType::WEAK ), result );
//    result = c3.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c3.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c4.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c5.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c5.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c7.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c7.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
//    result = c8.mul( d9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//
//    result = c0.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c0.left() * d11.left(), BoundType::WEAK ), result );
//    result = c0.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c0.left() * d10.left(), BoundType::WEAK ), result );
//    result = c1.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c1.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c2.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.right() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c2.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.right() * d10.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c3.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c3.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
//    result = c4.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c5.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c5.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c7.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c7.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//
//    result = c0.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c0.right() * d11.left(), BoundType::WEAK ), result );
//    result = c0.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c0.right() * d3.left(), BoundType::WEAK ), result );
//    result = c1.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c1.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c2.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.left() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c2.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c2.left() * d3.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c3.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c3.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c4.right() * d11.left(), BoundType::WEAK ), result );
//    result = c4.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, c4.right() * d3.left(), BoundType::WEAK ), result );
//    result = c5.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c5.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c7.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c7.left() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c7.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( c7.left() * d3.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
//    result = c8.mul( d11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//
//    result = c0.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c1.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c2.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c3.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0 ), result );
//    result = c4.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c5.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c6.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c7.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//    result = c8.mul( d12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result );
//}
//
//
//TEST(FloatInterval<mpfr_t>, ExtendedDivision)
//{
//
//    FloatInterval<mpfr_t> a0  = FloatInterval<mpfr_t>( -2, BoundType::WEAK, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> a1  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> a2  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 2, BoundType::WEAK );
//    FloatInterval<mpfr_t> a3  = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> a4  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> a5  = FloatInterval<mpfr_t>( -2, BoundType::INFTY, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> a6  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> a7  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> a8  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY );
//
//    FloatInterval<mpfr_t> b0  = FloatInterval<mpfr_t>( -2, BoundType::WEAK, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> b1  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 2, BoundType::WEAK );
//    FloatInterval<mpfr_t> b2  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, -1, BoundType::WEAK );
//    FloatInterval<mpfr_t> b3  = FloatInterval<mpfr_t>( 1, BoundType::WEAK, 1, BoundType::INFTY );
//
//    FloatInterval<mpfr_t> b4  = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> b5  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> b6  = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> b7  = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> b8  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK );
//    FloatInterval<mpfr_t> b9  = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::WEAK );
//    FloatInterval<mpfr_t> b10 = FloatInterval<mpfr_t>( -1, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> b11 = FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> b12 = FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY );
//    FloatInterval<mpfr_t> result1, result2;
//    // Table 7 Tests: Division without containing 0
//    a0.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a0.right() / b0.left(), BoundType::WEAK, a0.left() / b0.right(), BoundType::WEAK ),
//                          result1 );
//    a1.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a1.right() / b0.right(), BoundType::WEAK, a1.left() / b0.right(), BoundType::WEAK ),
//                          result1 );
//    a2.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a2.right() / b0.right(), BoundType::WEAK, a2.left() / b0.left(), BoundType::WEAK ),
//                          result1 );
//    a3.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
//    a4.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a4.right() / b0.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a5.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a5.right() / b0.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a6.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a6.left() / b0.right(), BoundType::WEAK ), result1 );
//    a7.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a7.left() / b0.left(), BoundType::WEAK ), result1 );
//    a8.div_ext( result1, result2, b0 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );
//
//    a0.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a0.left() / b1.left(), BoundType::WEAK, a0.right() / b1.right(), BoundType::WEAK ),
//                          result1 );
//    a1.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a1.left() / b1.left(), BoundType::WEAK, a1.right() / b1.left(), BoundType::WEAK ),
//                          result1 );
//    a2.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a2.left() / b1.right(), BoundType::WEAK, a2.right() / b1.left(), BoundType::WEAK ),
//                          result1 );
//    a3.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
//    a4.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a4.right() / b1.right(), BoundType::WEAK ), result1 );
//    a5.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a5.right() / b1.left(), BoundType::WEAK ), result1 );
//    a6.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a6.left() / b1.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a7.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a7.left() / b1.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a8.div_ext( result1, result2, b1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );
//
//    a0.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, a0.left() / b2.right(), BoundType::WEAK ), result1 );
//    a1.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a1.right() / b2.right(), BoundType::WEAK, a1.left() / b2.right(), BoundType::WEAK ),
//                          result1 );
//    a2.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a2.right() / b2.right(), BoundType::WEAK, 0, BoundType::WEAK ), result1 );
//    a3.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
//    a4.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a5.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a5.right() / b2.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a6.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a6.left() / b2.right(), BoundType::WEAK ), result1 );
//    a7.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//    a8.div_ext( result1, result2, b2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );
//
//    a0.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a0.left() / b3.left(), BoundType::WEAK, 0, BoundType::WEAK ), result1 );
//    a1.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a1.left() / b3.left(), BoundType::WEAK, a1.right() / b3.left(), BoundType::WEAK ),
//                          result1 );
//    a2.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, a2.right() / b3.left(), BoundType::WEAK ), result1 );
//    a3.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
//    a4.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//    a5.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a5.right() / b3.left(), BoundType::WEAK ), result1 );
//    a6.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a6.left() / b3.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a7.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a8.div_ext( result1, result2, b3 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );
//
//    //Table 8 tests with division containin 0
//    a0.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::emptyInterval(), result1 );
//    a1.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::emptyInterval(), result1 );
//
//    a4.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::emptyInterval(), result1 );
//    a5.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::emptyInterval(), result1 );
//    a8.div_ext( result1, result2, b4 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a0.right() / b5.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a1.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a2.left() / b5.left(), BoundType::WEAK ), result1 );
//
//    a4.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a4.right() / b5.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a5.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a7.left() / b5.left(), BoundType::WEAK ), result1 );
//    a8.div_ext( result1, result2, b5 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a0.right() / b6.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a0.right() / b6.right(), BoundType::WEAK ), result2 );
//    a1.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a2.left() / b6.left(), BoundType::WEAK ), result1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a2.left() / b6.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
//
//    a4.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a4.right() / b6.right(), BoundType::WEAK ), result2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a4.right() / b6.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a5.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a7.left() / b6.left(), BoundType::WEAK ), result1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a7.left() / b6.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
//    a8.div_ext( result1, result2, b6 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a0.right() / b7.right(), BoundType::WEAK ), result1 );
//    a1.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a2.left() / b7.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//
//    a4.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a4.right() / b7.right(), BoundType::WEAK ), result1 );
//    a5.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a7.left() / b7.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a8.div_ext( result1, result2, b7 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a1.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//
//    a4.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a5.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//    a8.div_ext( result1, result2, b8 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a0.right() / b9.right(), BoundType::WEAK ), result2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a1.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a2.left() / b9.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
//
//    a4.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a4.right() / b9.right(), BoundType::WEAK ), result2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a5.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a7.left() / b9.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
//    a8.div_ext( result1, result2, b9 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a0.right() / b10.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a1.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a2.left() / b10.left(), BoundType::WEAK ), result1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );
//
//    a4.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( a4.right() / b10.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a5.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, a7.left() / b10.left(), BoundType::WEAK ), result1 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );
//    a8.div_ext( result1, result2, b10 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//    a1.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//
//    a4.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
//    a5.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
//    a8.div_ext( result1, result2, b11 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a0.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a1.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a2.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//
//    a4.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a5.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a6.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a7.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//    a8.div_ext( result1, result2, b12 );
//    EXPECT_EQ( FloatInterval<mpfr_t>::unboundedInterval(), result1 );
//}
//
//
//TEST(FloatInterval<mpfr_t>, Intersection)
//{
//    FloatInterval<mpfr_t> a1(-1,BoundType::WEAK,1,BoundType::WEAK);
//    
//    FloatInterval<mpfr_t> b01(2,BoundType::WEAK,3,BoundType::WEAK);
//    FloatInterval<mpfr_t> b02(1,BoundType::WEAK,3,BoundType::WEAK);
//    FloatInterval<mpfr_t> b03(0,BoundType::WEAK,3,BoundType::WEAK);
//    FloatInterval<mpfr_t> b04(-1,BoundType::WEAK,1,BoundType::WEAK);
//    FloatInterval<mpfr_t> b05(-2,BoundType::WEAK,0,BoundType::WEAK);
//    
//    FloatInterval<mpfr_t> b06(-2,BoundType::WEAK,-1,BoundType::WEAK);
//    FloatInterval<mpfr_t> b07(-3,BoundType::WEAK,-2,BoundType::WEAK);
//    FloatInterval<mpfr_t> b08(-1,BoundType::WEAK,-1,BoundType::WEAK);
//    FloatInterval<mpfr_t> b09(1,BoundType::WEAK,1,BoundType::WEAK);
//    FloatInterval<mpfr_t> b10(0,BoundType::WEAK,1,BoundType::WEAK);
//    
//    FloatInterval<mpfr_t> b11(-1,BoundType::WEAK,0,BoundType::WEAK);
//    FloatInterval<mpfr_t> b12(-0.5,BoundType::WEAK,0.5,BoundType::WEAK);
//    FloatInterval<mpfr_t> b13(-2,BoundType::WEAK,2,BoundType::WEAK);
//    FloatInterval<mpfr_t> b14(0,BoundType::STRICT,0,BoundType::STRICT);
//    FloatInterval<mpfr_t> b15(-1,BoundType::INFTY,1,BoundType::INFTY);
//    
//    FloatInterval<mpfr_t> b16(-1,BoundType::INFTY,-1,BoundType::WEAK);
//    FloatInterval<mpfr_t> b17(-1,BoundType::INFTY,0,BoundType::WEAK);
//    FloatInterval<mpfr_t> b18(-1,BoundType::INFTY,1,BoundType::WEAK);
//    FloatInterval<mpfr_t> b19(-1,BoundType::WEAK,1,BoundType::INFTY);
//    FloatInterval<mpfr_t> b20(0,BoundType::WEAK,1,BoundType::INFTY);
//    
//    FloatInterval<mpfr_t> b21(1,BoundType::WEAK,1,BoundType::INFTY);
//    
//    EXPECT_EQ(FloatInterval<mpfr_t>::emptyInterval(), a1.intersect(b01));
//    EXPECT_EQ(FloatInterval<mpfr_t>(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b02));
//    EXPECT_EQ(FloatInterval<mpfr_t>(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b03));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b04));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b05));
//    
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,-1,BoundType::WEAK), a1.intersect(b06));
//    EXPECT_EQ(FloatInterval<mpfr_t>::emptyInterval(), a1.intersect(b07));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,-1,BoundType::WEAK), a1.intersect(b08));
//    EXPECT_EQ(FloatInterval<mpfr_t>(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b09));
//    EXPECT_EQ(FloatInterval<mpfr_t>(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b10));
//    
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b11));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-0.5,BoundType::WEAK,0.5,BoundType::WEAK), a1.intersect(b12));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b13));
//    EXPECT_EQ(FloatInterval<mpfr_t>::emptyInterval(), a1.intersect(b14));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b15));
//    
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,-1,BoundType::WEAK), a1.intersect(b16));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b17));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b18));
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b19));
//    EXPECT_EQ(FloatInterval<mpfr_t>(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b20));
//    
//    EXPECT_EQ(FloatInterval<mpfr_t>(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b21));
//}
//
//TEST(FloatInterval<mpfr_t>, Split)
//{
//    FloatInterval<mpfr_t> i1(-1, BoundType::INFTY, 1, BoundType::INFTY);
//    FloatInterval<mpfr_t> i2(-1, BoundType::STRICT, 1, BoundType::STRICT);
//    FloatInterval<mpfr_t> i3(-1, BoundType::WEAK, 1, BoundType::WEAK);
//    FloatInterval<mpfr_t> i4(0, BoundType::STRICT, 0, BoundType::STRICT);
//    
//    FloatInterval<mpfr_t> resA;
//    FloatInterval<mpfr_t> resB;
//    
//    i1.split(resA, resB);
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1, BoundType::INFTY, 0, BoundType::STRICT), resA);
//    EXPECT_EQ(FloatInterval<mpfr_t>(0, BoundType::WEAK, 1, BoundType::INFTY), resB);
//    
//    i2.split(resA, resB);
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1, BoundType::STRICT, 0, BoundType::STRICT), resA);
//    EXPECT_EQ(FloatInterval<mpfr_t>(0, BoundType::WEAK, 1, BoundType::STRICT), resB);
//    
//    i3.split(resA, resB);
//    EXPECT_EQ(FloatInterval<mpfr_t>(-1, BoundType::WEAK, 0, BoundType::STRICT), resA);
//    EXPECT_EQ(FloatInterval<mpfr_t>(0, BoundType::WEAK, 1, BoundType::WEAK), resB);
//    
//    i4.split(resA, resB);
//    EXPECT_EQ(FloatInterval<mpfr_t>(0, BoundType::STRICT, 0, BoundType::STRICT), resA);
//    EXPECT_EQ(FloatInterval<mpfr_t>(0, BoundType::STRICT, 0, BoundType::STRICT), resB);
//    
//    // uniform multi-split
//    FloatInterval<mpfr_t> i5(0,BoundType::WEAK, 5, BoundType::STRICT);
//    
//    std::vector<FloatInterval<mpfr_t>> results;
//    i5.split(results,5);
//    EXPECT_EQ(5, results.size());
//    EXPECT_EQ(FloatInterval<mpfr_t>(0, BoundType::WEAK, 1, BoundType::STRICT), results.at(0));
//    EXPECT_EQ(FloatInterval<mpfr_t>(1, BoundType::WEAK, 2, BoundType::STRICT), results.at(1));
//    EXPECT_EQ(FloatInterval<mpfr_t>(2, BoundType::WEAK, 3, BoundType::STRICT), results.at(2));
//    EXPECT_EQ(FloatInterval<mpfr_t>(3, BoundType::WEAK, 4, BoundType::STRICT), results.at(3));
//    EXPECT_EQ(FloatInterval<mpfr_t>(4, BoundType::WEAK, 5, BoundType::STRICT), results.at(4));
//}
#include "gtest/gtest.h"
#include "carl/interval/ExactInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include <gmpxx.h>

using namespace carl;

TEST(ExactInterval, Constructor)
{
    ExactInterval<cln::cl_RA> test1((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::WEAK);
    ExactInterval<cln::cl_RA> test2((cln::cl_RA)-1, BoundType::STRICT, (cln::cl_RA)1, BoundType::STRICT);
    ExactInterval<cln::cl_RA> test3((cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY);
    EXPECT_EQ(ExactInterval<cln::cl_RA>((cln::cl_RA)0, BoundType::STRICT, (cln::cl_RA)0, BoundType::STRICT), ExactInterval<cln::cl_RA>::emptyExactInterval());
    ExactInterval<cln::cl_RA> test5 = ExactInterval<cln::cl_RA>::unboundedExactInterval();
    ExactInterval<cln::cl_RA> test6 = ExactInterval<cln::cl_RA>::emptyExactInterval();
    ExactInterval<mpq_class> test7 = ExactInterval<mpq_class>((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::WEAK);
    SUCCEED();
}

TEST(ExactInterval, Getters)
{
    ExactInterval<cln::cl_RA> test1 ((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::STRICT);
    EXPECT_EQ((cln::cl_RA)-1, test1.left());
    EXPECT_EQ((cln::cl_RA)1, test1.right());
    EXPECT_EQ(BoundType::WEAK, test1.leftType());
    EXPECT_EQ(BoundType::STRICT, test1.rightType());
    ExactInterval<cln::cl_RA> test2 ((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::STRICT);
    EXPECT_EQ((cln::cl_RA)-1, test2.left());
    EXPECT_EQ((cln::cl_RA)1, test2.right());
    EXPECT_EQ(BoundType::WEAK, test2.leftType());
    EXPECT_EQ(BoundType::STRICT, test2.rightType());
    ExactInterval<mpq_class> test3 ((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::STRICT);
    EXPECT_EQ((mpq_class)-1, test3.left());
    EXPECT_EQ((mpq_class)1, test3.right());
    EXPECT_EQ(BoundType::WEAK, test3.leftType());
    EXPECT_EQ(BoundType::STRICT, test3.rightType());
    ExactInterval<cln::cl_RA> test4 ((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::STRICT);
    EXPECT_EQ((cln::cl_RA)-1, test4.left());
    EXPECT_EQ((cln::cl_RA)1, test4.right());
    EXPECT_EQ(BoundType::WEAK, test4.leftType());
    EXPECT_EQ(BoundType::STRICT, test4.rightType());
	ExactInterval<cln::cl_RA> test5 ((cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK);
	EXPECT_TRUE(test5.isZero());
}

TEST(ExactInterval, Addition)
{
    ExactInterval<cln::cl_RA> a0 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> a1 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> a2 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY);
    ExactInterval<cln::cl_RA> a3 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY);
    
    ExactInterval<cln::cl_RA> b0 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> b1 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> b2 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY);
    ExactInterval<cln::cl_RA> b3 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY);
    
    ExactInterval<cln::cl_RA> result;
    
    result = a0.add(b0);
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)4, BoundType::WEAK), result);
    result = a0.add(b1);
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)4, BoundType::WEAK), result);
    result = a0.add(b2);
    EXPECT_EQ( true, result.unbounded());
    result = a0.add(b3);
    EXPECT_EQ( true, result.unbounded());
    
    result = a1.add(b0);
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)4, BoundType::WEAK), result);
    result = a1.add(b1);
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-2, BoundType::WEAK, (cln::cl_RA)4, BoundType::WEAK), result);
    result = a1.add(b2);
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-2, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY), result);
    result = a1.add(b3);
    EXPECT_EQ( true, result.unbounded());
    
    result = a2.add(b0);
    EXPECT_EQ( true, result.unbounded());
    result = a2.add(b1);
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-2, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY), result);
    result = a2.add(b2);
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-2, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY), result);
    result = a2.add(b3);
    EXPECT_EQ( true, result.unbounded());
    
    result = a3.add(b0);
    EXPECT_EQ( true, result.unbounded());
    result = a3.add(b1);
    EXPECT_EQ( true, result.unbounded());
    result = a3.add(b2);
    EXPECT_EQ( true, result.unbounded());
    result = a3.add(b3);
    EXPECT_EQ( true, result.unbounded());
}


TEST(ExactInterval, Subtraction)
{
    ExactInterval<cln::cl_RA> a0 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> a1 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> a2 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY);
    ExactInterval<cln::cl_RA> a3 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY);
    
    ExactInterval<cln::cl_RA> b0 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> b1 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK);
    ExactInterval<cln::cl_RA> b2 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY);
    ExactInterval<cln::cl_RA> b3 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY);
    
    ExactInterval<cln::cl_RA> result;
    
    result = a0.add(b0.inverse());
    EXPECT_EQ( true, result.unbounded());
    result = a0.add(b1.inverse());
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)3, BoundType::WEAK), result);
    result = a0.add(b2.inverse());
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)3, BoundType::WEAK), result);
    result = a0.add(b3.inverse());
    EXPECT_EQ( true, result.unbounded());
    
    result = a1.add(b0.inverse());
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-3, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY), result);
    result = a1.add(b1.inverse());
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-3, BoundType::WEAK, (cln::cl_RA)3, BoundType::WEAK), result);
    result = a1.add(b2.inverse());
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)3, BoundType::WEAK), result);
    result = a1.add(b3.inverse());
    EXPECT_EQ( true, result.unbounded());
    
    result = a2.add(b0.inverse());
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-3, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY), result);
    result = a2.add(b1.inverse());
    EXPECT_EQ( ExactInterval<cln::cl_RA>((cln::cl_RA)-3, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY), result);
    result = a2.add(b2.inverse());
    EXPECT_EQ( true, result.unbounded());
    result = a2.add(b3.inverse());
    EXPECT_EQ( true, result.unbounded());
    
    result = a3.add(b0.inverse());
    EXPECT_EQ( true, result.unbounded());
    result = a3.add(b1.inverse());
    EXPECT_EQ( true, result.unbounded());
    result = a3.add(b2.inverse());
    EXPECT_EQ( true, result.unbounded());
    result = a3.add(b3.inverse());
    EXPECT_EQ( true, result.unbounded());

	{
		ExactInterval<cln::cl_RA> interval((cln::cl_RA)72, (cln::cl_RA)73, BoundType::STRICT);
		ExactInterval<cln::cl_RA> result((cln::cl_RA)-173, (cln::cl_RA)-172, BoundType::STRICT);
		EXPECT_EQ(result, -cln::cl_RA(70) - interval);
	}
}


TEST(ExactInterval, Multiplication)
{
    ExactInterval<cln::cl_RA> c0  ( (cln::cl_RA) -2, BoundType::WEAK, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> c1  ( (cln::cl_RA) -1, BoundType::WEAK, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> c2  ( (cln::cl_RA) 1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK );
    ExactInterval<cln::cl_RA> c3  ( (cln::cl_RA) 0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> c4  ( (cln::cl_RA) -1, BoundType::INFTY, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> c5  ( (cln::cl_RA) -2, BoundType::INFTY, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> c6  ( (cln::cl_RA) -1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> c7  ( (cln::cl_RA) 1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> c8  ( (cln::cl_RA) -1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY );

    ExactInterval<cln::cl_RA> d0  ( (cln::cl_RA) -2, BoundType::WEAK, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d1  ( (cln::cl_RA) 1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d2  ( (cln::cl_RA) -1, BoundType::INFTY, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d3  ( (cln::cl_RA) 1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );

    ExactInterval<cln::cl_RA> d4  ( (cln::cl_RA) 0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d5  ( (cln::cl_RA) -1, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d6  ( (cln::cl_RA) -1, BoundType::WEAK, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d7  ( (cln::cl_RA) 0, BoundType::WEAK, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d8  ( (cln::cl_RA) -1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d9  ( (cln::cl_RA) -1, BoundType::INFTY, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> d10 ( (cln::cl_RA) -1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> d11 ( (cln::cl_RA) 0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> d12 ( (cln::cl_RA) -1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> result;

    result = c0.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.right() * d0.right(), BoundType::WEAK, c0.left() * d0.left(), BoundType::WEAK ),
                          result );
    result = c0.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.right() * d5.right(), BoundType::WEAK, c0.left() * d5.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c1.right() * d0.left(), BoundType::WEAK, c1.left() * d0.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c1.right() * d5.left(), BoundType::WEAK, c1.left() * d5.left(), BoundType::WEAK ),
                          result );
    result = c2.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.right() * d0.left(), BoundType::WEAK, c2.left() * d0.right(), BoundType::WEAK ),
                          result );
    result = c2.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.right() * d5.left(), BoundType::WEAK, c2.left() * d5.right(), BoundType::WEAK ),
                          result );
    result = c3.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result );
    result = c3.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result );
    result = c4.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c4.right() * d0.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c4.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c4.right() * d5.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c5.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c5.right() * d0.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c5.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c5.right() * d5.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c6.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c6.left() * d0.left(), BoundType::WEAK ), result );
    result = c6.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c6.left() * d5.left(), BoundType::WEAK ), result );
    result = c7.mul( d0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c7.left() * d0.right(), BoundType::WEAK ), result );
    result = c7.mul( d5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c7.left() * d5.right(), BoundType::WEAK ), result );
    result = c8.mul( d0 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d5 );
    EXPECT_EQ( true, result.unbounded());

    result = c0.mul( d6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.left() * d6.right(), BoundType::WEAK, c0.left() * d6.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( ( c1.left() * d6.right() < c1.right() * d6.left() ? c1.left() * d6.right() : c1.right() * d6.left()),
                                         BoundType::WEAK,
                                         c1.left() * d6.left() > c1.right() * d6.right() ? c1.left() * d6.left() : c1.right() * d6.right(),
                                         BoundType::WEAK ),
                          result );
    result = c2.mul( d6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.right() * d6.left(), BoundType::WEAK, c2.right() * d6.right(), BoundType::WEAK ),
                          result );
    result = c3.mul( d6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d6 );
    EXPECT_EQ( true, result.unbounded());
    result = c5.mul( d6 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d6 );
    EXPECT_EQ( true, result.unbounded());
    result = c7.mul( d6 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d6 );
    EXPECT_EQ( true, result.unbounded());

    result = c0.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.left() * d1.right(), BoundType::WEAK, c0.right() * d1.left(), BoundType::WEAK ),
                          result );
    result = c0.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.left() * d7.right(), BoundType::WEAK, c0.right() * d7.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c1.left() * d1.right(), BoundType::WEAK, c1.right() * d1.right(), BoundType::WEAK ),
                          result );
    result = c1.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c1.left() * d7.right(), BoundType::WEAK, c1.right() * d7.right(), BoundType::WEAK ),
                          result );
    result = c2.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.left() * d1.left(), BoundType::WEAK, c2.right() * d1.right(), BoundType::WEAK ),
                          result );
    result = c2.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.left() * d7.left(), BoundType::WEAK, c2.right() * d7.right(), BoundType::WEAK ),
                          result );
    result = c3.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c3.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c4.right() * d1.left(), BoundType::WEAK ), result );
    result = c4.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c4.right() * d7.left(), BoundType::WEAK ), result );
    result = c5.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c5.right() * d1.right(), BoundType::WEAK ), result );
    result = c5.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)1, BoundType::INFTY, c5.right() * d7.right(), BoundType::WEAK ), result );
    result = c6.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c6.left() * d1.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c6.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c6.left() * d7.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c7.mul( d1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c7.left() * d1.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c7.mul( d7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c7.left() * d7.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c8.mul( d1 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d7 );
    EXPECT_EQ( true, result.unbounded());

    result = c0.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c1.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c2.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c3.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );;
    result = c5.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c6.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c7.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c8.mul( d4 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );

    result = c0.mul( d2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.right() * d2.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.right() * d8.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c1.mul( d2 );
    EXPECT_EQ( true, result.unbounded());
    result = c1.mul( d8 );
    EXPECT_EQ( true, result.unbounded());
    result = c2.mul( d2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c2.left() * d2.right(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c2.left() * d8.right(), BoundType::WEAK ), result );
    result = c3.mul( d2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c4.right() * d2.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c4.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c4.right() * d8.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d2 );
    EXPECT_EQ( true, result.unbounded());
    result = c5.mul( d8 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d2 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d8 );
    EXPECT_EQ( true, result.unbounded());
    result = c7.mul( d2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c7.left() * d2.right(), BoundType::WEAK ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c7.left() * d8.right(), BoundType::WEAK ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d8 );
    EXPECT_EQ( true, result.unbounded());

    result = c0.mul( d9 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.left() * d9.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c0.left() * d8.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( true, result.unbounded());
    result = c1.mul( d8 );
    EXPECT_EQ( true, result.unbounded());
    result = c2.mul( d9 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c2.right() * d9.right(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c2.right() * d8.right(), BoundType::WEAK ), result );
    result = c3.mul( d9 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d9 );
    EXPECT_EQ( true, result.unbounded());
    result = c4.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c5.mul( d9 );
    EXPECT_EQ( true, result.unbounded());
    result = c5.mul( d8 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d9 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d8 );
    EXPECT_EQ( true, result.unbounded());
    result = c7.mul( d9 );
    EXPECT_EQ( true, result.unbounded());
    result = c7.mul( d8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result );
    result = c8.mul( d9 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d8 );
    EXPECT_EQ( true, result.unbounded());

    result = c0.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c0.left() * d11.left(), BoundType::WEAK ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c0.left() * d10.left(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c1.mul( d10 );
    EXPECT_EQ( true, result.unbounded());
    result = c2.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.right() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c2.mul( d10 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.right() * d10.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c3.mul( d10 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result );
    result = c4.mul( d10 );
    EXPECT_EQ( true, result.unbounded());
    result = c5.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c5.mul( d10 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d10 );
    EXPECT_EQ( true, result.unbounded());
    result = c7.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c7.mul( d10 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d10 );
    EXPECT_EQ( true, result.unbounded());

    result = c0.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c0.right() * d11.left(), BoundType::WEAK ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c0.right() * d3.left(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c1.mul( d3 );
    EXPECT_EQ( true, result.unbounded());
    result = c2.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.left() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c2.mul( d3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c2.left() * d3.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c3.mul( d3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c4.right() * d11.left(), BoundType::WEAK ), result );
    result = c4.mul( d3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, c4.right() * d3.left(), BoundType::WEAK ), result );
    result = c5.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c5.mul( d3 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d3 );
    EXPECT_EQ( true, result.unbounded());
    result = c7.mul( d11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c7.left() * d11.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( c7.left() * d3.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result );
    result = c8.mul( d11 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d3 );
    EXPECT_EQ( true, result.unbounded());

    result = c0.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
    result = c1.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
    result = c2.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
    result = c3.mul( d12 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0 ), result );
    result = c4.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
    result = c5.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
    result = c6.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
    result = c7.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
    result = c8.mul( d12 );
    EXPECT_EQ( true, result.unbounded());
}


TEST(ExactInterval, ExtendedDivision)
{

    ExactInterval<cln::cl_RA> a0  ( (cln::cl_RA)-2, BoundType::WEAK, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> a1  ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> a2  ( (cln::cl_RA)1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK );
    ExactInterval<cln::cl_RA> a3  ( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> a4  ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> a5  ( (cln::cl_RA)-2, BoundType::INFTY, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> a6  ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> a7  ( (cln::cl_RA)1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> a8  ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY );

    ExactInterval<cln::cl_RA> b0  ( (cln::cl_RA)-2, BoundType::WEAK, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b1  ( (cln::cl_RA)1, BoundType::WEAK, (cln::cl_RA)2, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b2  ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)-1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b3  ( (cln::cl_RA)1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );

    ExactInterval<cln::cl_RA> b4  ( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b5  ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b6  ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b7  ( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b8  ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b9  ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::WEAK );
    ExactInterval<cln::cl_RA> b10 ( (cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> b11 ( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> b12 ( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY );
    ExactInterval<cln::cl_RA> result1, result2;
    // Table 7 Tests: Division without containing 0
    a0.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a0.right() / b0.left(), BoundType::WEAK, a0.left() / b0.right(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a1.right() / b0.right(), BoundType::WEAK, a1.left() / b0.right(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a2.right() / b0.right(), BoundType::WEAK, a2.left() / b0.left(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a4.right() / b0.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a5.right() / b0.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a6.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a6.left() / b0.right(), BoundType::WEAK ), result1 );
    a7.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a7.left() / b0.left(), BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b0 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a0.left() / b1.left(), BoundType::WEAK, a0.right() / b1.right(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a1.left() / b1.left(), BoundType::WEAK, a1.right() / b1.left(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a2.left() / b1.right(), BoundType::WEAK, a2.right() / b1.left(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a4.right() / b1.right(), BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a5.right() / b1.left(), BoundType::WEAK ), result1 );
    a6.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a6.left() / b1.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a7.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a7.left() / b1.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a8.div_ext( result1, result2, b1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY ), result1 );

    a0.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, a0.left() / b2.right(), BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a1.right() / b2.right(), BoundType::WEAK, a1.left() / b2.right(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a2.right() / b2.right(), BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a3.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a5.right() / b2.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a6.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a6.left() / b2.right(), BoundType::WEAK ), result1 );
    a7.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY ), result1 );

    a0.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a0.left() / b3.left(), BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a1.left() / b3.left(), BoundType::WEAK, a1.right() / b3.left(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, a2.right() / b3.left(), BoundType::WEAK ), result1 );
    a3.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a5.right() / b3.left(), BoundType::WEAK ), result1 );
    a6.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a6.left() / b3.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a7.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a8.div_ext( result1, result2, b3 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)1, BoundType::INFTY ), result1 );

    //Table 8 tests with division containin 0
    a0.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.empty());
    a1.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.empty());

    a4.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.empty());
    a5.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.empty());
    a8.div_ext( result1, result2, b4 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a0.right() / b5.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b5 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a2.left() / b5.left(), BoundType::WEAK ), result1 );

    a4.div_ext( result1, result2, b5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a4.right() / b5.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b5 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b5 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b5 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a7.left() / b5.left(), BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b5 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a0.right() / b6.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a0.right() / b6.right(), BoundType::WEAK ), result2 );
    a1.div_ext( result1, result2, b6 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a2.left() / b6.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a2.left() / b6.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result2 );

    a4.div_ext( result1, result2, b6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a4.right() / b6.right(), BoundType::WEAK ), result2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a4.right() / b6.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b6 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b6 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b6 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a7.left() / b6.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a7.left() / b6.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result2 );
    a8.div_ext( result1, result2, b6 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a0.right() / b7.right(), BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b7 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a2.left() / b7.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );

    a4.div_ext( result1, result2, b7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a4.right() / b7.right(), BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b7 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b7 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b7 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a7.left() / b7.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( result1, result2, b7 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b8 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );

    a4.div_ext( result1, result2, b8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b8 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b8 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b8 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b8 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b9 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a0.right() / b9.right(), BoundType::WEAK ), result2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b9 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b9 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a2.left() / b9.right(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result2 );

    a4.div_ext( result1, result2, b9 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)1, BoundType::INFTY, a4.right() / b9.right(), BoundType::WEAK ), result2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b9 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b9 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b9 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a7.left() / b9.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( result1, result2, b9 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b10 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a0.right() / b10.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b10 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b10 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a2.left() / b10.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result2 );

    a4.div_ext( result1, result2, b10 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result2 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( a4.right() / b10.left(), BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b10 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b10 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b10 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, a7.left() / b10.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result2 );
    a8.div_ext( result1, result2, b10 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b11 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );

    a4.div_ext( result1, result2, b11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)-1, BoundType::INFTY, (cln::cl_RA)0, BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b11 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b11 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b11 );
    EXPECT_EQ( ExactInterval<cln::cl_RA>( (cln::cl_RA)0, BoundType::WEAK, (cln::cl_RA)1, BoundType::INFTY ), result1 );
    a8.div_ext( result1, result2, b11 );
    EXPECT_EQ( true, result1.unbounded());

    a0.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());
    a1.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());
    a2.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());

    a4.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());
    a5.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());
    a6.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());
    a7.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());
    a8.div_ext( result1, result2, b12 );
    EXPECT_EQ( true, result1.unbounded());
}


TEST(ExactInterval, Intersection)
{
    ExactInterval<cln::cl_RA> a1( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK);
    
    ExactInterval<cln::cl_RA> b01( (cln::cl_RA)2,BoundType::WEAK,(cln::cl_RA)3,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b02( (cln::cl_RA)1,BoundType::WEAK,(cln::cl_RA)3,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b03( (cln::cl_RA)0,BoundType::WEAK,(cln::cl_RA)3,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b04( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b05( (cln::cl_RA)-2,BoundType::WEAK,(cln::cl_RA)0,BoundType::WEAK);
    
    ExactInterval<cln::cl_RA> b06( (cln::cl_RA)-2,BoundType::WEAK,(cln::cl_RA)-1,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b07( (cln::cl_RA)-3,BoundType::WEAK,(cln::cl_RA)-2,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b08( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)-1,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b09( (cln::cl_RA)1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b10( (cln::cl_RA)0,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK);
    
    ExactInterval<cln::cl_RA> b11( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)0,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b12( (cln::cl_RA)-1/2,BoundType::WEAK,(cln::cl_RA)1/2,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b13( (cln::cl_RA)-2,BoundType::WEAK,(cln::cl_RA)2,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b14( (cln::cl_RA)0,BoundType::STRICT,(cln::cl_RA)0,BoundType::STRICT);
    ExactInterval<cln::cl_RA> b15( (cln::cl_RA)-1,BoundType::INFTY,(cln::cl_RA)1,BoundType::INFTY);
    
    ExactInterval<cln::cl_RA> b16( (cln::cl_RA)-1,BoundType::INFTY,(cln::cl_RA)-1,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b17( (cln::cl_RA)-1,BoundType::INFTY,(cln::cl_RA)0,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b18( (cln::cl_RA)-1,BoundType::INFTY,(cln::cl_RA)1,BoundType::WEAK);
    ExactInterval<cln::cl_RA> b19( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::INFTY);
    ExactInterval<cln::cl_RA> b20( (cln::cl_RA)0,BoundType::WEAK,(cln::cl_RA)1,BoundType::INFTY);
    
    ExactInterval<cln::cl_RA> b21( (cln::cl_RA)1,BoundType::WEAK,(cln::cl_RA)1,BoundType::INFTY);
    
    
    EXPECT_EQ(ExactInterval<cln::cl_RA>::emptyExactInterval(), a1.intersect(b01));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b02));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)0,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b03));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b04));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)0,BoundType::WEAK), a1.intersect(b05));
    
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)-1,BoundType::WEAK), a1.intersect(b06));
    EXPECT_EQ(ExactInterval<cln::cl_RA>::emptyExactInterval(), a1.intersect(b07));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)-1,BoundType::WEAK), a1.intersect(b08));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b09));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)0,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b10));
    
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)0,BoundType::WEAK), a1.intersect(b11));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1/2,BoundType::WEAK,(cln::cl_RA)1/2,BoundType::WEAK), a1.intersect(b12));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b13));
    EXPECT_EQ(true, a1.intersect(b14).empty());
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b15));
    
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)-1,BoundType::WEAK), a1.intersect(b16));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)0,BoundType::WEAK), a1.intersect(b17));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b18));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)-1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b19));
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)0,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b20));
    
    EXPECT_EQ(ExactInterval<cln::cl_RA>( (cln::cl_RA)1,BoundType::WEAK,(cln::cl_RA)1,BoundType::WEAK), a1.intersect(b21));
}
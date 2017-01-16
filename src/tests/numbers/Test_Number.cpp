#include "gtest/gtest.h"
#include "../../carl/numbers/numbers.h"

#include "../../carl/numbers/NumberMpz.h"
#include "../../carl/numbers/NumberMpq.h"
#include "../../carl/numbers/NumberClI.h"
#include "../../carl/numbers/NumberClRA.h"

TEST(Number, Constructor)
{
//mpz
	mpz_class mpz(2);	
	carl::Number<mpz_class> z1, z2, z3, z4, z5, z6, z7, z8, z9, z10;
	ASSERT_NO_THROW(z1 = carl::Number<mpz_class>());
	ASSERT_NO_THROW(z2 = carl::Number<mpz_class>(mpz));
	ASSERT_NO_THROW(z3 = carl::Number<mpz_class>(std::move(mpz)));
	ASSERT_NO_THROW(z4 = carl::Number<mpz_class>(z1));
	ASSERT_NO_THROW(z5 = carl::Number<mpz_class>(3ll));
	ASSERT_NO_THROW(z6 = carl::Number<mpz_class>(4ull));
	ASSERT_NO_THROW(z7 = carl::Number<mpz_class>("-30"));
	ASSERT_NO_THROW(z8 = carl::Number<mpz_class>(3));
	ASSERT_NO_THROW(z9 = carl::Number<mpz_class>(std::move(z1)));


	EXPECT_EQ(z1.getValue(),mpz_class(0));
	EXPECT_EQ(z2.getValue(),mpz_class(2));
	EXPECT_EQ(z3.getValue(),mpz_class(2));
	EXPECT_EQ(z4.getValue(),mpz_class(0));
	EXPECT_EQ(z5.getValue(),mpz_class(3));
	EXPECT_EQ(z6.getValue(),mpz_class(4));
	EXPECT_EQ(z7.getValue(),mpz_class(-30));
	EXPECT_EQ(z8.getValue(),mpz_class(3));
	EXPECT_EQ(z9.getValue(),mpz_class(0));
	

//mpq class
	mpq_class mpq(2);	
	carl::Number<mpq_class> q1, q2, q3, q4, q5, q6, q7, q8, q9, q10, q11, q12, q13, q14, q15, q16;
	ASSERT_NO_THROW(q1 = carl::Number<mpq_class>());
	ASSERT_NO_THROW(q2 = carl::Number<mpq_class>(mpq));
	ASSERT_NO_THROW(q3 = carl::Number<mpq_class>(std::move(mpq)));
	ASSERT_NO_THROW(q4 = carl::Number<mpq_class>(q1));
	ASSERT_NO_THROW(q5 = carl::Number<mpq_class>(3ll));
	ASSERT_NO_THROW(q6 = carl::Number<mpq_class>(4ull));
	ASSERT_NO_THROW(q7 = carl::Number<mpq_class>("-30"));
	ASSERT_NO_THROW(q8 = carl::Number<mpq_class>(3));

	ASSERT_NO_THROW(q9 = carl::Number<mpq_class>(3.5f));
	ASSERT_NO_THROW(q10 = carl::Number<mpq_class>(3.5));
	ASSERT_NO_THROW(q11 = carl::Number<mpq_class>(z8,z7));
	ASSERT_NO_THROW(q12 = carl::Number<mpq_class>(z8));
	ASSERT_NO_THROW(q13 = carl::Number<mpq_class>(mpz));
	ASSERT_NO_THROW(q14 = carl::Number<mpq_class>(std::move(q1)));


	EXPECT_EQ(q1.getValue(),mpq_class(0));
	EXPECT_EQ(q2.getValue(),mpq_class(2));
	EXPECT_EQ(q3.getValue(),mpq_class(2));
	EXPECT_EQ(q4.getValue(),mpq_class(0));
	EXPECT_EQ(q5.getValue(),mpq_class(3));
	EXPECT_EQ(q6.getValue(),mpq_class(4));
	EXPECT_EQ(q7.getValue(),mpq_class(-30));
	EXPECT_EQ(q8.getValue(),mpq_class(3));

	EXPECT_EQ(q9.getValue(),mpq_class(3.5));
	EXPECT_EQ(q10.getValue(),mpq_class(3.5));
	EXPECT_EQ(q11.getValue(),mpq_class(3,-30));
	EXPECT_EQ(q12.getValue(),mpq_class(3));
	EXPECT_EQ(q13.getValue(),mpq_class(2));
	EXPECT_EQ(q14.getValue(),mpq_class(0));


//cln::cl_I
#ifdef USE_CLN_NUMBERS
	cln::cl_I clI(2);	
	carl::Number<cln::cl_I> i1, i2, i3, i4, i5, i6, i7, i8, i9, i10;
	ASSERT_NO_THROW(i1 = carl::Number<cln::cl_I>());
	ASSERT_NO_THROW(i2 = carl::Number<cln::cl_I>(clI));
	ASSERT_NO_THROW(i3 = carl::Number<cln::cl_I>(std::move(clI)));
	ASSERT_NO_THROW(i4 = carl::Number<cln::cl_I>(z1));
	ASSERT_NO_THROW(i5 = carl::Number<cln::cl_I>(3ll));
	ASSERT_NO_THROW(i6 = carl::Number<cln::cl_I>(4ull));
	ASSERT_NO_THROW(i7 = carl::Number<cln::cl_I>((std::string)"-30"));
	ASSERT_NO_THROW(i8 = carl::Number<cln::cl_I>(3));
	ASSERT_NO_THROW(i9 = carl::Number<cln::cl_I>(std::move(i1)));
	ASSERT_NO_THROW(i10 = carl::Number<cln::cl_I>(z1));


	EXPECT_EQ(i1.getValue(),cln::cl_I(0));
	EXPECT_EQ(i2.getValue(),cln::cl_I(2));
	EXPECT_EQ(i3.getValue(),cln::cl_I(2));
	EXPECT_EQ(i4.getValue(),cln::cl_I(0));
	EXPECT_EQ(i5.getValue(),cln::cl_I(3));
	EXPECT_EQ(i6.getValue(),cln::cl_I(4));
	EXPECT_EQ(i7.getValue(),cln::cl_I(-30));
	EXPECT_EQ(i8.getValue(),cln::cl_I(3));
	EXPECT_EQ(i9.getValue(),cln::cl_I(0));
	EXPECT_EQ(i10.getValue(),cln::cl_I(0));

	//conversion constructor cl_i -> mpz
	ASSERT_NO_THROW(z10 = carl::Number<mpz_class>(i1));
	EXPECT_EQ(z10.getValue(),mpz_class(0));

//cln::cl_RA

	cln::cl_RA clRA(2);	
	carl::Number<cln::cl_RA> r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16;
	ASSERT_NO_THROW(r1 = carl::Number<cln::cl_RA>());
	ASSERT_NO_THROW(r2 = carl::Number<cln::cl_RA>(clRA));
	ASSERT_NO_THROW(r3 = carl::Number<cln::cl_RA>(std::move(clRA)));
	ASSERT_NO_THROW(r4 = carl::Number<cln::cl_RA>(clRA));
	ASSERT_NO_THROW(r5 = carl::Number<cln::cl_RA>(3ll));
	ASSERT_NO_THROW(r6 = carl::Number<cln::cl_RA>(4ull));
	ASSERT_NO_THROW(r7 = carl::Number<cln::cl_RA>((std::string)"-30"));
	ASSERT_NO_THROW(r8 = carl::Number<cln::cl_RA>(3));

	ASSERT_NO_THROW(r9 = carl::Number<cln::cl_RA>(3.5f));
	ASSERT_NO_THROW(r10 = carl::Number<cln::cl_RA>(3.5));
	ASSERT_NO_THROW(r11 = carl::Number<cln::cl_RA>(i8,i7));
	ASSERT_NO_THROW(r12 = carl::Number<cln::cl_RA>(i8));
	ASSERT_NO_THROW(r13 = carl::Number<cln::cl_RA>(clI));
	ASSERT_NO_THROW(r14 = carl::Number<cln::cl_RA>(std::move(r1)));

	ASSERT_NO_THROW(r15 = carl::Number<cln::cl_RA>(z1));
	ASSERT_NO_THROW(r16 = carl::Number<cln::cl_RA>(q1));


	EXPECT_EQ(r1.getValue(),cln::cl_RA(0));
	EXPECT_EQ(r2.getValue(),cln::cl_RA(2));
	EXPECT_EQ(r3.getValue(),cln::cl_RA(2));
	EXPECT_EQ(r4.getValue(),cln::cl_RA(2));
	EXPECT_EQ(r5.getValue(),cln::cl_RA(3));
	EXPECT_EQ(r6.getValue(),cln::cl_RA(4));
	EXPECT_EQ(r7.getValue(),cln::cl_RA(-30));
	EXPECT_EQ(r8.getValue(),cln::cl_RA(3));


	std::string s = "7/2";
	EXPECT_EQ(r9.getValue(),cln::cl_RA(s.c_str()));
	EXPECT_EQ(r10.getValue(),cln::cl_RA(s.c_str()));
	s = "-1/10";
	EXPECT_EQ(r11.getValue(),cln::cl_RA(s.c_str()));
	EXPECT_EQ(r12.getValue(),cln::cl_RA(3));
	EXPECT_EQ(r13.getValue(),cln::cl_RA(2));
	EXPECT_EQ(r14.getValue(),cln::cl_RA(0));

	EXPECT_EQ(r15.getValue(),cln::cl_RA(0));
	EXPECT_EQ(r16.getValue(),cln::cl_RA(0));

//test conversion constructors into mpq:
	ASSERT_NO_THROW(q15 = carl::Number<mpq_class>(i1));
	ASSERT_NO_THROW(q16 = carl::Number<mpq_class>(r1));

	EXPECT_EQ(q15.getValue(),mpq_class(0));
	EXPECT_EQ(q16.getValue(),mpq_class(0));
#endif
}


TEST(Number,IntegerOperations) {

//mpz
	carl::Number<mpz_class> z(1);
	EXPECT_FALSE(z.isZero());
	EXPECT_TRUE(z.isOne());
	EXPECT_TRUE(z.isPositive());
	EXPECT_FALSE(z.isNegative());
	EXPECT_TRUE(z.isInteger());
	EXPECT_EQ(z.bitsize(),1);
	EXPECT_EQ(z.toDouble(),1);
	EXPECT_EQ(z.abs(),carl::Number<mpz_class>(1)); 
	carl::Number<mpz_class> x(3);
	EXPECT_EQ(z.gcd(x),carl::Number<mpz_class>(1));
	EXPECT_EQ(z.lcm(x),carl::Number<mpz_class>(3));
	EXPECT_EQ(x.pow(2),carl::Number<mpz_class>(9));
	
	EXPECT_EQ(z.mod(x),z);	
	EXPECT_EQ(z.remainder(x),z);	
	EXPECT_EQ(z.quotient(x),carl::Number<mpz_class>(0));
	EXPECT_EQ(z/x,carl::Number<mpz_class>(0));
	EXPECT_EQ(z.div(z),z);

	EXPECT_EQ(z.toInt<carl::sint>(),1);
	EXPECT_EQ(z.toInt<carl::uint>(),1);

	EXPECT_EQ(z.toString(),"1");


//clI
#ifdef USE_CLN_NUMBERS
	carl::Number<cln::cl_I> i(1);
	EXPECT_FALSE(i.isZero());
	EXPECT_TRUE(i.isOne());
	EXPECT_TRUE(i.isPositive());
	EXPECT_FALSE(i.isNegative());
	EXPECT_TRUE(i.isInteger());
	EXPECT_EQ(i.bitsize(),1);
	EXPECT_EQ(i.toDouble(),1);
	EXPECT_EQ(i.abs(),carl::Number<cln::cl_I>(1)); 
	carl::Number<cln::cl_I> j(3);
	EXPECT_EQ(i.gcd(j),carl::Number<cln::cl_I>(1));
	EXPECT_EQ(i.lcm(j),carl::Number<cln::cl_I>(3));
	EXPECT_EQ(j.pow(2),carl::Number<cln::cl_I>(9));
	
	EXPECT_EQ(i.mod(j),i);	
	EXPECT_EQ(i.remainder(j),i);	
	EXPECT_EQ(i.quotient(j),carl::Number<cln::cl_I>(0));
	EXPECT_EQ(i/j,carl::Number<cln::cl_I>(0));
	EXPECT_EQ(i.div(i),i);

	EXPECT_EQ(i.toInt<carl::sint>(),1);
	EXPECT_EQ(i.toInt<carl::uint>(),1);

	EXPECT_EQ(i.toString(),"1");
#endif
}


TEST(Number,RationalOperations) {

//mpq
	carl::Number<mpq_class> q(1);

	EXPECT_FALSE(q.isZero());
	EXPECT_TRUE(q.isOne());
	EXPECT_TRUE(q.isPositive());
	EXPECT_FALSE(q.isNegative());
	EXPECT_TRUE(q.isInteger());
	EXPECT_EQ(q.bitsize(),2);
	EXPECT_EQ(q.toDouble(),1);
	EXPECT_EQ(q.getNum(),carl::Number<mpz_class>(1));
	EXPECT_EQ(q.getDenom(),carl::Number<mpz_class>(1));
	EXPECT_EQ(q.abs(),carl::Number<mpq_class>(1)); 


	EXPECT_EQ(q.round(),carl::Number<mpz_class>(1));
	EXPECT_EQ(q.floor(),carl::Number<mpz_class>(1));
	EXPECT_EQ(q.ceil(),carl::Number<mpz_class>(1));
	
	carl::Number<mpq_class> p(3);

	EXPECT_EQ(q.gcd(p),carl::Number<mpq_class>(1));
	EXPECT_EQ(q.lcm(p),carl::Number<mpq_class>(3));
	EXPECT_EQ(p.pow(2),carl::Number<mpq_class>(9));
	EXPECT_EQ(q.log(),carl::Number<mpq_class>(0));
	EXPECT_EQ(q.sin(),carl::Number<mpq_class>(std::sin(1)));
	EXPECT_EQ(q.cos(),carl::Number<mpq_class>(std::cos(1)));
	carl::Number<mpq_class> result;
	q.sqrt_exact(result);
	EXPECT_EQ(result,q);
	EXPECT_EQ(q.sqrt(),q);
	auto pair = q.sqrt_safe();
	EXPECT_EQ(pair.first,q);
	EXPECT_EQ(pair.second,q);
	pair = q.sqrt_fast();
	EXPECT_EQ(pair.first,q);
	EXPECT_EQ(pair.second,q);
	
	EXPECT_EQ(q.quotient(p),carl::Number<mpq_class>(1,3));
	EXPECT_EQ(q.div(p),carl::Number<mpq_class>(1,3));
	EXPECT_EQ(q/p,carl::Number<mpq_class>(1,3));
	EXPECT_EQ(q.reciprocal(),q);

	EXPECT_EQ(q.toInt<carl::Number<mpz_class>>(),carl::Number<mpz_class>(1));
	EXPECT_EQ(q.toInt<carl::sint>(),1);
	EXPECT_EQ(q.toInt<carl::uint>(),1);

	EXPECT_EQ(q.toString(),"1");

// clRA:
#ifdef USE_CLN_NUMBERS
	carl::Number<cln::cl_RA> r(1);

	EXPECT_TRUE(r.isOne());
	EXPECT_TRUE(r.isPositive());
	EXPECT_FALSE(r.isNegative());
	EXPECT_TRUE(r.isInteger());
	EXPECT_EQ(r.bitsize(),2);
	EXPECT_EQ(r.toDouble(),1);
	EXPECT_EQ(r.getNum(),carl::Number<cln::cl_I>(1));
	EXPECT_EQ(r.getDenom(),carl::Number<cln::cl_I>(1));
	EXPECT_EQ(r.abs(),carl::Number<cln::cl_RA>(1)); 


	EXPECT_EQ(r.round(),carl::Number<cln::cl_I>(1));
	EXPECT_EQ(r.floor(),carl::Number<cln::cl_I>(1));
	EXPECT_EQ(r.ceil(),carl::Number<cln::cl_I>(1));
	
	carl::Number<cln::cl_RA> s(3);

	EXPECT_EQ(r.gcd(s),carl::Number<cln::cl_RA>(1));
	EXPECT_EQ(r.lcm(s),carl::Number<cln::cl_RA>(3));
	EXPECT_EQ(s.pow(2),carl::Number<cln::cl_RA>(9));
	EXPECT_EQ(r.log(),carl::Number<cln::cl_RA>(0));
	EXPECT_EQ(r.sin(),carl::Number<cln::cl_RA>(std::sin(1)));
	EXPECT_EQ(r.cos(),carl::Number<cln::cl_RA>(std::cos(1)));

	carl::Number<cln::cl_RA> result2;
	r.sqrt_exact(result2);
	EXPECT_EQ(result2,r);
	EXPECT_EQ(r.sqrt(),r);
	auto pair2 = r.sqrt_safe();
	EXPECT_EQ(pair2.first,r);
	EXPECT_EQ(pair2.second,r);
	pair2 = r.sqrt_fast();
	EXPECT_EQ(pair2.first,r);
	EXPECT_EQ(pair2.second,r);
	
	std::cout << "quotient r/s: " << r.quotient(s) << "\n";
	std::cout << "constructing Number(1,3): " << carl::Number<cln::cl_RA>(1,3) << "\n";
	EXPECT_EQ(r.quotient(s),carl::Number<cln::cl_RA>(1,3));
	EXPECT_EQ(r.div(s),carl::Number<cln::cl_RA>(1,3));
	//EXPECT_EQ(r/s,carl::Number<cln::cl_RA>(1,3)); not yet implemented
	EXPECT_EQ(r.reciprocal(),r);

	EXPECT_EQ(r.toInt<carl::Number<cln::cl_I>>(),carl::Number<cln::cl_I>(1));
	EXPECT_EQ(r.toInt<carl::sint>(),1);
	EXPECT_EQ(r.toInt<carl::uint>(),1);

	EXPECT_EQ(r.toString(),"1");
#endif
}


TEST(Number,BinaryOperators) {

//mpz
	carl::Number<mpz_class> z1(1), z2(2);
	EXPECT_TRUE(z1 != z2);
	EXPECT_FALSE(z1 == z2);
	EXPECT_TRUE(z1 < z2);
	EXPECT_TRUE(z1 <= z2);
	EXPECT_FALSE(z1 > z2);
	EXPECT_FALSE(z1 >= z2);

	EXPECT_EQ(z1+z2, carl::Number<mpz_class>(3));
	EXPECT_EQ(z1-z2, carl::Number<mpz_class>(-1));
	EXPECT_EQ(z1*z2, carl::Number<mpz_class>(2));
	EXPECT_EQ(z1/z2, carl::Number<mpz_class>(0));
	

//mpq
	carl::Number<mpq_class> q1(1), q2(2);
	EXPECT_TRUE(q1 != q2);
	EXPECT_FALSE(q1 == q2);
	EXPECT_TRUE(q1 < q2);
	EXPECT_TRUE(q1 <= q2);
	EXPECT_FALSE(q1 > q2);
	EXPECT_FALSE(q1 >= q2);

	EXPECT_EQ(q1+q2, carl::Number<mpq_class>(3));
	EXPECT_EQ(q1-q2, carl::Number<mpq_class>(-1));
	EXPECT_EQ(q1*q2, carl::Number<mpq_class>(2));
	EXPECT_EQ(q1/q2, carl::Number<mpq_class>(0.5));

//clI
	carl::Number<cln::cl_I> i1(1), i2(2);
	EXPECT_TRUE(i1 != i2);
	EXPECT_FALSE(i1 == i2);
	EXPECT_TRUE(i1 < i2);
	EXPECT_TRUE(i1 <= i2);
	EXPECT_FALSE(i1 > i2);
	EXPECT_FALSE(i1 >= i2);

	EXPECT_EQ(i1+i2, carl::Number<cln::cl_I>(3));
	EXPECT_EQ(i1-i2, carl::Number<cln::cl_I>(-1));
	EXPECT_EQ(i1*i2, carl::Number<cln::cl_I>(2));
	EXPECT_EQ(i1/i2, carl::Number<cln::cl_I>(0));

//clRA
	carl::Number<cln::cl_RA> r1(1), r2(2);

	EXPECT_TRUE(r1 != r2);
	EXPECT_FALSE(r1 == r2);
	EXPECT_TRUE(r1 < r2);
	EXPECT_TRUE(r1 <= r2);
	EXPECT_FALSE(r1 > r2);
	EXPECT_FALSE(r1 >= r2);

	EXPECT_EQ(r1+r2, carl::Number<cln::cl_RA>(3));
	EXPECT_EQ(r1-r2, carl::Number<cln::cl_RA>(-1));
	EXPECT_EQ(r1*r2, carl::Number<cln::cl_RA>(2));
	EXPECT_EQ(r1/r2, carl::Number<cln::cl_RA>(0.5));


}

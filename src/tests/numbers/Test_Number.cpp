#include "gtest/gtest.h"
#include "../../carl/numbers/numbers.h"

#include "../../carl/numbers/NumberMpz.h"
#include "../../carl/numbers/NumberMpq.h"
#include "../../carl/numbers/NumberClI.h"
#include "../../carl/numbers/NumberClRA.h"

TEST(Number, Constructor)
{

/*
	Number(): BaseNumber() {}
	explicit Number(const mpz_class& t): BaseNumber(t) {}
	explicit Number(mpz_class&& t): BaseNumber(t) {}  //how to test this?
	Number(const Number<mpz_class>& n): BaseNumber(n) {}
	Number(Number<mpz_class>&& n) noexcept : BaseNumber(n) {}
	Number(long long int n) : BaseNumber(n) {}
	Number(unsigned long long int n): BaseNumber(n) {}
	Number(const std::string& s) : BaseNumber(s) {}

*/
//Mpz class
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
	EXPECT_EQ(q11.getValue(),mpq_class(-0.1));
	EXPECT_EQ(q12.getValue(),mpq_class(3));
	EXPECT_EQ(q13.getValue(),mpq_class(2));
	EXPECT_EQ(q14.getValue(),mpq_class(0));


//cln::cl_I


	cln::cl_I clI(2);	
	carl::Number<cln::cl_I> i1, i2, i3, i4, i5, i6, i7, i8, i9, i10;
	ASSERT_NO_THROW(i1 = carl::Number<cln::cl_I>());
	ASSERT_NO_THROW(i2 = carl::Number<cln::cl_I>(clI));
	ASSERT_NO_THROW(i3 = carl::Number<cln::cl_I>(std::move(clI)));
	ASSERT_NO_THROW(i4 = carl::Number<cln::cl_I>(z1));
	ASSERT_NO_THROW(i5 = carl::Number<cln::cl_I>(3ll));
	ASSERT_NO_THROW(i6 = carl::Number<cln::cl_I>(4ull));
	ASSERT_NO_THROW(i7 = carl::Number<cln::cl_I>("-30"));
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
	EXPECT_EQ(i9.getValue(),cln::cl_I(3));
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
	ASSERT_NO_THROW(r7 = carl::Number<cln::cl_RA>("-30"));
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
	EXPECT_EQ(r4.getValue(),cln::cl_RA(0));
	EXPECT_EQ(r5.getValue(),cln::cl_RA(3));
	EXPECT_EQ(r6.getValue(),cln::cl_RA(4));
	EXPECT_EQ(r7.getValue(),cln::cl_RA(-30));
	EXPECT_EQ(r8.getValue(),cln::cl_RA(3));

	EXPECT_EQ(r9.getValue(),"3.5");
	EXPECT_EQ(r10.getValue(),"3.5");
	EXPECT_EQ(r11.getValue(),"-0.1");
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
	

	
}

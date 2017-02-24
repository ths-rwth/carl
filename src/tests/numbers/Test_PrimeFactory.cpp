#include "gtest/gtest.h"
#include <carl/numbers/PrimeFactory.h>
#include <carl/numbers/numbers.h>

using namespace carl;

TEST(PrimeFactory, integers)
{
    PrimeFactory<carl::uint> primefact;
    EXPECT_EQ(carl::uint(2),primefact.nextPrime());
    EXPECT_EQ(carl::uint(3),primefact.nextPrime());
    EXPECT_EQ(carl::uint(5),primefact.nextPrime());
	EXPECT_EQ(carl::uint(7),primefact.nextPrime());
	EXPECT_EQ(carl::uint(11),primefact.nextPrime());
	EXPECT_EQ(carl::uint(13),primefact.nextPrime());
	EXPECT_EQ(carl::uint(17),primefact.nextPrime());
	
	PrimeFactory<mpz_class> p2;
    EXPECT_EQ(mpz_class(2),p2.nextPrime());
    EXPECT_EQ(mpz_class(3),p2.nextPrime());
    EXPECT_EQ(mpz_class(5),p2.nextPrime());
    EXPECT_EQ(mpz_class(7),p2.nextPrime());
    EXPECT_EQ(mpz_class(11),p2.nextPrime());
    EXPECT_EQ(mpz_class(13),p2.nextPrime());
    EXPECT_EQ(mpz_class(17),p2.nextPrime());
	
	{
		PrimeFactory<cln::cl_I> p2;
	    EXPECT_EQ(cln::cl_I(2),p2.nextPrime());
	    EXPECT_EQ(cln::cl_I(3),p2.nextPrime());
	    EXPECT_EQ(cln::cl_I(5),p2.nextPrime());
	    EXPECT_EQ(cln::cl_I(7),p2.nextPrime());
	    EXPECT_EQ(cln::cl_I(11),p2.nextPrime());
	    EXPECT_EQ(cln::cl_I(13),p2.nextPrime());
	    EXPECT_EQ(cln::cl_I(17),p2.nextPrime());
	}
}

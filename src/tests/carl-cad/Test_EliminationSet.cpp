#include "gtest/gtest.h"

#include <memory>
#include <list>

#include <carl/core/UnivariatePolynomial.h>
#include <carl/util/platform.h>
#include <carl-cad/EliminationSet.h>

#include "../Common.h"

using namespace carl;

TEST(EliminationSet, BasicOperations)
{
	///@todo reformulate test case such that the polynomials are all consistent with x as main variable.
	/*
	VariablePool& vpool = VariablePool::getInstance();
	Variable x = vpool.getFreshVariable();
	Variable y = vpool.getFreshVariable();
	Variable z = vpool.getFreshVariable();
	cad::PolynomialOwner<Rational> owner;

	cad::EliminationSet<Rational> s(&owner);

	cad::MPolynomial<Rational> mpz({z});
	cad::MPolynomial<Rational> mpxz2({(Rational)-1*x*z});
	cad::MPolynomial<Rational> mpxz({(Rational)-1*x*z, Term<Rational>(2)});
	cad::MPolynomial<Rational> mpzero;
	cad::MPolynomial<Rational> mpone({1});

	cad::UPolynomial<Rational>* one = new cad::UPolynomial<Rational>(x, mpone);
	cad::UPolynomial<Rational>* p = new cad::UPolynomial<Rational>(x, {(Rational)-2*mpone,mpzero,mpone});
	cad::UPolynomial<Rational>* q = new cad::UPolynomial<Rational>(x, (Rational)-3*mpone + mpone*y*y*y);
	cad::UPolynomial<Rational>* r = new cad::UPolynomial<Rational>(x, mpz + mpxz2*y*y);
	cad::UPolynomial<Rational>* w = new cad::UPolynomial<Rational>(x, mpz + mpxz*y*y);

	s.insert(p);
	s.insert(q, {one, p});
	s.insert(r, {w, nullptr, p, q});
	s.insert(w, {p, q});

	ASSERT_EQ((size_t)1, s.erase(p));
	ASSERT_EQ((size_t)0, s.erase(p));
	s.removeByParent(p); // delete q and w
	ASSERT_EQ((size_t)0, s.erase(q));
	ASSERT_EQ((size_t)0, s.erase(w));
	ASSERT_EQ((size_t)1, s.erase(r));

	delete one;
	delete p;
	delete q;
	delete r;
	delete w;*/
}

TEST(EliminationSet, SetProperty)
{
	Variable x = freshRealVariable("x");
	cad::PolynomialOwner<Rational> owner;
	cad::EliminationSet<Rational> s(&owner);

	cad::MPolynomial<Rational> mpone(1);

	for (unsigned int i = 0; i < 10; i++) {
		cad::UPolynomial<Rational>* p = new cad::UPolynomial<Rational>(x, {mpone, mpone, mpone});
		s.insert(p);
	}
	EXPECT_EQ((unsigned)1, s.size());
}

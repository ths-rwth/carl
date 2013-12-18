#include "gtest/gtest.h"

#include <memory>
#include <list>

#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/cad/EliminationSet.h"

using namespace carl;

TEST(EliminationSet, BasicOperations)
{  
	VariablePool& vpool = VariablePool::getInstance();
	Variable x = vpool.getFreshVariable();
	Variable y = vpool.getFreshVariable();
	Variable z = vpool.getFreshVariable();

	cad::EliminationSet<MultivariatePolynomial<cln::cl_RA>> s;
	
	MultivariatePolynomial<cln::cl_RA> mpz({z});
	MultivariatePolynomial<cln::cl_RA> mpxz2({(cln::cl_RA)-1*x*z});
	MultivariatePolynomial<cln::cl_RA> mpxz({(cln::cl_RA)-1*x*z, Term<cln::cl_RA>(2)});
	MultivariatePolynomial<cln::cl_RA> mpzero;
	MultivariatePolynomial<cln::cl_RA> mpone({1});

	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>* one = new UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>(x, mpone);
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>* p = new UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>(x, {(cln::cl_RA)-2*mpone,mpzero,mpone});
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>* q = new UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>(y, {(cln::cl_RA)-3*mpone,mpzero,mpzero,mpone});
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>* r = new UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>(y, {mpz, mpzero, mpxz2});
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>* w = new UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>>(y, {mpz, mpzero, mpxz});
	
	s.insert(p);
	s.insert(q, {one, p});
	s.insert(r, {w, nullptr, p, q});
	s.insert(w, {p, q});
	
	ASSERT_EQ(s.erase(p), (size_t)1);
	ASSERT_EQ(s.erase(p), (size_t)0);
	s.removeByParent(p); // delete q and w
	ASSERT_EQ(s.erase(q), (size_t)0);
	ASSERT_EQ(s.erase(w), (size_t)0);
	ASSERT_EQ(s.erase(r), (size_t)1);
	
	delete one;
	delete p;
	delete q;
	delete r;
	delete w;
}

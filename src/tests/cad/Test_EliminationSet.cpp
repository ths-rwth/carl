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
	cad::PolynomialOwner<cln::cl_RA> owner;

	cad::EliminationSet<cln::cl_RA> s(&owner);
	
	cad::MPolynomial<cln::cl_RA> mpz({z});
	cad::MPolynomial<cln::cl_RA> mpxz2({(cln::cl_RA)-1*x*z});
	cad::MPolynomial<cln::cl_RA> mpxz({(cln::cl_RA)-1*x*z, Term<cln::cl_RA>(2)});
	cad::MPolynomial<cln::cl_RA> mpzero;
	cad::MPolynomial<cln::cl_RA> mpone({1});

	cad::UPolynomial<cln::cl_RA>* one = new cad::UPolynomial<cln::cl_RA>(x, mpone);
	cad::UPolynomial<cln::cl_RA>* p = new cad::UPolynomial<cln::cl_RA>(x, {(cln::cl_RA)-2*mpone,mpzero,mpone});
	cad::UPolynomial<cln::cl_RA>* q = new cad::UPolynomial<cln::cl_RA>(y, {(cln::cl_RA)-3*mpone,mpzero,mpzero,mpone});
	cad::UPolynomial<cln::cl_RA>* r = new cad::UPolynomial<cln::cl_RA>(y, {mpz, mpzero, mpxz2});
	cad::UPolynomial<cln::cl_RA>* w = new cad::UPolynomial<cln::cl_RA>(y, {mpz, mpzero, mpxz});
	
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
	delete w;
}

#include "gtest/gtest.h"

#include <cln/cln.h>
#include "carl/core/rootfinder/RootFinder.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/RealAlgebraicNumber.h"

using namespace carl;

typedef carl::UnivariatePolynomial<cln::cl_RA> UPolynomial;
typedef carl::MultivariatePolynomial<cln::cl_RA> MPolynomial;
typedef carl::UnivariatePolynomial<MPolynomial> UMPolynomial;

template<typename Number>
bool represents(const carl::RealAlgebraicNumber<Number>* root, const Number& exact) {
	if (root->isNumeric()) {
		return root->value() == exact;
	} else {
		const carl::RealAlgebraicNumberIR<Number>* interval = static_cast<const carl::RealAlgebraicNumberIR<Number>*>(root);
		return interval->getInterval().contains(exact);
	}
}

TEST(RootFinder, realRoots)
{
	VariablePool& vpool = VariablePool::getInstance();
	Variable x = vpool.getFreshVariable();
	Variable y = vpool.getFreshVariable();

	{
		UPolynomial p(x, {(cln::cl_RA)-1, (cln::cl_RA)0, (cln::cl_RA)0, (cln::cl_RA)1});
		auto roots = carl::rootfinder::realRoots(p);
		ASSERT_TRUE(roots.size() == 1);
		ASSERT_TRUE(represents(roots.front(), (cln::cl_RA)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::rootfinder::realRoots(p);
		ASSERT_TRUE(roots.size() == 1);
		ASSERT_TRUE(represents(roots.front(), (cln::cl_RA)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::rootfinder::realRoots(p);
		ASSERT_TRUE(roots.size() == 2);
		ASSERT_TRUE(represents(roots.front(), (cln::cl_RA)-1));
		ASSERT_TRUE(represents(roots.back(), (cln::cl_RA)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(y), MPolynomial(0), MPolynomial(1)});
		std::map<Variable, RealAlgebraicNumber<cln::cl_RA>*> m;
		m[y] = new RealAlgebraicNumberNR<cln::cl_RA>(-1);
		auto roots = carl::rootfinder::realRoots(p, m);
		ASSERT_TRUE(roots.size() == 2);
		ASSERT_TRUE(represents(roots.front(), (cln::cl_RA)-1));
		ASSERT_TRUE(represents(roots.back(), (cln::cl_RA)1));
	}
}

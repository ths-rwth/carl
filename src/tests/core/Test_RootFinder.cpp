#include "gtest/gtest.h"

#include <cln/cln.h>
#include "carl/core/rootfinder/RootFinder.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/RealAlgebraicNumber.h"

using namespace carl;

template<typename Number>
bool represents(const carl::RealAlgebraicNumber<Number>* root, const Number& exact) {
	if (root->isNumeric()) {
		return root->value() == exact;
	} else {
		const carl::RealAlgebraicNumberIR<Number>* interval = static_cast<const carl::RealAlgebraicNumberIR<Number>*>(root);
		return interval->getInterval().contains(exact);
	}
}

TEST(IncrementalRootFinder, Constructor)
{
	VariablePool& vpool = VariablePool::getInstance();
	Variable x = vpool.getFreshVariable();
	carl::UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)1, (cln::cl_RA)0, (cln::cl_RA)0, (cln::cl_RA)-1});
	
    carl::rootfinder::IncrementalRootFinder<cln::cl_RA> finder(p);
	std::list<carl::RealAlgebraicNumber<cln::cl_RA>*> roots = finder.getAllRoots();
	//ASSERT_TRUE(roots.size() == 1 && represents(roots.front(), (cln::cl_RA)1));
}

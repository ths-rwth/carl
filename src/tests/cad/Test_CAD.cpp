#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <cln/cln.h>

#include "carl/core/logging.h"
#include "carl/cad/CAD.h"

using namespace carl;

TEST(CAD, BasicOperations)
{
	typedef carl::CAD<cln::cl_RA>::MPolynomial Polynomial;
	carl::CAD<cln::cl_RA> cad;

	carl::Variable v1 = carl::VariablePool::getInstance().getFreshVariable();
	carl::Variable v2 = carl::VariablePool::getInstance().getFreshVariable();

	Polynomial* p1 = new Polynomial({v1, v2});

	cad.addPolynomial(p1, {v1, v2});
	cad.prepareElimination();
	cad.complete();
	cad.samples();
}

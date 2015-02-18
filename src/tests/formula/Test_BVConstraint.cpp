#include "gtest/gtest.h"
#include "../../carl/formula/bitvector/BVTermPool.h"
#include "../../carl/formula/bitvector/BVConstraintPool.h"
#include "../../carl/core/MultivariatePolynomial.h"
#include "../../carl/core/VariablePool.h"
#include "../../carl/formula/FormulaPool.h"
// #include "../../carl/formula/Formula.h"
// #include "../../carl/util/stringparser.h"

#include <cln/cln.h>


using namespace carl;

typedef cln::cl_RA Rational;
typedef MultivariatePolynomial<Rational> Pol;

TEST(BVConstraint, Construction)
{
	Variable a = VariablePool::getInstance().getFreshVariable("a", VariableType::VT_BITVECTOR);
	Variable b = VariablePool::getInstance().getFreshVariable("b", VariableType::VT_BITVECTOR);

	BVTerm<Pol> a_t(BVTermType::VARIABLE, a, 16);
	BVTerm<Pol> b_t(BVTermType::VARIABLE, b, 16);

	BVTerm<Pol> check_for_default_constructor;

	BVTerm<Pol> oxfff0(BVTermType::CONSTANT, BVValue(16, 65520));
	const Formula<Pol> trueFormula = Formula<Pol>(FormulaType::TRUE);

	BVTerm<Pol> ite(BVTermType::ITE, trueFormula, a_t, b_t);

	BVTerm<Pol> bvand(BVTermType::AND, a_t, oxfff0);
	BVTerm<Pol> bvmul(BVTermType::MUL, bvand, b_t);

	BVTerm<Pol> bvnot(BVTermType::NOT, bvmul);

	BVTerm<Pol> oxa(BVTermType::CONSTANT, BVValue(4, 10));
	BVTerm<Pol> oxaa(BVTermType::REPEAT, oxa, 2);

	BVTerm<Pol> bvextract(BVTermType::EXTRACT, bvnot, 6, 8);
	BVTerm<Pol> bvconcat(BVTermType::CONCAT, bvextract, bvextract);

	BVTerm<Pol> bvzeroext(BVTermType::EXT_U, bvconcat, 8);

	// check for copy assignment
	check_for_default_constructor = bvzeroext;

	BVConstraint<Pol> constraint = BVConstraint<Pol>::create(BVCompareRelation::SLT, oxaa, bvzeroext);
	BVConstraint<Pol> inconsistentConstraint = BVConstraint<Pol>::create(false);

	std::cout << constraint.toString(false, 0, "", false, false, true) << std::endl;

	BV_TERM_POOL.print();
	BV_CONSTRAINT_POOL.print();
}

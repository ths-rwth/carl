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
	Variable a = VariablePool::getInstance().getFreshVariable("a", VariableType::VT_REAL); // TODO: Should be VT_BITVECTOR
	Variable b = VariablePool::getInstance().getFreshVariable("b", VariableType::VT_REAL); // TODO: Should be VT_BITVECTOR

	auto a_t = BV_TERM_POOL.create(BVTermType::VARIABLE, a, 16);
	auto b_t = BV_TERM_POOL.create(BVTermType::VARIABLE, b, 16);

	auto foo = BV_TERM_POOL.create(BVTermType::CONSTANT, BVValue(16, 65520));

	auto oxfff0 = BV_TERM_POOL.create(BVTermType::CONSTANT, BVValue(16, 65520));
	const Formula<Pol> trueFormula = Formula<Pol>(FormulaType::TRUE);

	auto ite = BV_TERM_POOL.create(BVTermType::ITE, trueFormula, *a_t, *b_t);

	auto bvand = BV_TERM_POOL.create(BVTermType::AND, *a_t, *oxfff0);
	auto bvmul = BV_TERM_POOL.create(BVTermType::MUL, *bvand, *b_t);

	auto bvnot = BV_TERM_POOL.create(BVTermType::NOT, *bvmul);

	auto oxa = BV_TERM_POOL.create(BVTermType::CONSTANT, BVValue(4, 10));
	auto oxaa = BV_TERM_POOL.create(BVTermType::REPEAT, *oxa, 2);

	auto bvextract = BV_TERM_POOL.create(BVTermType::EXTRACT, *bvnot, 6, 8);
	auto bvconcat = BV_TERM_POOL.create(BVTermType::CONCAT, *bvextract, *bvextract);

	auto bvzeroext = BV_TERM_POOL.create(BVTermType::EXT_U, *bvconcat, 8);

	auto constraint = BV_CONSTRAINT_POOL.create(BVCompareRelation::SLT, *oxaa, *bvzeroext);

	std::cout << constraint->toString(false, 0, "", false, false, true) << std::endl;

	BV_TERM_POOL.print();
	BV_CONSTRAINT_POOL.print();
}

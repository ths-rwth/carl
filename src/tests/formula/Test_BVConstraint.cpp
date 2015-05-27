#include "gtest/gtest.h"
#include "../../carl/formula/bitvector/BVTermPool.h"
#include "../../carl/formula/bitvector/BVConstraintPool.h"
#include "../../carl/core/MultivariatePolynomial.h"
#include "../../carl/core/VariablePool.h"
#include "../../carl/formula/FormulaPool.h"
#include "../../carl/formula/SortManager.h"
#include "../../carl/numbers/config.h"
#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
#endif

using namespace carl;

TEST(BVConstraint, Construction)
{
    Variable a = VariablePool::getInstance().getFreshVariable("a", VariableType::VT_BITVECTOR);
    Variable b = VariablePool::getInstance().getFreshVariable("b", VariableType::VT_BITVECTOR);

    Sort bvSort = SortManager::getInstance().addSort("BitVec");
    SortManager::getInstance().makeSortIndexable(bvSort, 1, VariableType::VT_BITVECTOR);
    Sort bv16Sort = SortManager::getInstance().index(bvSort, {16});

    BVTerm a_t(BVTermType::VARIABLE, BVVariable(a, bv16Sort));
    BVTerm b_t(BVTermType::VARIABLE, BVVariable(b, bv16Sort));

    BVTerm check_for_default_constructor;

    BVTerm oxfff0(BVTermType::CONSTANT, BVValue(16, 65520));

    BVTerm bvand(BVTermType::AND, a_t, oxfff0);
    BVTerm bvmul(BVTermType::MUL, bvand, b_t);

    BVTerm bvnot(BVTermType::NOT, bvmul);

    BVTerm oxa(BVTermType::CONSTANT, BVValue(4, 10));
    BVTerm oxaa(BVTermType::REPEAT, oxa, 2);

    BVTerm bvextract(BVTermType::EXTRACT, bvnot, 8, 6);
    BVTerm bvconcat(BVTermType::CONCAT, bvextract, bvextract);

    BVTerm bvzeroext(BVTermType::EXT_U, bvconcat, 2);

    BVTerm bvzeroext2(BVTermType::EXT_U, bvconcat, 2);

    std::cout << "equal? " << (bvzeroext == bvzeroext2) << std::endl;

    // check for copy assignment
    check_for_default_constructor = bvzeroext;

    BVConstraint constraint = BVConstraint::create(BVCompareRelation::SLT, oxaa, bvzeroext);
    BVConstraint inconsistentConstraint = BVConstraint::create(false);

    std::cout << constraint.toString("", false, false, true) << std::endl;

#ifdef USE_CLN_NUMBERS
    // Test BVValue construction from CLN / GMP objects
    // (every 8-bit value)
    for(int i=-256;i<=256;++i) {
        char i_str[5];
        snprintf(i_str, 5, "%d", i);

        cln::cl_I clnNumber(i_str);
        mpz_class gmpNumber(i_str);

        BVValue clnVal(8, clnNumber);
        BVValue gmpVal(8, gmpNumber);
        assert(clnVal == gmpVal);
        assert(clnVal().to_ulong() == ((i + 256) % 256));
    }

    // Test BVValue construction from CLN / GMP objects
    // for big numbers
    mpz_class hugeNumber("-123019238129310293810293810293810928301928301298309182301928313428734982734");
    cln::cl_I hugeClnNumber("-123019238129310293810293810293810928301928301298309182301928313428734982734");

    BVValue hugeVal(512, hugeNumber);
    BVValue hugeClnVal(512, hugeClnNumber);
    assert(hugeVal == hugeClnVal);
#endif
    BV_TERM_POOL.print();
    BV_CONSTRAINT_POOL.print();
}

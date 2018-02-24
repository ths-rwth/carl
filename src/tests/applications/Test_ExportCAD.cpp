#include <gtest/gtest.h>

#include <carl/core/VariablePool.h>
#include <carl/formula/Formula.h>
#include <carl/io/MapleStream.h>
#include <carl/io/QEPCADStream.h>
#include <carl/io/SMTLIBStream.h>

#include "../Common.h"

using Poly = carl::MultivariatePolynomial<Rational>;
using FormulaT = carl::Formula<carl::MultivariatePolynomial<Rational>>;

struct QEPCADExample: ::testing::Test {
	// Taken from "Improved Projection for Cylindrical Algebraic Decomposition", Figure 1
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");
	Poly p1 = Poly(x)*x + Poly(y)*y - Poly(4);
	Poly p2 = Poly(x)*y - Poly(1);
	FormulaT f1 = FormulaT(p1, carl::Relation::LESS);
	FormulaT f2 = FormulaT(p2, carl::Relation::GREATER);
	FormulaT f = FormulaT(carl::FormulaType::AND, {f1, f2});
};

TEST_F(QEPCADExample, Maple) {
	carl::MapleStream s;
	s << "restart:" << std::endl;
	s << "start := time():" << std::endl;
	s << "F := [" << this->p1 << ", " << this->p2 << "]:" << std::endl;
	s << "L := [" << this->x << ", " << this->y << "]:" << std::endl;
	s << "R := RegularChains:-PolynomialRing(L):" << std::endl;
	s << "C := RegularChains:-SemiAlgebraicSetTools:-CylindricalAlgebraicDecompose(F, R, 'output' = 'rootof'):" << std::endl;
	s << "seconds := time() - start;" << std::endl;
	s << "Cells := numelems(C);" << std::endl;

	const auto filename = "QEPCADExample.mpl";
	std::cout << "Exporting to " << filename << std::endl;
	std::ofstream out(filename);
	out << s;
	out.close();
}

TEST_F(QEPCADExample, SMTLIB) {
	const auto filename = "QEPCADExample.smt2";
	std::cout << "Exporting to " << filename << std::endl;
	std::ofstream out(filename);
	out << carl::outputSMTLIB(carl::Logic::QF_NRA, {this->f});
	out.close();
}

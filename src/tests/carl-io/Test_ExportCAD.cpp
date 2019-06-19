#include <gtest/gtest.h>

#include <carl/core/VariablePool.h>
#include <carl/formula/Formula.h>
#include <carl-io/MapleStream.h>
#include <carl-io/QEPCADStream.h>
#include <carl-io/SMTLIBStream.h>

#include "../Common.h"

using Poly = carl::MultivariatePolynomial<Rational>;
using FormulaT = carl::Formula<carl::MultivariatePolynomial<Rational>>;

struct QEPCADExample: ::testing::Test {
	// Taken from "Improved Projection for Cylindrical Algebraic Decomposition", Figure 1
	carl::Variable y = carl::freshRealVariable("y");
	carl::Variable x = carl::freshRealVariable("x");
	Poly p1 = Poly(x)*x + Poly(y)*y - Poly(4);
	Poly p2 = Poly(x)*y - Poly(1);
	Poly p3 = Poly(x)*x + Poly(y) - Poly(3);
	FormulaT f1 = FormulaT(p1, carl::Relation::LESS);
	FormulaT f2 = FormulaT(p2, carl::Relation::GREATER);
	FormulaT f3 = FormulaT(p3, carl::Relation::LEQ);
	FormulaT f_1 = FormulaT(carl::FormulaType::AND, {f1, f2});
	FormulaT f_2 = FormulaT(carl::FormulaType::AND, {f1, f2, f3});
	
	bool extended = true;
};

TEST_F(QEPCADExample, Maple) {
	carl::MapleStream s;
	s << "restart:" << std::endl;
	s << "start := time():" << std::endl;
	if (extended) {
		s << "F := [" << this->p1 << ", " << this->p2 << ", " << this->p3 << "]:" << std::endl;
	} else {
		s << "F := [" << this->p1 << ", " << this->p2 << "]:" << std::endl;
	}
	s << "L := [" << this->y << ", " << this->x << "]:" << std::endl;
	s << "R := RegularChains:-PolynomialRing(L):" << std::endl;
	s << "C := RegularChains:-SemiAlgebraicSetTools:-CylindricalAlgebraicDecompose(F, R, 'output' = 'rootof'):" << std::endl;
	s << "seconds := time() - start;" << std::endl;
	s << "Cells := numelems(C);" << std::endl;
	if (extended) {
		s << "F := [[" << this->f1 << ", " << this->f2 << ", " << this->f3 << "]]:" << std::endl;
	} else {
		s << "F := [[" << this->f1 << ", " << this->f2 << "]]:" << std::endl;
	}
	s << "C := RegularChains:-SemiAlgebraicSetTools:-CylindricalAlgebraicDecompose(F, R, 'output' = 'rootof'):" << std::endl;
	s << "C[1];" << std::endl;
	s << "C[2];" << std::endl;

	std::ofstream out("QEPCADExample.mpl");
	out << s;
	out.close();
}

TEST_F(QEPCADExample, QEPCAD) {
	carl::QEPCADStream s;
	// informal description
	s << "[Example]" << std::endl;
	// variable list
	s << "(" << this->x << "," << this->y << ")" << std::endl;
	// number of free variables
	s << "2" << std::endl;
	// prenex formula
	if (extended) {
		s << "[ " << this->f_2 << " ] ." << std::endl;
	} else {
		s << "[ " << this->f_1 << " ] ." << std::endl;
	}
	// normalization
	s << "go" << std::endl;
	// projection
	s << "full-cad()." << std::endl;
	s << "go" << std::endl;
	// choice
	s << "go" << std::endl;
	// solution
	s << "d-stat()." << std::endl;
	s << "d-true-cells()." << std::endl;
	s << "go" << std::endl;
	
	std::ofstream out("QEPCADExample.qep");
	out << s;
	out.close();
}

TEST_F(QEPCADExample, SMTLIB) {
	std::ofstream out("QEPCADExample.smt2");
	if (extended) {
		out << carl::outputSMTLIB(carl::Logic::QF_NRA, {this->f_2});
	} else {
		out << carl::outputSMTLIB(carl::Logic::QF_NRA, {this->f_1});
	}
	out.close();
}

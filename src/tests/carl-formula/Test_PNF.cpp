
#include <functional>
#include <gtest/gtest.h>
#include <carl-formula/formula/functions/PNF.h>

#include "../Common.h"

using namespace carl;

typedef MultivariatePolynomial<Rational> Poly;
typedef Constraint<Poly> ConstraintT;
typedef Formula<Poly> FormulaT;

TEST(PNF, testNegation) {
	// Example: not (forall x. exists y. x - y > 0) -> exists x. forall y. x - y <= 0
	auto x = carl::fresh_real_variable("x");
	auto y = carl::fresh_real_variable("y");
	auto formula = FormulaT(carl::FormulaType::NOT, FormulaT(carl::FormulaType::FORALL, std::vector({x}), FormulaT(carl::FormulaType::EXISTS, std::vector({y}), FormulaT(Poly(x) - Poly(y), carl::Relation::GREATER))));
	auto prenexed = FormulaT(carl::FormulaType::EXISTS, std::vector({x}), FormulaT(carl::FormulaType::FORALL, std::vector({y}), FormulaT(Poly(x) - Poly(y), carl::Relation::LEQ)));

	auto [quantifiers, matrix] = carl::to_pnf(formula);
	auto prenex = to_formula(quantifiers, matrix);

	std::cout << "Formula: " << formula << std::endl;
	std::cout << "Prenexed: " << prenex << std::endl;

	EXPECT_EQ(prenexed, prenex);
}

TEST(PNF, testAndOr) {
	// Example: (forall x. x > 0) and (exists x. x < 0) -> (forall x. x > 0) and (exists y. y < 0) with y != x
	auto x = carl::fresh_real_variable("x");
	auto formula = FormulaT(carl::FormulaType::AND, {FormulaT(carl::FormulaType::FORALL, std::vector({x}), FormulaT(Poly(x), carl::Relation::GREATER)), FormulaT(carl::FormulaType::EXISTS, std::vector({x}), FormulaT(Poly(x), carl::Relation::LESS))});

	auto [quantifiers, matrix] = carl::to_pnf(formula);
	auto prenex = to_formula(quantifiers, matrix);

	std::cout << "Formula: " << formula << std::endl;
	std::cout << "Prenexed: " << prenex << std::endl;

	EXPECT_EQ(formula.variables().size(), 1);
	EXPECT_EQ(prenex.variables().size(), 2);
}


TEST(PNF, testImplication){
	// Example: (forall x. x - 1 > 0) => (exists x. x < 0)
	auto x = carl::fresh_real_variable("x");
	auto formula = FormulaT(carl::FormulaType::IMPLIES, {FormulaT(carl::FormulaType::FORALL, std::vector({x}), FormulaT(Poly(x)-Poly(1), carl::Relation::GREATER)), FormulaT(carl::FormulaType::EXISTS, std::vector({x}), FormulaT(Poly(x), carl::Relation::LESS))});

	auto [quantifiers, matrix] = carl::to_pnf(formula);
	auto prenex = to_formula(quantifiers, matrix);

	std::cout << "Formula: " << formula << std::endl;
	std::cout << "Prenexed: " << prenex << std::endl;

	EXPECT_EQ(formula.variables().size(), 1);
	EXPECT_EQ(prenex.variables().size(), 2);
}
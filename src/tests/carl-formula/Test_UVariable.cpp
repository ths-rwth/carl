#include <carl-formula/uninterpreted/UVariable.h>
#include <carl-formula/sort/SortManager.h>
#include <carl-formula/formula/Formula.h>
#include <carl-formula/model/Model.h>
#include <carl-formula/uninterpreted/SortValueManager.h>
#include <carl-formula/model/evaluation/ModelEvaluation.h>
#include <gtest/gtest.h>

#include "../get_output.h"

TEST(UVariable, Basics)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	sm.clear();
	carl::Sort sort = sm.addSort("S", carl::VariableType::VT_UNINTERPRETED);

	carl::Variable a = carl::fresh_uninterpreted_variable("a");
	carl::Variable b = carl::fresh_uninterpreted_variable("b");
	carl::UVariable v1;
	carl::UVariable v2(a, sort);
	carl::UVariable v3(a, sort);
	carl::UVariable v4(b, sort);

	EXPECT_EQ(v2.domain(), sort);
	EXPECT_EQ(v2, v3);
	EXPECT_TRUE(v3 < v4);
	EXPECT_EQ(v2.variable(), a);
	EXPECT_EQ(v4.variable(), b);
	EXPECT_EQ(get_output(v2), "a");
	EXPECT_EQ(get_output(v4), "b");
	EXPECT_EQ(std::hash<carl::UVariable>()(v2), std::hash<carl::UVariable>()(v3));
}

TEST(UVariable, Evaluation)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	sm.clear();
	carl::Sort sort = sm.addSort("S", carl::VariableType::VT_UNINTERPRETED);

	carl::Variable a = carl::fresh_uninterpreted_variable("a");
	carl::Variable b = carl::fresh_uninterpreted_variable("b");
	carl::Variable c = carl::fresh_uninterpreted_variable("c");
	carl::UVariable v1(a, sort);
	carl::UVariable v2(b, sort);
	carl::UVariable v3(c, sort);

	using P = carl::MultivariatePolynomial<mpq_class>;
	using F = carl::Formula<P>;

	auto f = F(carl::FormulaType::AND, {
		F(carl::UEquality(v1, v2, false)),
		F(carl::UEquality(v2, v3, true))
	});

	auto sv1 = newSortValue(sort);
	auto sv2 = newSortValue(sort);
	carl::Model<mpq_class, P> m;
	m.emplace(v1, sv1);
	m.emplace(v2, sv1);
	m.emplace(v3, sv2);

	carl::ModelValue<mpq_class, P> reference(true);
	EXPECT_EQ(carl::evaluate(f, m), reference);
}
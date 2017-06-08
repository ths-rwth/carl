
#include "gtest/gtest.h"
#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>

#include "../Common.h"

using namespace carl;

TEST(Variable, Constructor)
{
    
}

TEST(Variable, Equals)
{
    Variable v1 = carl::freshIntegerVariable("v1");
    Variable v2 = carl::freshRealVariable("v2");
    EXPECT_EQ(VariableType::VT_INT,v1.getType());
    EXPECT_EQ(VariableType::VT_REAL,v2.getType());
    EXPECT_TRUE(v1 != v2);
    EXPECT_TRUE(v1 == v1);
}

TEST(Variable, Comparison)
{
    ComparisonList<Variable> list;

    Variable v0 = carl::freshRealVariable("v0");
    Variable v1 = carl::freshRealVariable("v1");
    Variable v2 = carl::freshRealVariable("v2");

    list.push_back(v2);
    list.push_back(v1);
    list.push_back(v0);
}

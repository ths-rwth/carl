
#include "gtest/gtest.h"
#include "carl/core/Variable.h"
#include "Util.cpp"

using namespace carl;

TEST(Variable, Constructor)
{
    
}

TEST(Variable, Equals)
{
    Variable v1(1,VariableType::VT_INT);
    Variable v2(2,VariableType::VT_REAL);
    EXPECT_EQ(VariableType::VT_INT,v1.getType());
    EXPECT_EQ((unsigned)1,v1.getId());
    EXPECT_EQ(VariableType::VT_REAL,v2.getType());
    EXPECT_TRUE(v1 != v2);
    EXPECT_TRUE(v1 == v1);
}

TEST(Variable, Comparison)
{
    ComparisonList<Variable> list;

    Variable v0(321);
    Variable v1(123);
    Variable v2(132);

    list.push_back(v0);
    list.push_back(v1);
    list.push_back(v2);
}

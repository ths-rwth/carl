#include "gtest/gtest.h"

#include "carl/core/Variable.h"
#include "carl/util/parser/Parser.h"

using namespace carl;

TEST(Parser, Polynomial)
{
	carl::parser::Parser<MultivariatePolynomial<cln::cl_RA>> parser;
	carl::Variable x = freshRealVariable("x");
	carl::Variable y = freshRealVariable("y");
	parser.addVariable(x);
	parser.addVariable(y);
	
	EXPECT_EQ(cln::cl_RA(1), parser.polynomial("1"));
	EXPECT_EQ(x, parser.polynomial("x"));
	EXPECT_EQ(x*y, parser.polynomial("x*y"));
	EXPECT_EQ(x*x, parser.polynomial("x*x"));
	EXPECT_EQ(x*x, parser.polynomial("x^2"));
}
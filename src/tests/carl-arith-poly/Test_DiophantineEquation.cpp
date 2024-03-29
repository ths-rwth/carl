#include <carl-arith/converter/OldGinacConverter.h>
#include <carl-arith/poly/umvpoly/functions/DiophantineEquation.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/core/VariablePool.h>
#include <carl-arith/interval/Interval.h>
#include <carl-io/StringParser.h>
#include "gtest/gtest.h"
#include <carl-arith/numbers/numbers.h>
#include <list>

using namespace carl;

#ifdef USE_CLN_NUMBERS

TEST(DiophantineEquation, extended_gcd_integer) {
	cln::cl_I s, t, a, b;
	cln::cl_I gcd;

	a = 4077;
	b = 4077;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = 12;
	b = 8;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = -11;
	b = -11;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = -77261;
	b = -884;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = 163;
	b = -9;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = -33;
	b = 33;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = 0;
	b = 5;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = -17;
	b = 0;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);

	a = 0;
	b = 0;
	gcd = extended_gcd_integer(a, b, s, t);
	EXPECT_EQ(s * a + t * b, gcd);
}

// helper function
cln::cl_I evaluatePolynomial(MultivariatePolynomial<cln::cl_I>& p, std::vector<cln::cl_I>& res) {
	std::map<carl::Variable, cln::cl_I> varMap;
	// the ordering of the values in res is like the ordering of the terms
	std::vector<carl::Term<cln::cl_I>> terms = p.terms();
	std::size_t start = p.has_constant_term() ? 1 : 0; // terms[0] is the constant part if there is one
	for (std::size_t i = start; i < terms.size(); i++) {
		varMap[terms[i].single_variable()] = res[i - start];
	}
	return carl::evaluate(p, varMap);
}

TEST(DiophantineEquation, solve_linear) {
	Variable x = carl::fresh_real_variable();
	Variable y = carl::fresh_real_variable();
	Variable z = carl::fresh_real_variable();
	Variable w = carl::fresh_real_variable();
	std::vector<cln::cl_I> res;
	MultivariatePolynomial<cln::cl_I> p;

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)6 * x, (Term<cln::cl_I>)((cln::cl_I)12)});
	res = solveDiophantine(p);
	EXPECT_EQ(evaluatePolynomial(p, res), 0);

	// not solvable
	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)4 * x, (Term<cln::cl_I>)((cln::cl_I)7)});
	res = solveDiophantine(p);
	EXPECT_FALSE(evaluatePolynomial(p, res) == 0);

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)-4499 * x});
	res = solveDiophantine(p);
	EXPECT_EQ(evaluatePolynomial(p, res), 0);

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)-17 * x, (cln::cl_I)4 * y, (cln::cl_I)6 * z});
	res = solveDiophantine(p);
	EXPECT_EQ(evaluatePolynomial(p, res), 0);

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)2 * x, (cln::cl_I)-2 * y, (cln::cl_I)4 * z, (Term<cln::cl_I>)((cln::cl_I)8)});
	res = solveDiophantine(p);
	EXPECT_EQ(evaluatePolynomial(p, res), 0);

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)-18 * x, (cln::cl_I)6 * y, (cln::cl_I)9 * z, (Term<cln::cl_I>)((cln::cl_I)15)});
	res = solveDiophantine(p);
	EXPECT_EQ(evaluatePolynomial(p, res), 0);

	// not solvable
	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)2 * x, (cln::cl_I)-100 * y, (cln::cl_I)22 * z, (Term<cln::cl_I>)((cln::cl_I)-777)});
	res = solveDiophantine(p);
	EXPECT_FALSE(evaluatePolynomial(p, res) == 0);

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)-38789 * x, (cln::cl_I)3772 * y, (cln::cl_I)1999 * z, (cln::cl_I)-180003 * w, (Term<cln::cl_I>)((cln::cl_I)-10029)});
	res = solveDiophantine(p);
	EXPECT_EQ(res.size(), carl::variables(p).size());
	EXPECT_EQ(evaluatePolynomial(p, res), 0);

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)-8 * x, (cln::cl_I)4 * y, (cln::cl_I)-12 * z, (cln::cl_I)7 * w, (Term<cln::cl_I>)((cln::cl_I)999)});
	res = solveDiophantine(p);
	EXPECT_EQ(evaluatePolynomial(p, res), 0);

	p = MultivariatePolynomial<cln::cl_I>({(cln::cl_I)2 * x, (cln::cl_I)2 * y, (cln::cl_I)-2 * z, (cln::cl_I)1 * w, (Term<cln::cl_I>)((cln::cl_I)7)});
	res = solveDiophantine(p);
	EXPECT_EQ(evaluatePolynomial(p, res), 0);
}
#endif

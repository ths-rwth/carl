#include "gtest/gtest.h"

#include "carl/core/rootfinder/RootFinder.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/RealAlgebraicNumber.h"
#include "carl/util/platform.h"

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#elif defined(__VS)
#include <mpirxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

typedef carl::UnivariatePolynomial<Rational> UPolynomial;
typedef carl::MultivariatePolynomial<Rational> MPolynomial;
typedef carl::UnivariatePolynomial<MPolynomial> UMPolynomial;

template<typename Number>
bool represents(const carl::RealAlgebraicNumberPtr<Number> root, const Number& exact) {
	if (root->isNumeric()) {
		return root->value() == exact;
	} else {
		carl::RealAlgebraicNumberIRPtr<Number> interval = std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(root);
		return interval->getInterval().contains(exact);
	}
}

TEST(RootFinder, realRoots)
{
	carl::VariablePool& vpool = carl::VariablePool::getInstance();
	carl::Variable x = vpool.getFreshVariable();
	carl::Variable y = vpool.getFreshVariable();

	{
		UPolynomial p(x, {(Rational)-1, (Rational)0, (Rational)0, (Rational)1});
		auto roots = carl::rootfinder::realRoots(p);
		ASSERT_TRUE(roots.size() == 1);
		ASSERT_TRUE(represents(roots.front(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::rootfinder::realRoots(p);
		ASSERT_TRUE(roots.size() == 1);
		ASSERT_TRUE(represents(roots.front(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(-1), MPolynomial(0), MPolynomial(1)});
		auto roots = carl::rootfinder::realRoots(p);
		ASSERT_TRUE(roots.size() == 2);
		ASSERT_TRUE(represents(roots.front(), (Rational)-1));
		ASSERT_TRUE(represents(roots.back(), (Rational)1));
	}

	{
		UMPolynomial p(x, {MPolynomial(y), MPolynomial(0), MPolynomial(1)});
		std::map<carl::Variable, carl::RealAlgebraicNumberPtr<Rational>> m;
		m[y] = carl::RealAlgebraicNumberNR<Rational>::create(-1);
		auto roots = carl::rootfinder::realRoots(p, m);
		ASSERT_TRUE(roots.size() == 2);
		ASSERT_TRUE(represents(roots.front(), (Rational)-1));
		ASSERT_TRUE(represents(roots.back(), (Rational)1));
	}
}

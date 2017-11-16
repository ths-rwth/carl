#include "gtest/gtest.h"

#include "../Common.h"

#include <carl/converter/CoCoAAdaptor.h>
#include <carl/core/MultivariatePolynomial.h>

#include <random>

#ifdef USE_COCOA

#include "CoCoA/library.H"

TEST(CoCoA, Basic) {
	auto xv = CoCoA::symbol("x");
	auto yv = CoCoA::symbol("y");
	CoCoA::ring Q = CoCoA::RingQQ();

	CoCoA::SparsePolyRing polyRing = CoCoA::NewPolyRing(Q, {xv, yv}, CoCoA::StdDegLex);
	auto x = CoCoA::indets(polyRing);

	auto p = (x[0] * x[0]) - 1;
	auto q = (x[0] + 1) * (x[0] - 2);

	auto g = CoCoA::gcd(p, q);
	EXPECT_EQ(x[0] + 1, g);
}

TEST(CoCoA, GCD) {
	using Poly = carl::MultivariatePolynomial<mpq_class>;
	carl::Variable x = carl::freshRealVariable("x");

	Poly p1 = (x * x) - mpq_class(1);
	Poly p2 = (x + mpq_class(1)) * (x - mpq_class(2));
	Poly p3 = x + mpq_class(1);

	carl::CoCoAAdaptor<Poly> c({p1, p2});
	Poly q = c.gcd(p1, p2);
	EXPECT_EQ(p3, q);
}

TEST(CoCoA, Factorize) {
	using Poly = carl::MultivariatePolynomial<mpq_class>;
	carl::Variable x = carl::freshRealVariable("x");

	Poly p1 = (x * x) - mpq_class(1);
	Poly p2 = (x + mpq_class(1)) * (x - mpq_class(2));
	Poly q1 = x + mpq_class(1);
	Poly q2 = x - mpq_class(1);
	Poly q3 = x - mpq_class(2);

	carl::CoCoAAdaptor<Poly> c({p1, p2});
	{
		auto res = c.factorize(p1);
		auto it1 = res.find(q1);
		EXPECT_FALSE(it1 == res.end());
		EXPECT_EQ(it1->second, 1);
		auto it2 = res.find(q2);
		EXPECT_FALSE(it2 == res.end());
		EXPECT_EQ(it2->second, 1);
	}
	{
		auto res = c.factorize(p2);
		auto it1 = res.find(q1);
		EXPECT_FALSE(it1 == res.end());
		EXPECT_EQ(it1->second, 1);
		auto it2 = res.find(q3);
		EXPECT_FALSE(it2 == res.end());
		EXPECT_EQ(it2->second, 1);
	}
}

TEST(CoCoA, SquareFreePart) {
	using Poly = carl::MultivariatePolynomial<mpq_class>;
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");

	carl::CoCoAAdaptor<Poly> c({Poly(x * y)});
	{
		Poly p = (x * x - mpq_class(1)) * (x * x - mpq_class(1)) * (x + mpq_class(1)) * (x - mpq_class(2));
		Poly res = Poly(x * x * x) - mpq_class(2) * x * x - x + mpq_class(2);
		EXPECT_EQ(c.squareFreePart(p), res);
	}
	{
		Poly p = (x * x - mpq_class(4)) * (x - mpq_class(2)) + (x * x * y - mpq_class(4) * y) * (x - mpq_class(2));
		Poly res = Poly(x * x * y) + x * x - mpq_class(4) * y - mpq_class(4);
		EXPECT_EQ(c.squareFreePart(p), res);
	}
}

carl::MultivariatePolynomial<mpq_class> randomPoly(const std::initializer_list<carl::Variable>& vars) {
	static std::mt19937 rand(4);
	carl::MultivariatePolynomial<mpq_class> res;
	for (std::size_t i = 0; i < 20; i++) {
		mpq_class coeff = rand() % 1024;
		carl::Monomial::Arg m;
		std::size_t left = i;
		for (const auto& v : vars) {
			if (left == 0) break;
			std::size_t e = rand() % (left + 1);
			if (e == 0) continue;
			m = m * createMonomial(v, e);
			left -= e;
		}
		res += coeff * m;
	}
	return res;
}

TEST(CoCoA, Benchmark) {
	using Poly = carl::MultivariatePolynomial<mpq_class>;
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");

	std::size_t count = 10;
	std::cout << "Generating " << count << "..." << std::endl;
	std::vector<std::tuple<Poly, Poly, Poly>> instances;
	for (std::size_t i = 0; i < count; i++) {
		Poly a = randomPoly({x, y});
		Poly b = randomPoly({x, y});
		Poly c = randomPoly({x, y});
		instances.emplace_back(a * c, b * c, c);
	}
	std::cout << "Starting..." << std::endl;
	carl::CoCoAAdaptor<Poly> conv({randomPoly({x, y})});
	carl::Timer timer;
	for (const auto& i : instances) {
		auto res = conv.gcd(std::get<0>(i), std::get<1>(i));
		EXPECT_EQ(res, std::get<2>(i));
	}
	std::cout << "Passed: " << (double(timer.passed()) / double(count)) << "ms per instance" << std::endl;
}

#endif

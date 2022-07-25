#include "gtest/gtest.h"

#include "../Common.h"

#include <carl-arith/poly/umvpoly/CoCoAAdaptor.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/functions/CoprimePart.h>
#include <carl-arith/poly/umvpoly/functions/SquareFreePart.h>
#include <carl-common/debug/Timer.h>

#include <random>
#include <map>

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
	carl::Variable x = carl::fresh_real_variable("x");

	Poly p1 = (x * x) - mpq_class(1);
	Poly p2 = (x + mpq_class(1)) * (x - mpq_class(2));
	Poly p3 = x + mpq_class(1);

	carl::CoCoAAdaptor<Poly> c({p1, p2});
	Poly q = c.gcd(p1, p2);
	EXPECT_EQ(p3, q);
}

TEST(CoCoA, Factorize) {
	using Poly = carl::MultivariatePolynomial<mpq_class>;
	carl::Variable x = carl::fresh_real_variable("x");

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

TEST(CoCoA, IrreducibleFactors) {
	using Poly = carl::MultivariatePolynomial<mpq_class>;
	carl::Variable x = carl::fresh_real_variable("x");

	Poly p1 = (x * x) - mpq_class(1);
	Poly p2 = (x + mpq_class(1)) * (x - mpq_class(2));
	Poly q1 = x + mpq_class(1);
	Poly q2 = x - mpq_class(1);
	Poly q3 = x - mpq_class(2);

	carl::CoCoAAdaptor<Poly> c({p1, p2});
	{
		auto factors = c.irreducible_factors(p1);
    std::map<Poly,int> res;
    for (Poly& p : factors)
      res[p] = 1;
    EXPECT_EQ(factors.size(), 2);
		auto it1 = res.find(q1);
		EXPECT_FALSE(it1 == res.end());
		EXPECT_EQ(it1->second, 1);
		auto it2 = res.find(q2);
		EXPECT_FALSE(it2 == res.end());
		EXPECT_EQ(it2->second, 1);
	}
	{
		auto factors = c.irreducible_factors(p2);
    std::map<Poly,int> res;
    for (Poly& p : factors)
      res[p] = 1;
    EXPECT_EQ(factors.size(), 2);
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
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");

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

TEST(CoCoA, SquareFreeBase)
{
	using Poly = carl::MultivariatePolynomial<mpq_class>;
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");
	carl::CoCoAAdaptor<Poly> c({Poly(x*y)});

	Poly p = (x*x - mpq_class(1)) * (x*x - mpq_class(1)) * (x-mpq_class(2));
	Poly q = (x*x + mpq_class(1)) * (x*x + mpq_class(1)) * (x+mpq_class(1));
	Poly r = (x*x - mpq_class(1)) * (x*x + mpq_class(1)) * (x+mpq_class(1)) * (x-mpq_class(2)) * (x+mpq_class(3));
	Poly res = (x+mpq_class(3));
	Poly onePoly = (x+mpq_class(1)-x);
	std::vector<Poly> polyvec({p,q});
	{
		Poly sqareFreeBase = c.makeCoprimeWith(carl::squareFreePart(r), p);
		sqareFreeBase = c.makeCoprimeWith(sqareFreeBase,q);
		EXPECT_EQ(sqareFreeBase, res);
		EXPECT_EQ(c.gcd(p,sqareFreeBase), onePoly);
		EXPECT_EQ(c.gcd(q,sqareFreeBase), onePoly);
	}
	{
		Poly sqareFreeBase = carl::coprimePart(carl::coprimePart(carl::squareFreePart(r), p), q);
		EXPECT_EQ(sqareFreeBase, res);
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
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");

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


TEST(CoCoA, Zeug1) {
	auto Q = CoCoA::RingQQ();
	auto Qx = CoCoA::NewPolyRing(Q, CoCoA::symbols("x"));
	auto x = CoCoA::indets(Qx)[0];
	
	auto cf = 1*x*x - 2;
	
	auto Qadjx = CoCoA::NewQuotientRing(Qx, CoCoA::ideal(cf));
	auto QHom = CoCoA::QuotientingHom(Qadjx);
	
	std::cout << cf << std::endl;
	std::cout << QHom(cf) << std::endl;
	
	auto Qy = CoCoA::NewPolyRing(Qadjx, CoCoA::symbols("y"));
	auto y = CoCoA::indets(Qy)[0];
	
	auto cg = (1*y*y*y*y - 2); // *(1*y*y*y*y - 2);
	
	std::cout << "CG: " << cg << std::endl;
	auto fact = CoCoA::factor(cg).myFactors();
	std::cout << fact << std::endl;
	//std::cout << QHom(cg) << std::endl;
	
	auto Qadjy = CoCoA::NewQuotientRing(Qy, CoCoA::ideal(cg));
	auto QyHom = CoCoA::QuotientingHom(Qadjy);
	
	std::cout << Qadjy << std::endl;
	std::cout << "Field? " << CoCoA::IsField(Qadjy) << std::endl;
	//
	auto Qz = CoCoA::NewPolyRing(Qadjy, CoCoA::symbols("z"));
	auto z = CoCoA::indets(Qz)[0];
	
	auto ch = 1*z*z*z*z - 2;
	
	try {
		auto facth = CoCoA::factor(ch).myFactors();
	}
	catch (const CoCoA::ErrorInfo& e) {
		std::cout << e << std::endl;
	}
	//std::cout << "CH: " << facth << std::endl;
}

TEST(CoCoA, Zeug) {
	auto Q = CoCoA::RingQQ();
	auto Qx = CoCoA::NewPolyRing(Q, CoCoA::symbols("x"));
	auto x = CoCoA::indets(Qx)[0];
	
	{
		auto test = 1*x*x*x*x - 9*x*x + 9;
		auto fact = CoCoA::factor(test).myFactors();
		std::cout << test << " -> " << fact << std::endl;
	}
	
	auto cf = 1*x*x*x*x - 6*x*x + 6;
	
	auto ideal = CoCoA::ideal(cf);
	auto Qadjx = CoCoA::NewQuotientRing(Qx, ideal);
	auto QHom = CoCoA::QuotientingHom(Qadjx);
	
	std::cout << cf << std::endl;
	std::cout << QHom(cf) << std::endl;
	
	auto Qy = CoCoA::NewPolyRing(Qadjx, CoCoA::symbols("y"));
	auto y = CoCoA::indets(Qy)[0];
	
	auto cg = 1*y*y*y*y - 9*y*y + 9;
	
	std::cout << "CG: " << cg << std::endl;
	auto fact = CoCoA::factor(cg).myFactors();
	std::cout << fact << std::endl;
	//std::cout << QHom(cg) << std::endl;
}

#endif

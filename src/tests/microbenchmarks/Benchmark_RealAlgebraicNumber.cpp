#include <benchmark/benchmark.h>

#include <carl/core/polynomialfunctions/RootFinder.h>
#include <carl/formula/model/ran/RealAlgebraicNumber.h>

using Poly = carl::UnivariatePolynomial<mpq_class>;

class RAN_Fixture: public benchmark::Fixture {
public:
	carl::Variable x = carl::freshRealVariable("x");
	Poly p = Poly(x, {-2, 0, 1}); // * Poly(x, {-2, 0, 0, 1});
};

BENCHMARK_F(RAN_Fixture, RAN_Create)(benchmark::State& state) {
	auto rans = carl::rootfinder::realRoots(p);
	auto p = rans[0].getIRPolynomial();
	auto i = rans[0].getInterval();
	for (auto _ : state) {
		auto ran = carl::RealAlgebraicNumber<mpq_class>(p, i);
	}
}

BENCHMARK_F(RAN_Fixture, RAN_CreateRefineOld)(benchmark::State& state) {
	auto rans = carl::rootfinder::realRoots(p);
	auto p = rans[0].getIRPolynomial();
	auto i = rans[0].getInterval();
	for (auto _ : state) {
		auto ran = carl::RealAlgebraicNumber<mpq_class>(p, i);
		ran.refine(false);
	}
}

BENCHMARK_F(RAN_Fixture, RAN_CreateRefineNew)(benchmark::State& state) {
	auto rans = carl::rootfinder::realRoots(p);
	auto p = rans[0].getIRPolynomial();
	auto i = rans[0].getInterval();
	for (auto _ : state) {
		auto ran = carl::RealAlgebraicNumber<mpq_class>(p, i);
		ran.refine(true);
	}
}

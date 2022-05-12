#include <benchmark/benchmark.h>

#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/numbers/numbers.h>

using MVP = carl::MultivariatePolynomial<mpq_class>;

class MVP_Add_Fixture: public benchmark::Fixture {
public:
    carl::Variable x = carl::freshRealVariable("x");
    carl::Variable y = carl::freshRealVariable("y");
    carl::Variable z = carl::freshRealVariable("z");
    MVP p = MVP(x)*x*x + MVP(x)*y*y + MVP(y)*z;
    MVP q = MVP(x)*x*y + MVP(x)*y*z + MVP(y)*z;
    static constexpr auto& tam = MVP::mTermAdditionManager;
};

BENCHMARK_F(MVP_Add_Fixture, MVP_Add)(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(MVP(p) += q);
    }
}

BENCHMARK_F(MVP_Add_Fixture, MVP_Add_alt)(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(MVP(p) += (q));
    }
}

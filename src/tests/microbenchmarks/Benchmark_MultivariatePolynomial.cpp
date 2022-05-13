#include <benchmark/benchmark.h>

#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/numbers/numbers.h>

using MVP = carl::MultivariatePolynomial<mpq_class>;

class MVP_Add_Fixture: public benchmark::Fixture {
public:
    carl::Variable x = carl::fresh_real_variable("x");
    carl::Variable y = carl::fresh_real_variable("y");
    carl::Variable z = carl::fresh_real_variable("z");
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

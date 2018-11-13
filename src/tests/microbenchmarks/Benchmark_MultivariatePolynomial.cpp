#include <benchmark/benchmark.h>

#include <carl/core/MultivariatePolynomial.h>
#include <carl/numbers/numbers.h>

using MVP = carl::MultivariatePolynomial<mpq_class>;

static void MVP_Add(benchmark::State& state) {
    carl::Variable x = carl::freshRealVariable("x");
    carl::Variable y = carl::freshRealVariable("y");
    carl::Variable z = carl::freshRealVariable("z");
    MVP p = MVP(x)*x*x + MVP(x)*y*y + MVP(y)*z;
    MVP q = MVP(x)*x*y + MVP(x)*y*z + MVP(y)*z;
    for (auto _ : state) {
        benchmark::DoNotOptimize(p + q);
    }
}
BENCHMARK(MVP_Add);

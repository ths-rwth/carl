#include <benchmark/benchmark.h>

#include <carl/core/Variable.h>
#include <carl/formula/uninterpreted/UFInstance.h>
#include <carl/formula/uninterpreted/UTerm.h>
#include <carl/formula/uninterpreted/UVariable.h>

template<typename T>
void pass_by_ref(const T& v) {
	benchmark::DoNotOptimize(v);
}
template<typename T>
void pass_by_value(T v) {
	benchmark::DoNotOptimize(v);
}

static void Variable_by_ref(benchmark::State& state) {
	carl::Variable v = carl::freshBooleanVariable();
	for (auto _ : state)
		pass_by_ref(v);
}
BENCHMARK(Variable_by_ref);

static void Variable_by_value(benchmark::State& state) {
	carl::Variable v = carl::freshBooleanVariable();
	for (auto _ : state)
		pass_by_value(v);
}
BENCHMARK(Variable_by_value);

static void UFInstance_by_ref(benchmark::State& state) {
	carl::UFInstance v;
	for (auto _ : state)
		pass_by_ref(v);
}
BENCHMARK(UFInstance_by_ref);

static void UFInstance_by_value(benchmark::State& state) {
	carl::UFInstance v;
	for (auto _ : state)
		pass_by_value(v);
}
BENCHMARK(UFInstance_by_value);

static void UTerm_by_ref(benchmark::State& state) {
	carl::UTerm v;
	for (auto _ : state)
		pass_by_ref(v);
}
BENCHMARK(UTerm_by_ref);

static void UTerm_by_value(benchmark::State& state) {
	carl::UTerm v;
	for (auto _ : state)
		pass_by_value(v);
}
BENCHMARK(UTerm_by_value);

static void UVariable_by_ref(benchmark::State& state) {
	carl::UVariable v;
	for (auto _ : state)
		pass_by_ref(v);
}
BENCHMARK(UVariable_by_ref);

static void UVariable_by_value(benchmark::State& state) {
	carl::UVariable v;
	for (auto _ : state)
		pass_by_value(v);
}
BENCHMARK(UVariable_by_value);

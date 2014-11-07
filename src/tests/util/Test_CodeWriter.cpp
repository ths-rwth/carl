#include <gmpxx.h>
#include "gtest/gtest.h"
#include "carl/io/CodeWriter.h"
#include "carl/core/Monomial.h"

#include <fstream>

using namespace carl;

TEST(CodeWriter, Basic)
{
		
    carl::GeneratorWriter<Monomial::Arg, Monomial::Arg> gw("TestAdditionGenerator");
	
	carl::Variable v1(1);
	carl::Variable v2(2);
	carl::Variable v3(3);
	gw.addCall(v1*v2, v2*v3);
	gw.addCall(v2*v2, v2*v1);
	gw.addCall(v3*v2, v2*v2);
	
	std::ofstream out("test.cpp");
	out << "#include \"Benchmark.h\"" << std::endl;
	out << gw << std::endl;
	out << "\
struct TestAdditionExecutor {\
	template<typename Coeff>\
	CUMP<Coeff> operator()(const std::tuple<CUMP<Coeff>,CUMP<Coeff>>& args) {\
		return std::get<0>(args) + std::get<1>(args);\
	}\
	GMP operator()(const std::tuple<GMP,GMP,GVAR>& args) {\
		return GiNaC::expand(std::get<0>(args) + std::get<1>(args));\
	}\
	ZMP operator()(const std::tuple<ZMP,ZMP,ZVAR>& args) {\
		return std::get<0>(args) + std::get<1>(args);\
	}\
};" << std::endl;
	out << "\
TEST_F(BenchmarkTest, Addition)\
{\
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);\
	bi.n = TestAdditionGenerator::size;\
	Benchmark<TestAdditionGenerator, TestAdditionExecutor, CMP<Coeff>> bench(bi, \"CArL\");\
	bench.compare<GMP, TupleConverter<GMP,GMP>>(\"GiNaC\");\
	bench.compare<ZMP, TupleConverter<ZMP,ZMP>>(\"Z3\");\
}\
" << std::endl;
}
	


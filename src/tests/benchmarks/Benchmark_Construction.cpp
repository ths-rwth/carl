#include "gtest/gtest.h"

#include "framework/Benchmark.h"
#include "carl/core/MultivariatePolynomial.h"

using namespace carl;

namespace carl {
	
	//##### Generator
	template<typename C>
	struct AdditionGenerator: public BaseGenerator {
		typedef std::tuple<CMP<C>,CMP<C>> type;
		AdditionGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			return std::make_tuple(g.newMP<C>(), g.newMP<C>());
		}
	};
	template<typename C>
	struct DivisionGenerator: public BaseGenerator {
		typedef std::tuple<CMP<C>,CMP<C>> type;
		DivisionGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			auto p1 = g.newMP<C>(bi.degree / 2);
			auto p2 = g.newMP<C>(bi.degree - bi.degree / 2);
			return std::make_tuple(p1*p2, p1);
		}
	};
	template<typename C>
	struct PowerGenerator: public BaseGenerator {
		typedef std::tuple<CMP<C>,unsigned> type;
		PowerGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			return std::make_tuple(g.newMP<C>(), 3);
		}
	};
	template<typename C>
	struct SubstituteGenerator: public BaseGenerator {
		typedef std::tuple<CMP<C>,CVAR,CMP<C>> type;
		SubstituteGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			return std::make_tuple(g.newMP<C>(), g.randomVariable(), g.newMP<C>());
		}
	};
	template<typename C>
	struct ResultantGenerator: public BaseGenerator {
		typedef std::tuple<CUMP<C>,CUMP<C>> type;
		ResultantGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			auto v = g.randomVariable();
			auto p1 = g.newMP<C>().toUnivariatePolynomial(v);
			auto p2 = g.newMP<C>().toUnivariatePolynomial(v);
			return std::make_tuple(p1, p2);
		}
	};

	//##### Executor
	struct AdditionExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::get<0>(args) + std::get<1>(args);
		}
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			return GiNaC::expand(std::get<0>(args) + std::get<1>(args));
		}
		ZMP operator()(const std::tuple<ZMP,ZMP>& args) {
			auto res = std::get<0>(args) + std::get<1>(args);
			//res.m().lex_sort(res);
			return res;
		}
	};
	struct MultiplicationExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::get<0>(args) * std::get<1>(args);
		}
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			return GiNaC::expand(std::get<0>(args) * std::get<1>(args));
		}
		ZMP operator()(const std::tuple<ZMP,ZMP>& args) {
			auto res = std::get<0>(args) * std::get<1>(args);
			res.m().lex_sort(res);
			return res;
		}
	};
	struct DivisionExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			//return std::get<0>(args).quotient(std::get<1>(args));
			CMP<Coeff> res;
			std::get<0>(args).divideBy(std::get<1>(args), res);
			return res;
		}
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			GMP res;
			GiNaC::divide(std::get<0>(args), std::get<1>(args), res);
			return GiNaC::expand(res);
		}
		ZMP operator()(const std::tuple<ZMP,ZMP>& args) {
			auto res = exact_div(std::get<0>(args), std::get<1>(args));
			res.m().lex_sort(res);
			return res;
		}
	};
	struct RemainderExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::get<0>(args).remainder(std::get<1>(args));
		}
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			GMP res;
			GiNaC::prem(std::get<0>(args), std::get<1>(args), res);
			return GiNaC::expand(res);
		}
		ZMP operator()(const std::tuple<ZMP,ZMP>& args) {
			ZMP p1 = std::get<0>(args);
			ZMP p2 = std::get<1>(args);
			ZMP res(p1.m());
			unsigned d;
			res = pseudo_remainder(p1, p2, 0, d);
			//res.m().lex_sort(res);
			return res;
		}
	};
	struct PowerExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,unsigned>& args) {
			return std::get<0>(args).pow(std::get<1>(args));
		}
		GMP operator()(const std::tuple<GMP,unsigned>& args) {
			return GiNaC::expand(GiNaC::pow(std::get<0>(args), std::get<1>(args)));
		}
		ZMP operator()(const std::tuple<ZMP,unsigned>& args) {
			ZMP p = std::get<0>(args);
			ZMP res(p.m());
			p.m().pw(p, std::get<1>(args), res);
			res.m().lex_sort(res);
			return res;
		}
	};
	struct SubstituteExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CVAR,CMP<Coeff>>& args) {
			return std::get<0>(args).substitute(std::get<1>(args), std::get<2>(args));
		}
		GMP operator()(const std::tuple<GMP,GVAR,GMP>& args) {
			return GiNaC::expand(std::get<0>(args).subs(std::get<1>(args) == std::get<2>(args)));
		}
		ZMP operator()(const std::tuple<ZMP,ZVAR,ZMP>& args) {
			return std::get<0>(args);
		}
	};
	struct ResultantExecutor {
		template<typename Coeff>
		CUMP<Coeff> operator()(const std::tuple<CUMP<Coeff>,CUMP<Coeff>>& args) {
			return std::get<0>(args).resultant(std::get<1>(args));
		}
		GMP operator()(const std::tuple<GMP,GMP,GVAR>& args) {
			return GiNaC::expand(GiNaC::resultant(std::get<0>(args), std::get<1>(args), std::get<2>(args)));
		}
		ZMP operator()(const std::tuple<ZMP,ZMP,ZVAR>& args) {
			return resultant(std::get<0>(args), std::get<1>(args), std::get<2>(args));
		}
	};
	
	//##### Converter
	template<typename P, typename V>
	struct ResultantConverter: public BaseConverter {
	public:
		typedef std::tuple<P, P, V> type;
		ResultantConverter(const CIPtr& ci): BaseConverter(ci) {}
		template<typename Coeff>
		type operator()(const std::tuple<CUMP<Coeff>, CUMP<Coeff>>& t) {
			auto p1 = Conversion::template convert<P>(std::get<0>(t), ci);
			auto p2 = Conversion::template convert<P>(std::get<1>(t), ci);
			auto v = Conversion::template convert<V>(std::get<0>(t).mainVar(), ci);
			return std::make_tuple(p1, p2, v);
		}
	};
}


typedef cln::cl_RA Coeff;

class BenchmarkTest : public ::testing::Test {
protected:
	BenchmarkTest(): file({"CArL", "GiNaC", "Z3"}) {
	}
	~BenchmarkTest() {
		auto info = ::testing::UnitTest::GetInstance()->current_test_info();
		std::stringstream ss;
		ss << "benchmark_" << info->name() << ".tex";
		std::ofstream out(ss.str(), std::ios_base::out);
		out << file;
	}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
	BenchmarkFile<std::size_t> file;
};

TEST_F(BenchmarkTest, ReleaseCheck)
{
#ifndef NDEBUG
	std::cout << "Warning: You have compiled in DEBUG mode." << std::endl;
#endif
}

TEST_F(BenchmarkTest, Addition)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 100;
	for (bi.degree = 15; bi.degree < 16; bi.degree++) {
		Benchmark<AdditionGenerator<Coeff>, AdditionExecutor, CMP<Coeff>> bench(bi, "CArL");
		//bench.compare<CMP<mpq_class>, TupleConverter<CMP<mpq_class>,CMP<mpq_class>>>("CArL GMP");
		//bench.compare<GMP, TupleConverter<GMP,GMP>>("GiNaC");
		//bench.compare<ZMP, TupleConverter<ZMP,ZMP>>("Z3");
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Multiplication)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 1000;
	for (bi.degree = 5; bi.degree < 10; bi.degree++) {
		Benchmark<AdditionGenerator<Coeff>, MultiplicationExecutor, CMP<Coeff>> bench(bi, "CArL");
		bench.compare<GMP, TupleConverter<GMP,GMP>>("GiNaC");
		bench.compare<ZMP, TupleConverter<ZMP,ZMP>>("Z3");
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Division)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 3);
	bi.n = 1000;
	for (bi.degree = 10; bi.degree < 20; bi.degree++) {
		Benchmark<DivisionGenerator<Coeff>, DivisionExecutor, CMP<Coeff>> bench(bi, "CArL");
		bench.compare<GMP, TupleConverter<GMP,GMP>>("GiNaC");
		bench.compare<ZMP, TupleConverter<ZMP,ZMP>>("Z3");
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Power)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 1000;
	for (bi.degree = 5; bi.degree < 8; bi.degree++) {
		Benchmark<PowerGenerator<Coeff>, PowerExecutor, CMP<Coeff>> bench(bi, "CArL");
		bench.compare<GMP, TupleConverter<GMP,unsigned>>("GiNaC");
		bench.compare<ZMP, TupleConverter<ZMP,unsigned>>("Z3");
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Substitute)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 1000;
	for (bi.degree = 5; bi.degree < 8; bi.degree++) {
		Benchmark<SubstituteGenerator<Coeff>, SubstituteExecutor, CMP<Coeff>> bench(bi, "CArL");
		bench.compare<GMP, TupleConverter<GMP,GVAR,GMP>>("GiNaC");
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Resultant)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 4);
	bi.n = 10;
	for (bi.degree = 5; bi.degree < 8; bi.degree++) {
		Benchmark<ResultantGenerator<Coeff>, ResultantExecutor, CUMP<Coeff>> bench(bi, "CArL");
		bench.compare<GMP, ResultantConverter<GMP,GVAR>>("GiNaC");
		bench.compare<ZMP, ResultantConverter<ZMP,ZVAR>>("Z3");
		file.push(bench.result(), bi.degree);
	}
}

TEST(Benchmark, BuildPDF)
{
	std::ofstream out("benchmarks.tex");
	out << "\\documentclass{article}" << std::endl;
	out << "\\begin{document}" << std::endl;
	auto inst = ::testing::UnitTest::GetInstance();
	auto testcase = inst->GetTestCase(0);
	for (auto i = 0; i < testcase->total_test_count(); i++) {
		auto cur = testcase->GetTestInfo(i);
		out << "\\subsection*{" << cur->name() << "}" << std::endl;
		out << "\\input{benchmark_" << cur->name() << "}" << std::endl << std::endl;
	}
	out << "\\end{document}";
	out.close();
	system("pdflatex benchmarks");
}
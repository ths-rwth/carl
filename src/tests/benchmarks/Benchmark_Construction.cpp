#include "gtest/gtest.h"

#include <algorithm>

#include "framework/Benchmark.h"
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/functions/GCD.h>
#include <carl-arith/poly/umvpoly/functions/Resultant.h>
#include <carl-arith/poly/umvpoly/functions/Substitution.h>
#include "BenchmarkTest.h"
#include "framework/BenchmarkGenerator.h"

#include <carl-common/config.h>
#include <carl-logging/config.h>


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
	struct PremGenerator: public BaseGenerator {
		typedef std::tuple<CMP<C>,CMP<C>,CVAR> type;
		PremGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			auto p1 = g.newMP<C>(bi.degree);
			auto p2 = g.newMP<C>(bi.degree - bi.degree / 4);
			auto v = g.randomVariable();
			return std::make_tuple(p1, p2, v);
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
			return std::make_tuple(g.newMP<C>(), g.randomVariable(), g.newMP<C>(4));
		}
	};
	template<typename C>
	struct ResultantGenerator: public BaseGenerator {
		typedef std::tuple<CUMP<C>,CUMP<C>> type;
		ResultantGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			auto v = g.randomVariable();
			auto p1 = carl::to_univariate_polynomial(g.newMP<C>(), v);
			auto p2 = carl::to_univariate_polynomial(g.newMP<C>(), v);
			return std::make_tuple(p1, p2);
		}
	};
	template<typename C>
	struct ComparisonGenerator: public BaseGenerator {
		typedef std::tuple<CMP<C>,CMP<C>> type;
		ComparisonGenerator(const BenchmarkInformation& bi): BaseGenerator(bi) {}
		type operator()() const {
			auto p1 = g.newMP<C>();
			auto p2(p1);
			std::shuffle(p1.terms().begin(), p1.terms().end(), std::mt19937(std::random_device()()));
			p1.makeMinimallyOrdered();
			return std::make_tuple(p1, p2);
		}
	};

	//##### Executor
	struct AdditionExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::get<0>(args) + std::get<1>(args);
		}
		#ifdef USE_COCOA
		CoMP operator()(const std::tuple<CoMP,CoMP>& args) {
			return std::forward<const CoMP>(std::get<0>(args) + std::get<1>(args));
		}
		#endif
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			return std::forward<const GMP>(GiNaC::expand(std::get<0>(args) + std::get<1>(args)));
		}
        #endif
	};
	struct MultiplicationExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::forward<const CMP<Coeff>>(std::get<0>(args) * std::get<1>(args));
		}
		#ifdef USE_COCOA
		CoMP operator()(const std::tuple<CoMP,CoMP>& args) {
			return std::forward<const CoMP>(std::get<0>(args) * std::get<1>(args));
		}
		#endif
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			return std::forward<const GMP>(GiNaC::expand(std::get<0>(args) * std::get<1>(args)));
		}
        #endif
	};
	struct DivisionExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			//return std::get<0>(args).quotient(std::get<1>(args));
			CMP<Coeff> res;
			carl::try_divide(std::get<0>(args), std::get<1>(args), res);
			return std::forward<const CMP<Coeff>>(res);
		}
		#ifdef USE_COCOA
		CoMP operator()(const std::tuple<CoMP,CoMP>& args) {
			return std::forward<const CoMP>(std::get<0>(args) / std::get<1>(args));
		}
		#endif
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			GMP res;
			GiNaC::divide(std::get<0>(args), std::get<1>(args), res);
			return std::forward<const GMP>(GiNaC::expand(res));
		}
        #endif
	};
	struct PremExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>,CVAR>& args) {
			CMP<Coeff> res = carl::pseudo_remainder(std::get<0>(args), std::get<1>(args), std::get<2>(args));
			return std::forward<const CMP<Coeff>>(res);
		}
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GMP,GVAR>& args) {
			return std::forward<const GMP>(GiNaC::expand(GiNaC::prem(std::get<0>(args), std::get<1>(args), std::get<2>(args))));
		}
        #endif
	};
	struct RemainderExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::forward<const CMP<Coeff>>(std::get<0>(args).remainder(std::get<1>(args)));
		}
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			GMP res;
			GiNaC::prem(std::get<0>(args), std::get<1>(args), res);
			return std::forward<const GMP>(GiNaC::expand(res));
		}
        #endif
	};
	struct PowerExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,unsigned>& args) {
			return std::forward<const CMP<Coeff>>(carl::pow(std::get<0>(args), std::get<1>(args)));
		}
        #ifdef USE_COCOA
		CoMP operator()(const std::tuple<CoMP,unsigned>& args) {
			return std::forward<const CoMP>(CoCoA::power(std::get<0>(args), std::get<1>(args)));
		}
        #endif
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,unsigned>& args) {
			return std::forward<const GMP>(GiNaC::expand(GiNaC::pow(std::get<0>(args), std::get<1>(args))));
		}
        #endif
	};
	struct SubstituteExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CVAR,CMP<Coeff>>& args) {
			return std::forward<const CMP<Coeff>>(carl::substitute(std::get<0>(args), std::get<1>(args), std::get<2>(args)));
		}
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GVAR,GMP>& args) {
			return std::forward<const GMP>(GiNaC::expand(std::get<0>(args).subs(std::get<1>(args) == std::get<2>(args))));
		}
        #endif
	};
	struct ResultantExecutor {
		template<typename Coeff>
		CUMP<Coeff> operator()(const std::tuple<CUMP<Coeff>,CUMP<Coeff>>& args) {
			//carl::Resultant calc;
			//return std::forward<const CUMP<Coeff>>(calc.resultant_z3(std::get<0>(args), std::get<1>(args)));
			return std::forward<const CUMP<Coeff>>(carl::resultant(std::get<0>(args), std::get<1>(args)));
		}
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GMP,GVAR>& args) {
			return std::forward<const GMP>(GiNaC::expand(GiNaC::resultant(std::get<0>(args), std::get<1>(args), std::get<2>(args))));
		}
        #endif
	};
	struct GCDExecutor {
		template<typename Coeff>
		CMP<Coeff> operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::forward<const CMP<Coeff>>(carl::gcd(std::get<0>(args), std::get<1>(args)));
		}
        #ifdef USE_GINAC
		GMP operator()(const std::tuple<GMP,GMP>& args) {
			return std::forward<const GMP>(GiNaC::expand(GiNaC::gcd(std::get<0>(args), std::get<1>(args))));
		}
        #endif
	};
	struct CompareExecutor {
		template<typename Coeff>
		bool operator()(const std::tuple<CMP<Coeff>,CMP<Coeff>>& args) {
			return std::get<0>(args) == std::get<1>(args);
		}
        #ifdef USE_COCOA
		bool operator()(const std::tuple<CoMP,CoMP>& args) {
			return std::get<0>(args) == std::get<1>(args);
		}
        #endif
        #ifdef USE_GINAC
		bool operator()(const std::tuple<GMP,GMP>& args) {
			return std::get<0>(args) == std::get<1>(args);
		}
        #endif
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
			auto v = Conversion::template convert<V>(std::get<0>(t).main_var(), ci);
			return std::make_tuple(p1, p2, v);
		}
	};
}

TEST_F(BenchmarkTest, ReleaseCheck)
{
#ifndef NDEBUG
	std::cout << "Warning: You have compiled in DEBUG mode." << std::endl;
#endif
#ifdef LOGGING
	std::cout << "Warning: You have compiled with LOGGING enabled." << std::endl;
#endif
}

typedef mpq_class Coeff;

TEST_F(BenchmarkTest, Addition)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 1000;
	for (bi.degree = 15; bi.degree < 25; bi.degree += 2) {
        Benchmark<AdditionGenerator<Coeff>, AdditionExecutor, CMP<Coeff>> bench(bi, "CArL");
		//bench.compare<CMP<mpq_class>, TupleConverter<CMP<mpq_class>,CMP<mpq_class>>>("CArL GMP");
		#ifdef USE_COCOA
		bench.compare<CoMP, TupleConverter<CoMP,CoMP>>("CoCoA");
		#endif
        #ifdef USE_GINAC
		bench.compare<GMP, TupleConverter<GMP,GMP>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Multiplication)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 1000;
	for (bi.degree = 5; bi.degree < 14; bi.degree++) {
		Benchmark<AdditionGenerator<Coeff>, MultiplicationExecutor, CMP<Coeff>> bench(bi, "CArL");
		//break;
		#ifdef USE_COCOA
		bench.compare<CoMP, TupleConverter<CoMP,CoMP>>("CoCoA");
		#endif
        #ifdef USE_GINAC
		if (bi.degree <= 10)
		bench.compare<GMP, TupleConverter<GMP,GMP>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Division)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 3);
	bi.n = 1000;
	for (bi.degree = 10; bi.degree < 16; bi.degree++) {
		Benchmark<DivisionGenerator<Coeff>, DivisionExecutor, CMP<Coeff>> bench(bi, "CArL");
		#ifdef USE_COCOA
		bench.compare<CoMP, TupleConverter<CoMP,CoMP>>("CoCoA");
		#endif
        #ifdef USE_GINAC
		bench.compare<GMP, TupleConverter<GMP,GMP>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Prem)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 3);
	bi.n = 100;
	for (bi.degree = 8; bi.degree < 13; bi.degree += 2) {
		Benchmark<PremGenerator<Coeff>, PremExecutor, CMP<Coeff>> bench(bi, "CArL");
        #ifdef USE_GINAC
		bench.compare<GMP, TupleConverter<GMP,GMP,GVAR>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Power)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 1000;
	for (bi.degree = 5; bi.degree < 10; bi.degree++) {
		Benchmark<PowerGenerator<Coeff>, PowerExecutor, CMP<Coeff>> bench(bi, "CArL");
		#ifdef USE_COCOA
		bench.compare<CoMP, TupleConverter<CoMP,unsigned>>("CoCoA");
        #endif
        #ifdef USE_GINAC
		if (bi.degree <= 7)
		bench.compare<GMP, TupleConverter<GMP,unsigned>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Substitute)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 6);
	bi.n = 1000;
	for (bi.degree = 5; bi.degree < 11; bi.degree++) {
		Benchmark<SubstituteGenerator<Coeff>, SubstituteExecutor, CMP<Coeff>> bench(bi, "CArL");
        #ifdef USE_COCOA
		//bench.compare<CoMP, TupleConverter<CoMP,CoVAR,CoMP>>("CoCoA");
        #endif
        #ifdef USE_GINAC
		bench.compare<GMP, TupleConverter<GMP,GVAR,GMP>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Resultant)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 3);
	bi.n = 10;
	for (bi.degree = 5; bi.degree < 7; bi.degree++) {
		Benchmark<ResultantGenerator<Coeff>, ResultantExecutor, CUMP<Coeff>> bench(bi, "CArL");
        #ifdef USE_COCOA
		//bench.compare<CoMP, ResultantConverter<CoMP,CoVAR>>("CoCoA");
        #endif
        #ifdef USE_GINAC
		bench.compare<GMP, ResultantConverter<GMP,GVAR>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, GCD)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 4);
	bi.n = 10;
	for (bi.degree = 5; bi.degree < 13; bi.degree++) {
		Benchmark<AdditionGenerator<Coeff>, GCDExecutor, CMP<Coeff>> bench(bi, "CArL");
        #ifdef USE_GINAC
		bench.compare<GMP, TupleConverter<GMP,GMP>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST_F(BenchmarkTest, Compare)
{
	BenchmarkInformation bi(BenchmarkSelection::Random, 3);
	bi.n = 1000;
	for (bi.degree = 20; bi.degree < 29; bi.degree+=2) {
		Benchmark<ComparisonGenerator<Coeff>, CompareExecutor, bool> bench(bi, "CArL");
        #ifdef USE_COCOA
		bench.compare<bool, TupleConverter<CoMP,CoMP>>("CoCoA");
        #endif
        #ifdef USE_GINAC
		bench.compare<bool, TupleConverter<GMP,GMP>>("GiNaC");
        #endif
		file.push(bench.result(), bi.degree);
	}
}

TEST(Benchmark, BuildPDF)
{
	auto inst = ::testing::UnitTest::GetInstance();
	auto testcase = inst->GetTestCase(0);
	std::ofstream out("benchmarks/benchmarks.tex");
	out << "\\documentclass{article}" << std::endl;
	out << "\\usepackage{tikz}" << std::endl;
	out << "\\usepackage{pgfplots}" << std::endl;
	for (auto i = 0; i < testcase->total_test_count(); i++) {
		auto cur = testcase->GetTestInfo(i);
		out << "\\input{benchmark_" << cur->name() << "_data.tex}" << std::endl;
	}
	out << "\\begin{document}" << std::endl;
	for (auto i = 0; i < testcase->total_test_count(); i++) {
		auto cur = testcase->GetTestInfo(i);
		out << "\\subsection*{" << cur->name() << "}" << std::endl;
		out << "\\input{benchmark_" << cur->name() << "}" << std::endl << std::endl;
	}
	out << "\\end{document}";
	out.close();
	//system("cd benchmarks && pdflatex benchmarks");
}

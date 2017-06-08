#include "../Common.h"

#include <carl/core/MultivariatePolynomial.h>
#include <carl/io/CodeWriter.h>

#include <fstream>


using namespace carl;

TEST(CodeWriter, Basic)
{

    carl::GeneratorWriter<carl::MultivariatePolynomial<Rational>, carl::MultivariatePolynomial<Rational>> gw("TestAdditionGenerator");

	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");
	carl::Variable z = carl::freshRealVariable("z");

	carl::MultivariatePolynomial<Rational> p1({Rational(1)*x, Rational(-1)*x*x, Rational(3)*x*x*x});
	carl::MultivariatePolynomial<Rational> p2({Rational(1)*x, Rational(-1)*y*z, Rational(3)*x*z*z});
	carl::MultivariatePolynomial<Rational> p3({Rational(1)*z, Rational(-1)*x*x, Rational(3)*y*x*x});


	gw.addCall(p1*p2, p2*p3);
	gw.addCall(p2*p2, p2*p1);
	gw.addCall(p3*p2, p2*p2);

	std::ofstream out("test.cpp");
	out << "\
#include <utility>\n\
#include <memory>\n\
#include \"gtest/gtest.h\"\n\
#include \"BenchmarkTest.h\"\n\
#include \"carl/core/Monomial.h\"\n\
#include \"framework/Benchmark.h\"\n\
#include \"framework/BenchmarkGenerator.h\"\n\
#include \"framework/Common.h\"\n";
	out << "\
using carl::Variable;\n\
using carl::exponent;\n\
using carl::Monomial;\n";
	out << gw << std::endl;
	out << "\
struct TestAdditionExecutor {\n\
	\ttemplate<typename Coeff>\n\
	\tcarl::CMP<Coeff> operator()(const std::tuple<carl::CMP<Coeff>,carl::CMP<Coeff>>& args) {\n\
		\t\treturn std::get<0>(args) + std::get<1>(args);\n\
	\t}\n\
	\tcarl::GMP operator()(const std::tuple<carl::GMP,carl::GMP>& args) {\n\
		\t\treturn GiNaC::expand(std::get<0>(args) + std::get<1>(args));\n\
	\t}\n\
	\t#ifdef COMPARE_WITH_Z3 \n\
	\tcarl::ZMP operator()(const std::tuple<carl::ZMP,carl::ZMP>& args) {\n\
		\t\treturn std::get<0>(args) + std::get<1>(args);\n\
	\t}\n\
	\t#endif\n\
};" << std::endl<<std::endl;
	out << "using carl::BenchmarkTest;\n\
TEST_F(BenchmarkTest, Addition)\n\
{\n\
	\tcarl::BenchmarkInformation bi(carl::BenchmarkSelection::Random, 6);\n\
	\tbi.n = TestAdditionGenerator<Coeff>::size;\n\
	\tcarl::Benchmark<TestAdditionGenerator<Coeff>, TestAdditionExecutor, carl::CMP<Coeff>> bench(bi, \"CArL\");\n\
	\tbench.compare<carl::GMP, carl::TupleConverter<carl::GMP,carl::GMP>>(\"GiNaC\");\n\
	\t#ifdef COMPARE_WITH_Z3\n\
	\tbench.compare<carl::ZMP, carl::TupleConverter<carl::ZMP,carl::ZMP>>(\"Z3\");\n\
	\t#endif\n\
}\
" << std::endl;
}

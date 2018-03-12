#include <gtest/gtest.h>

#include <carl/core/MultivariatePolynomial.h>
#include <carl/formula/Formula.h>
#include <carl/formula/symmetry/symmetry.h>

#include <bliss/graph.hh>

#include "../Common.h"

typedef carl::MultivariatePolynomial<Rational> Pol;
typedef carl::Constraint<Pol> Constr;
typedef carl::Formula<Pol> FormulaT;

void report_aut(void* param, const unsigned int n, const unsigned int* aut)
{
  std::cout << "Generator: ";
  for (unsigned int i = 0; i < n; ++i) {
	  std::cout << aut[i] << " ";
  }
  std::cout << std::endl;
}

TEST(Symmetry, BlissBase)
{
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");
	
	Pol lhsA = Rational(1)*x*x + Rational(1)*y*y + Rational(-1);
	Pol lhsB = Rational(1)*x*x*y + Rational(1)*x*y*y + Rational(-1);

	FormulaT fA(Constr(lhsA, carl::Relation::EQ));
	FormulaT fB(Constr(lhsB, carl::Relation::EQ));
	FormulaT f(carl::FormulaType::AND, {fA, fB});
	
	bliss::Digraph dg;
	auto v_vx = dg.add_vertex(0);
	auto v_vy = dg.add_vertex(1);
	auto v_c1 = dg.add_vertex(2);
	auto vand = dg.add_vertex(3);
	std::cout << "x -> " << v_vx << std::endl;
	std::cout << "y -> " << v_vy << std::endl;
	std::cout << "1 -> " << v_c1 << std::endl;
	std::cout << "AND -> " << vand << std::endl;
	
	auto va = dg.add_vertex(4);
	auto vap = dg.add_vertex(5);
	auto vam1 = dg.add_vertex(6);
	auto vam2 = dg.add_vertex(6);
	auto vam3 = dg.add_vertex(6);
	
	std::cout << "C1 -> " << va << std::endl;
	std::cout << "C1+ -> " << vap << std::endl;
	std::cout << "C1M1 -> " << vam1 << std::endl;
	std::cout << "C1M2 -> " << vam2 << std::endl;
	std::cout << "C1M3 -> " << vam3 << std::endl;
	
	dg.add_edge(vand, va);
	dg.add_edge(va, vap);
	dg.add_edge(vap, vam1);
	dg.add_edge(vap, vam2);
	dg.add_edge(vap, vam3);
	dg.add_edge(vam1, v_c1);
	dg.add_edge(vam1, v_vx);
	dg.add_edge(vam1, v_vx);
	dg.add_edge(vam2, v_c1);
	dg.add_edge(vam2, v_vy);
	dg.add_edge(vam2, v_vy);
	dg.add_edge(vam3, v_c1);
	
	auto vb = dg.add_vertex(4);
	auto vbp = dg.add_vertex(5);
	auto vbm1 = dg.add_vertex(6);
	auto vbm2 = dg.add_vertex(6);
	auto vbm3 = dg.add_vertex(6);
	
	std::cout << "C2 -> " << vb << std::endl;
	std::cout << "C2+ -> " << vbp << std::endl;
	std::cout << "C2M1 -> " << vbm1 << std::endl;
	std::cout << "C2M2 -> " << vbm2 << std::endl;
	std::cout << "C2M3 -> " << vbm3 << std::endl;
	
	auto tmp1 = dg.add_vertex(7);
	auto tmp2 = dg.add_vertex(7);
	auto tmp3 = dg.add_vertex(7);
	auto tmp4 = dg.add_vertex(7);
	auto tmp5 = dg.add_vertex(7);
	auto tmp6 = dg.add_vertex(7);
	
	dg.add_edge(vand, vb);
	dg.add_edge(vb, vbp);
	dg.add_edge(vbp, vbm1);
	dg.add_edge(vbp, vbm2);
	dg.add_edge(vbp, vbm3);
	dg.add_edge(vbm1, v_c1);
	dg.add_edge(vbm1, tmp1);
	dg.add_edge(tmp1, v_vx);
	dg.add_edge(vbm1, tmp2);
	dg.add_edge(tmp2, v_vx);
	dg.add_edge(vbm1, tmp3);
	dg.add_edge(tmp3, v_vy);
	dg.add_edge(vbm2, v_c1);
	dg.add_edge(vbm2, tmp4);
	dg.add_edge(tmp4, v_vx);
	dg.add_edge(vbm2, tmp5);
	dg.add_edge(tmp5, v_vy);
	dg.add_edge(vbm2, tmp6);
	dg.add_edge(tmp6, v_vy);
	dg.add_edge(vbm3, v_c1);
	
	dg.write_dot("test.dot");
	
	bliss::Stats stats;
	dg.find_automorphisms(stats, &report_aut, nullptr);
	
	auto res = carl::formula::findSymmetries(f);
	std::cout << res << std::endl;
	auto res2 = carl::formula::breakSymmetries(f);
	std::cout << res2 << std::endl;
}

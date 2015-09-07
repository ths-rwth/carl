#include "../carl/core/MultivariateHorner.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"
#include "../interval/IntervalEvaluation.h"

#ifdef COMPARE_WITH_GINAC
#include <cln/cln.h>
typedef cln::cl_RA Rational;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
#endif

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;

int main (int argc, char** argv)
{
	
  StringParser sp;
	sp.setVariables({"x", "y", "z"});
	//Pol p1 = sp.parseMultivariatePolynomial<Rational>("x^2*y^4*z^5*3 + x^3*10*y^4 + 20*z^6*y^2 + 21*x^9*z^2 + 4*x*y");
	//Pol p1 = sp.parseMultivariatePolynomial<Rational>("x*y + x*z");
  Pol p1 = sp.parseMultivariatePolynomial<Rational>("12");
  //Pol p1 = sp.parseMultivariatePolynomial<Rational>("2184*x^17+15708*z+(-126672)*x^2+643384*z^3+(-2306444)*z^4+4162512*x^13+(-10186920)*z^12+18820800*x^11+(-27118448)*x^10+31123477*x^9+6199788*x^5+(-12956461)*x^6+21524503*x^7+(-28784511)*x^8+(-1226048)*x^14+245224*y^15+(-31192)*y^16+(-924)");


	std::set<Variable> allVarInPolynome;
	p1.gatherVariables(allVarInPolynome);
	std::map<Variable, Interval<Rational>> map;

	for(auto i : allVarInPolynome) {
  	  if (i.getId() == 1)
  	  {
  	  	Interval<Rational> ix (-2, 2);
  	  	map[i] = ix;
  	  }
  	  if (i.getId() == 2)
  	  {
  	  	Interval<Rational> iy (0, 1);
  	  	map[i] = iy;
  	  }
  	  if (i.getId() == 3)
  	  {
  	  	Interval<Rational> iz (0, 1);
  	  	map[i] = iz;
  	  }
  	}

	std::cout << "\n Polynom       :" << p1 << std::endl;

	MultivariateHorner< Pol, GREEDY_Is > peterPolynom (std::move(p1),map);
  //MultivariateHorner< Pol, GREEDY_IIs > peterPolynom2 (p1,map);

	std::cout << "\n GREEDY_Is        :" << peterPolynom << std::endl;
  //std::cout << " GREEDY_IIs       :" << peterPolynom2 << std::endl;

	Interval<Rational> testInterval = IntervalEvaluation::evaluate(peterPolynom , map);
  //Interval<Rational> testInterval2 = evaluate(peterPolynom2 , map);

	std::cout << "\n Evaluate Horner :" << testInterval << std::endl;
  //std::cout << " Evaluate Horner2:" << testInterval2 << std::endl;
  
  
  /*
  Rational u_a;
  Rational l_a;
  Rational u_b;
  Rational l_b;
  Rational u_c;
  Rational l_c;

  std::map<Variable, Interval<Rational>> map;
  VariablePool& vpool = VariablePool::getInstance();
  Variable a = vpool.getFreshVariable();
  vpool.setName(a, "a");
  Variable b = vpool.getFreshVariable();
  vpool.setName(b, "b");
  Variable c = vpool.getFreshVariable();
  vpool.setName(c, "c");


  for (int i = 1; i <= 6; i++)
  {

    switch (i) 
      {
        case 1 : l_a = -200; u_a = -100;
        case 2 : l_a = -100; u_a = 0;
        case 3 : l_a = -100; u_a = 100;
        case 4 : l_a = 0; u_a = 0;
        case 5 : l_a = 0; u_a = 100;
        case 6 : l_a = 100; u_a = 200;
      }
    for (int j = 1; j <= 6; j++)
    {
      switch (j) 
        {
          case 1 : l_b = -200; u_b = -100;
          case 2 : l_b = -100; u_b = 0;
          case 3 : l_b = -100; u_b = 100;
          case 4 : l_b = 0; u_b = 0;
          case 5 : l_b = 0; u_b = 100;
          case 6 : l_b = 100; u_b = 200;
        }
      for (int k = 1; k <= 6; k++)
      {
        switch (k) 
        {
          case 1 : l_c = -200; u_c = -100;
          case 2 : l_c = -100; u_c = 0;
          case 3 : l_c = -100; u_c = 100;
          case 4 : l_c = 0; u_c = 0;
          case 5 : l_c = 0; u_c = 100;
          case 6 : l_c = 100; u_c = 200; 
        }

        Interval<Rational> ia( l_a, u_a );
        Interval<Rational> ib( l_b, u_b );
        Interval<Rational> ic( l_c, u_c );

        Interval<Rational> iR1 = ib + ic;
        iR1 *= ia;

        Interval<Rational> iR2 = ia * ib;
        Interval<Rational> iR3 = ia * ic;
        iR2 += iR3;

        std::cout << "[" << i << j << k << "] : "<< iR1 << " - " << iR2 << std::endl;

        assert (iR1 == iR2);


      }
    }
  }
  */
} 
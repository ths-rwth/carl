#include "../carl/core/MultivariateHorner.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"

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
	Pol p1 = sp.parseMultivariatePolynomial<Rational>("y*2+y");
  //Pol p1 = sp.parseMultivariatePolynomial<Rational>("2*x^4+8*x^7+5*x^2+2*x+3*y^3+2*y^2+4*z^5+2*z^1+8*z^12");
  //Pol p1 = sp.parseMultivariatePolynomial<Rational>("2184*x^17+15708*z+(-126672)*x^2+643384*z^3+(-2306444)*z^4+4162512*x^13+(-10186920)*z^12+18820800*x^11+(-27118448)*x^10+31123477*x^9+6199788*x^5+(-12956461)*x^6+21524503*x^7+(-28784511)*x^8+(-1226048)*x^14+245224*y^15+(-31192)*y^16+(-924)");


	std::set<Variable> allVarInPolynome;
	p1.gatherVariables(allVarInPolynome);
	std::map<Variable, Interval<Rational>> map;

	for(auto i : allVarInPolynome) {
  	  if (i.getId() == 1)
  	  {
  	  	Interval<Rational> ix (0, 5);
  	  	map[i] = ix;
  	  }
  	  if (i.getId() == 2)
  	  {
  	  	Interval<Rational> iy (0, 2);
  	  	map[i] = iy;
  	  }
  	  if (i.getId() == 3)
  	  {
  	  	Interval<Rational> iz (0, 1);
  	  	map[i] = iz;
  	  }
  	}



	std::cout << "\n Polynom       :" << p1 << std::endl;

	MultivariateHorner< Pol, GREEDY_IIs > peterPolynom (p1,map);
  MultivariateHorner< Pol, GREEDY_Is > peterPolynom2 (p1,map);

	std::cout << "\n Horner        :" << peterPolynom << std::endl;
  std::cout << " Horner2       :" << peterPolynom2 << std::endl;

	Interval<Rational> testInterval = evaluate(peterPolynom , map);
  Interval<Rational> testInterval2 = evaluate(peterPolynom2 , map);

	std::cout << "\n Evaluate Horner :" << testInterval << std::endl;
  std::cout << " Evaluate Horner2:" << testInterval2 << std::endl;


} 
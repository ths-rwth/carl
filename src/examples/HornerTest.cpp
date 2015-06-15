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
		Pol p1 = sp.parseMultivariatePolynomial<Rational>("2184*x^17+15708*x+(-126672)*x^2+643384*x^3+(-2306444)*x^4+4162512*x^13+(-10186920)*x^12+18820800*x^11+(-27118448)*x^10+31123477*x^9+6199788*x^5+(-12956461)*x^6+21524503*x^7+(-28784511)*x^8+(-1226048)*x^14+245224*y^15+(-31192)*y^16+(-924)");
   MultivariateHorner< Pol > peterPolynom (p1);

}
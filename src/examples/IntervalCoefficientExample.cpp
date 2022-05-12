

#include <carl/core/VariablePool.h>
#include <carl/poly/umvpoly/Term.h>
#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/interval/Interval.h>

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

using namespace carl;

int main(int argc, char** argv) {
	
	Variable a = freshRealVariable("a");
	Variable b = freshRealVariable("b");
	Variable c = freshRealVariable("c");
	Variable d = freshRealVariable("d");
    
//	MultivariatePolynomial<Rational> e6({(Term<Rational>)1,(Rational)3*b, (Rational)1*createMonomial(c,2),(Rational)-1*createMonomial(d,3)});
//	MultivariatePolynomial<Interval<double>> e6({(Term<Interval<double>>)(Interval<double>)1,(Interval<double>)3*b, (Interval<double>)1*createMonomial(c,2),(Interval<double>)-1*createMonomial(d,3)});
//	MultivariatePolynomial<Interval<double>> e7({(Interval<double>)1});
}

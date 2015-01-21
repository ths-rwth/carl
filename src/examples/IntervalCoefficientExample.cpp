

#include "carl/core/VariablePool.h"
#include <cln/cln.h>
#include "carl/core/Term.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/interval/Interval.h"

using namespace carl;

int main(int argc, char** argv) {
	
	VariablePool& vpool = VariablePool::getInstance();
    Variable a = vpool.getFreshVariable();
    vpool.setName(a, "a");
    Variable b = vpool.getFreshVariable();
    vpool.setName(b, "b");
    Variable c = vpool.getFreshVariable();
    vpool.setName(c, "c");
    Variable d = vpool.getFreshVariable();
    vpool.setName(d, "d");
	
//	MultivariatePolynomial<cln::cl_RA> e6({(Term<cln::cl_RA>)1,(cln::cl_RA)3*b, (cln::cl_RA)1*createMonomial(c,2),(cln::cl_RA)-1*createMonomial(d,3)});
	MultivariatePolynomial<Interval<double>> e6({(Term<Interval<double>>)1,(Interval<double>)3*b, (Interval<double>)1*createMonomial(c,2),(Interval<double>)-1*createMonomial(d,3)});
//	MultivariatePolynomial<Interval<double>> e7({(Interval<double>)1});
}

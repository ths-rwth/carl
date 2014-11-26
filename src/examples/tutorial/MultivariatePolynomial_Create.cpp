#include <iostream>

#include "carl/core/Variable.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/numbers/numbers.h"

int main() {
	carl::VariablePool& pool = carl::VariablePool::getInstance();
	carl::Variable x = pool.getFreshVariable();
	carl::Variable y = pool.getFreshVariable();

	/*
	 * A carl::MultivariatePolynomial represents a monomial in multiple
	 * variables. It consists of a vector of carl::Term objects that consist of
	 * a carl::Monomial and a coefficient.
     */
	
	carl::MultivariatePolynomial<cln::cl_RA> p(x*y);
	std::cout << p << std::endl;
}

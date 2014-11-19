#include <iostream>

#include "carl/core/Variable.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/numbers/numbers.h"

int main() {
	carl::Variable x = carl::VariablePool::getInstance().getFreshVariable();
	carl::Variable y = carl::VariablePool::getInstance().getFreshVariable();
	carl::MultivariatePolynomial<cln::cl_RA> p(x*y);
	std::cout << p << std::endl;
}

/**
 * @file DiophantineEquations.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../numbers/numbers.h"
#include "MultivariatePolynomial.h"

namespace carl {

template<typename T>
std::vector<T> solveDiophantine(const MultivariatePolynomial<T>& p) {
	static_assert(carl::is_integer<T>::value, "Diophantine equations are build from integral coefficients only!");
	std::vector<T> res;
	// Solve here
	return res;
}

cln::cl_I gcdOfLinearCoefficients(MultivariatePolynomial<cln::cl_I>& p) {
    
}

}
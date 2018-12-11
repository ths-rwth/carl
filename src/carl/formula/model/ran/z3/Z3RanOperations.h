#pragma once

#include "../RealAlgebraicNumber.h"

namespace carl {

template<typename Number>
Z3Ran<Number> sampleBelowZ3(const Z3Ran<Number>& n);

template<typename Number>
Z3Ran<Number> sampleBetweenZ3(const Z3Ran<Number>& lower, const Z3Ran<Number>& upper);

template<typename Number>
Z3Ran<Number> sampleAboveZ3(const Z3Ran<Number>& n);

}
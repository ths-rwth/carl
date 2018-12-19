#pragma once

#include "Z3Ran.h"

#ifdef RAN_USE_Z3

namespace carl {

template<typename Number>
Z3Ran<Number> sampleBelowZ3(const Z3Ran<Number>& n);

template<typename Number>
Z3Ran<Number> sampleBetweenZ3(const Z3Ran<Number>& lower, const Z3Ran<Number>& upper);

template<typename Number>
Z3Ran<Number> sampleAboveZ3(const Z3Ran<Number>& n);

}

#endif
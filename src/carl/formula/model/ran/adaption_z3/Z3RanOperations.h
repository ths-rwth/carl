#pragma once

#include "../RealAlgebraicNumber.h"

namespace carl {

template<typename Number>
RealAlgebraicNumber<Number> sampleBelowZ3(const RealAlgebraicNumber<Number>& n);

template<typename Number>
RealAlgebraicNumber<Number> sampleBetweenZ3(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper);

template<typename Number>
RealAlgebraicNumber<Number> sampleAboveZ3(const RealAlgebraicNumber<Number>& n);

}
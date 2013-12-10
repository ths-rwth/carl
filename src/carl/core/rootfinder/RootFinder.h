/* 
 * File:   RootFinder.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../RealAlgebraicNumber.h"
#include "../UnivariatePolynomial.h"
#include "../../interval/ExactInterval.h"
#include "../Sign.h"
#include "IncrementalRootFinder.h"

namespace carl {
namespace core {

template<typename Number, typename Finder = IncrementalRootFinder<Number>>
std::list<RealAlgebraicNumber<Number>*> realRoots(const UnivariatePolynomial<Number>& polynomial, const ExactInterval<Number>& interval = ExactInterval<Number>()) {
	Finder finder(polynomial, interval);
	while (finder.next() != nullptr);
	return finder.rootCache();
}

template<typename Number, typename Finder = IncrementalRootFinder<Number>>
unsigned int countRealRoots(const UnivariatePolynomial<Number>& polynomial, const ExactInterval<Number>& interval) {
	return realRoots<Number, Finder>(polynomial, interval).size();
}

template<typename Number>
unsigned int countRealRoots(const std::list<UnivariatePolynomial<Number>> sturmSequence, const ExactInterval<Number>& interval) {
	unsigned int l = signVariations(sturmSequence.begin(), sturmSequence.end(), [&interval](const UnivariatePolynomial<Number>& p){ return p.evaluate(interval.left()); });
	unsigned int r = signVariations(sturmSequence.begin(), sturmSequence.end(), [&interval](const UnivariatePolynomial<Number>& p){ return p.evaluate(interval.right()); });
	return l - r;
}

}
}
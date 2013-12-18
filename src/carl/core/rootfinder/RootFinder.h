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
namespace rootfinder {

/// Predefined flags for different real root isolation strategies in RealAlgebraicNumberFactory::realRoots.
enum class IsolationStrategy : unsigned  {
	/// Performs just interval splitting by OpenInterval::midpoint.
	SIMPLE,
	/// Performs interval splitting by OpenInterval::midpoint. If the midpoint happens to be the root itself, it is stored in RealAlgebraicNumberIR::mValue.
	GENERIC,
	/// During the splitting process, the midpoint is checked for being a root first. Then, OpenInterval::sample is checked. If it didn't prove to be a root, it is used to dissect the interval. Otherwise it is stored in RealAlgebraicNumberIR::mValue.
	BINARYSAMPLE,
	/// During the splitting process, the midpoint is checked for being a root first. Then, OpenInterval::sample is checked. If it didn't prove to be a root, both the midpoint and the sample point are used to split the interval.
	TERNARYSAMPLE,
	/// During the splitting process, the midpoint is checked for being a root first. Then, OpenInterval::sample is checked. If it didn't prove to be a root, both the midpoint and the sample point are used to split the interval.
	TERNARYNEWTON,
	DEFAULT = TERNARYSAMPLE
};

template<typename Number>
using evalmap = std::map<Variable, RealAlgebraicNumberIR<Number>*, Less<Number>()>;
template<typename Number>
using evalintervalmap = std::map<Variable, ExactInterval<Number>>;

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
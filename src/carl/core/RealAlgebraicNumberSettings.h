/* 
 * File:   RealAlgebraicNumberSettings.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {
namespace RealAlgebraicNumberSettings {

/// Predefined flags for different refinement strategies in RealAlgebraicNumberIR::refine.
enum class RefinementStrategy : unsigned {
	/// Performs interval splitting by midpoint. If the midpoint happens to be the root itself, it is stored in RealAlgebraicNumberIR::mValue.
	GENERIC,
	/// sample is checked for being a root. If not, it is used to dissect the interval. Otherwise it is stored in RealAlgebraicNumberIR::mValue.
	BINARYSAMPLE,
	/// Newton's iteration is applied for finding the a root first. If no root was found, the value is used to dissect the interval.
	BINARYNEWTON,
	DEFAULT = GENERIC
};

/// Maximum number of refinements in which the sample() value should be computed for splitting. Otherwise the midpoint is taken.
static const unsigned MAXREFINE = 8;

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

/// Maximum bound of an isolating interval so that the OpenInterval::sample method is used for splitting point selection.
static const long MAX_FASTSAMPLE_BOUND = SHRT_MAX;
/// Maximum denominator for the sample search is bounded to the square of the common denominator of the bounds; anything above that value is disregarded and a maybe non-optimal, intermediate value is returned instead
static const bool MAX_SAMPLE_DENOMINATOR_BOUNDED = true;

}
}
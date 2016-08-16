/**
 * @file:   RealAlgebraicNumberSettings.h
 * @author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {
namespace RealAlgebraicNumberSettings {

/// Predefined flags for different refinement strategies in RealAlgebraicNumberIR::refine.
enum class RefinementStrategy {
	/// Performs interval splitting by midpoint. If the midpoint happens to be the root itself, it is stored in RealAlgebraicNumberIR::mValue.
	GENERIC,
	/// sample is checked for being a root. If not, it is used to dissect the interval. Otherwise it is stored in RealAlgebraicNumberIR::mValue.
	BINARYSAMPLE,
	/// Newton's iteration is applied for finding the a root first. If no root was found, the value is used to dissect the interval.
	BINARYNEWTON,
	DEFAULT = BINARYSAMPLE
};

/// Maximum number of refinements in which the sample() value should be computed for splitting. Otherwise the midpoint is taken.
static const std::size_t MAXREFINE = 8;

/// Maximum bound of an isolating interval so that the OpenInterval::sample method is used for splitting point selection.
static const std::size_t MAX_FASTSAMPLE_BOUND = SHRT_MAX;
/// Maximum denominator for the sample search is bounded to the square of the common denominator of the bounds; anything above that value is disregarded and a maybe non-optimal, intermediate value is returned instead
static const bool MAX_SAMPLE_DENOMINATOR_BOUNDED = true;

}
}

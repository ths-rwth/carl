/**
 * @file:   RealAlgebraicNumberOperations.h
 * @author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * This file should never be included directly but only via RealAlgebraicNumber.h
 */

#pragma once

#include "../../../numbers/numbers.h"
#include "RealAlgebraicNumber.h"

namespace carl {
	template<typename Number>
	inline bool isZero(const RealAlgebraicNumber<Number>& n) {
		return n.isZero();
	}

	template<typename Number>
	inline bool isInteger(const RealAlgebraicNumber<Number>& n) {
		return n.isIntegral();
	}

	template<typename Number>
	inline typename IntegralType<Number>::type floor(const RealAlgebraicNumber<Number>& n) {
		n.refineToIntegrality();
		return floor_fast(n);
	}

	template<typename Number>
	inline typename IntegralType<Number>::type floor_fast(const RealAlgebraicNumber<Number>& n) {
		if (n.isNumeric()) return carl::floor(n.value());
		else return carl::floor(n.lower());
	}
	
	template<typename Number>
	inline typename IntegralType<Number>::type ceil(const RealAlgebraicNumber<Number>& n) {
		n.refineToIntegrality();
		return ceil_fast(n);
	}
	
	template<typename Number>
	inline typename IntegralType<Number>::type ceil_fast(const RealAlgebraicNumber<Number>& n) {
		if (n.isNumeric()) return carl::ceil(n.value());
		else return carl::ceil(n.upper());
	}

namespace ran {
	template<typename T1, typename T2>
	T1 sample_between(T1& lower, T2& upper) {
		CARL_LOG_ERROR("carl.ran", "Unsupported operation " << lower << " < " << upper);
		CARL_LOG_ERROR("carl.ran", "Types: " << typeid(T1).name() << " and " << typeid(T2).name());
		return lower;
	}
}

	template<typename Number>
	RealAlgebraicNumber<Number> sampleBelow(const RealAlgebraicNumber<Number>& n) {
		return std::visit(
			[](const auto& c){ return RealAlgebraicNumber<Number>(ran::sample_below(c), false); }
			, n.content()
		);
	}
	template<typename Number>
	RealAlgebraicNumber<Number> sampleBetween(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper, RANSampleHeuristic heuristic = RANSampleHeuristic::Default) {
		return std::visit(
			[](auto& l, auto& u){ return RealAlgebraicNumber<Number>(ran::sample_between(l, u), false); }
			, lower.content(), upper.content()
		);
		//CARL_LOG_FUNC("carl.ran", lower << ", " << upper);
		//if(lower.isThom() || upper.isThom()) {
		//	RealAlgebraicNumber<Number> res;
		//	if(lower.isThom() && upper.isThom()) {
		//		res =  RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(*(lower.mTE), *(upper.mTE)), false);
		//	} else if(lower.isNumeric() && upper.isThom()) {
		//		res = RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(lower.value(), *(upper.mTE)), false);
		//	} else {
		//		assert(lower.isThom() && upper.isNumeric());
		//		res = RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(*(lower.mTE), upper.value()), false);
		//	}
		//	CARL_LOG_TRACE("carl.ran", "selecting sample " << res);
		//	return res;
		//} else if (lower.isZ3Ran() || upper.isZ3Ran()) {
		//	auto res = sampleBetweenZ3(lower, upper);
		//	CARL_LOG_TRACE("carl.ran", "selecting sample " << res << " between " << lower << " and " << upper);
		//	return res;
		//} else if (lower.isNumeric() || upper.isNumeric() || lower.isInterval() || upper.isInterval()) {
		//	Interval<Number> i;
		//	if (lower.isNumeric()) i.set(lower.value(), lower.value());
		//	else i.set(lower.upper(), lower.upper());
		//	if (upper.isNumeric()) i.setUpper(upper.value());
		//	else i.setUpper(upper.lower());
		//	while (i.isEmpty()) {
		//		if (!lower.isNumeric()) {
		//			lower.refine();
		//			if (lower.isNumeric()) i.setLower(lower.value());
		//			else i.setLower(lower.upper());
		//		}
		//		if (!upper.isNumeric()) {
		//			upper.refine();
		//			if (upper.isNumeric()) i.setUpper(upper.value());
		//			else i.setUpper(upper.lower());
		//		}
		//	}
		//	CARL_LOG_TRACE("carl.ran", "Selecting from (" << lower << ", " << upper << ") -> " << sample(i, false) << " (from " << i << ")");
		//	switch (heuristic) {
		//		case RANSampleHeuristic::Center:
		//			return RealAlgebraicNumber<Number>(center(i), false);
		//		case RANSampleHeuristic::CenterInt:
		//			return RealAlgebraicNumber<Number>(sample(i, false), false);
		//		case RANSampleHeuristic::LeftInt:
		//			return RealAlgebraicNumber<Number>(sample_left(i), false);
		//		case RANSampleHeuristic::RightInt:
		//			return RealAlgebraicNumber<Number>(sample_right(i), false);
		//		case RANSampleHeuristic::ZeroInt:
		//			return RealAlgebraicNumber<Number>(sample_zero(i), false);
		//		case RANSampleHeuristic::InftyInt:
		//			return RealAlgebraicNumber<Number>(sample_infty(i), false);
		//		default:
		//			CARL_LOG_WARN("carl.ran", "Using unknown sampling heuristic " << heuristic << ", fallback to sample.");
		//			return RealAlgebraicNumber<Number>(sample(i, false), false);
		//	}
		//}
		//assert(false);
	}
	template<typename Number>
	RealAlgebraicNumber<Number> sampleAbove(const RealAlgebraicNumber<Number>& n) {
		return std::visit(
			[](const auto& c){ return RealAlgebraicNumber<Number>(ran::sample_above(c), false); }
			, n.content()
		);
	}

}

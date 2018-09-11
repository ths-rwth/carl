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
	
	template<typename Number>
	RealAlgebraicNumber<Number> sampleBelow(const RealAlgebraicNumber<Number>& n) {
		CARL_LOG_FUNC("carl.ran", n);
		if (n.isNumeric()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (-oo, " << n << ") -> " << (carl::ceil(n.value()) - 1));
			return RealAlgebraicNumber<Number>(carl::ceil(n.value()) - 1, false);
		} else if (n.isInterval()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (-oo, " << n << ") -> " << (carl::ceil(n.lower()) - 1));
			return RealAlgebraicNumber<Number>(carl::ceil(n.lower()) - 1, false);
		} else {
			assert(n.isThom());
			RealAlgebraicNumber<Number> res(*(n.mTE) + Number(-1), false);
			CARL_LOG_TRACE("carl.ran", "selecting sample " << res);
			return res;
		}
	}
	template<typename Number>
	RealAlgebraicNumber<Number> sampleBetween(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper, RANSampleHeuristic heuristic = RANSampleHeuristic::Default) {
		CARL_LOG_FUNC("carl.ran", lower << ", " << upper);
		if(lower.isThom() || upper.isThom()) {
			RealAlgebraicNumber<Number> res;
			if(lower.isThom() && upper.isThom()) {
				res =  RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(*(lower.mTE), *(upper.mTE)), false);
			} else if(lower.isNumeric() && upper.isThom()) {
				res = RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(lower.value(), *(upper.mTE)), false);
			} else {
				assert(lower.isThom() && upper.isNumeric());
				res = RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(*(lower.mTE), upper.value()), false);
			}
			CARL_LOG_TRACE("carl.ran", "selecting sample " << res);
			return res;
		} else {
			Interval<Number> i;
			if (lower.isNumeric()) i.set(lower.value(), lower.value());
			else i.set(lower.upper(), lower.upper());
			if (upper.isNumeric()) i.setUpper(upper.value());
			else i.setUpper(upper.lower());
			while (i.isEmpty()) {
				if (!lower.isNumeric()) {
					lower.refine();
					if (lower.isNumeric()) i.setLower(lower.value());
					else i.setLower(lower.upper());
				}
				if (!upper.isNumeric()) {
					upper.refine();
					if (upper.isNumeric()) i.setUpper(upper.value());
					else i.setUpper(upper.lower());
				}
			}
			CARL_LOG_TRACE("carl.ran", "Selecting from (" << lower << ", " << upper << ") -> " << i.sample(false) << " (from " << i << ")");
			switch (heuristic) {
				case RANSampleHeuristic::Center:
					return RealAlgebraicNumber<Number>(i.center(), false);
				case RANSampleHeuristic::CenterInt:
					return RealAlgebraicNumber<Number>(i.sample(false), false);
				case RANSampleHeuristic::LeftInt:
					return RealAlgebraicNumber<Number>(i.sampleLeft(), false);
				case RANSampleHeuristic::RightInt:
					return RealAlgebraicNumber<Number>(i.sampleRight(), false);
				case RANSampleHeuristic::ZeroInt:
					return RealAlgebraicNumber<Number>(i.sampleZero(), false);
				case RANSampleHeuristic::InftyInt:
					return RealAlgebraicNumber<Number>(i.sampleInfty(), false);
				default:
					CARL_LOG_WARN("carl.ran", "Using unknown sampling heuristic " << heuristic << ", fallback to sample.");
					return RealAlgebraicNumber<Number>(i.sample(false), false);
			}
		}
	}
	template<typename Number>
	RealAlgebraicNumber<Number> sampleAbove(const RealAlgebraicNumber<Number>& n) {
		CARL_LOG_FUNC("carl.ran", n);
		if (n.isNumeric()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (" << n << ", oo) -> " << (carl::floor(n.value()) + 1));
			return RealAlgebraicNumber<Number>(carl::floor(n.value()) + 1, false);
		} else if (n.isInterval()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (" << n << ", oo) -> " << (carl::floor(n.upper()) + 1));
			return RealAlgebraicNumber<Number>(carl::floor(n.upper()) + 1, false);
		} else {
			assert(n.isThom());
			RealAlgebraicNumber<Number> res(*(n.mTE) + Number(1), false);
			CARL_LOG_TRACE("carl.ran", "selecting sample " << res);
			return res;
		}
	}

}

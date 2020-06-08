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
		return std::visit(
			[](const auto& c){ return RealAlgebraicNumber<Number>(ran::floor(c)); }
			, n.content()
		);
	}
	
	template<typename Number>
	inline typename IntegralType<Number>::type ceil(const RealAlgebraicNumber<Number>& n) {
		return std::visit(
			[](const auto& c){ return RealAlgebraicNumber<Number>(ran::ceil(c)); }
			, n.content()
		);
	}
	
	template<typename Number>
	RealAlgebraicNumber<Number> sampleBelow(const RealAlgebraicNumber<Number>& n) {
		return std::visit(
			[](const auto& c){ return RealAlgebraicNumber<Number>(ran::sample_below(c)); }
			, n.content()
		);
	}
	template<typename Number>
	RealAlgebraicNumber<Number> sampleBetween(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper, RANSampleHeuristic heuristic = RANSampleHeuristic::Default) {
		return std::visit(
			[](auto& l, auto& u){ return RealAlgebraicNumber<Number>(ran::sample_between(l, u)); }
			, lower.mContent, upper.mContent
		);
	}
	template<typename Number>
	RealAlgebraicNumber<Number> sampleAbove(const RealAlgebraicNumber<Number>& n) {
		return std::visit(
			[](const auto& c){ return RealAlgebraicNumber<Number>(ran::sample_above(c)); }
			, n.content()
		);
	}

}

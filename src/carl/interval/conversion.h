#pragma once

#include "Interval.h"
#include "../numbers/numbers.h"

namespace carl {

    template<typename From, typename To, carl::DisableIf< std::is_same<From,To> > = dummy >
	inline Interval<To> convert(const Interval<From>& i) {
        return Interval<To>(carl::convert<From,To>(i.lower()), i.lowerBoundType(), carl::convert<From,To>(i.upper()), i.upperBoundType());
    }

	template<typename From, typename To, carl::EnableIf< std::is_same<From,To> > = dummy >
	inline Interval<To> convert(const Interval<From>& i) {
		return i;
	}

} // namespace

#pragma once

#include "Interval.h"
#include <carl-arith/numbers/numbers.h>

namespace carl {

    template<typename From, typename To, carl::DisableIf< std::is_same<From,To> > = dummy >
	inline Interval<To> convert(const Interval<From>& i) {
        return Interval<To>(carl::convert<From,To>(i.lower()), i.lower_bound_type(), carl::convert<From,To>(i.upper()), i.upper_bound_type());
    }

	template<typename From, typename To, carl::EnableIf< std::is_same<From,To> > = dummy >
	inline Interval<To> convert(const Interval<From>& i) {
		return i;
	}

} // namespace

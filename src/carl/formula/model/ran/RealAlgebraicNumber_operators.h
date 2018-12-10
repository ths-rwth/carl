#pragma once

namespace carl {
namespace ran {

template<typename Number>
struct equal {
	using NumberContent = typename RealAlgebraicNumber<Number>::NumberContent;
	using IntervalContent = typename RealAlgebraicNumber<Number>::IntervalContent;
	using ThomContent = typename RealAlgebraicNumber<Number>::ThomContent;
	using Z3Content = typename RealAlgebraicNumber<Number>::Z3Content;

	template<typename T>
	bool merge_if_identical(T& lhs, T& rhs, bool identical) const {
		if (identical) rhs = lhs;
		return identical;
	}

	template<typename T1, typename T2>
	bool operator()(const T1& lhs, const T2& rhs) const {
		CARL_LOG_ERROR("carl.ran", "Unsupported comparison " << lhs << " == " << rhs);
		CARL_LOG_ERROR("carl.ran", "Types: " << typeid(T1).name() << " and " << typeid(T2).name());
		CARL_LOG_ERROR("carl.ran", "Interval: " << typeid(IntervalContent).name());
		return false;
	}

	bool operator()(const NumberContent& lhs, const NumberContent& rhs) const {
		return lhs.value == rhs.value;
	}
	bool operator()(const NumberContent& lhs, IntervalContent& rhs) const {
		return rhs.refineAvoiding(lhs.value);
	}
	bool operator()(const NumberContent& lhs, const ThomContent& rhs) const {
		return rhs == lhs.value;
	}
	bool operator()(const NumberContent& lhs, const Z3Content& rhs) const {
		return rhs->equal(lhs.value);
	}

	bool operator()(const IntervalContent& lhs, const NumberContent& rhs) const {
		return (*this)(rhs, lhs);
	}
	bool operator()(const ThomContent& lhs, const NumberContent& rhs) const {
		return (*this)(rhs, lhs);
	}
	bool operator()(const Z3Content& lhs, const NumberContent& rhs) const {
		return (*this)(rhs, lhs);
	}

	bool operator()(IntervalContent& lhs, IntervalContent& rhs) const {
		return merge_if_identical(lhs, rhs, lhs == rhs);
	}
	bool operator()(IntervalContent& lhs, Z3Content& rhs) const {
		carl::ran::IntervalContent tmp(rhs->getPolynomial(), rhs->getInterval());
		return lhs == tmp;
	}
	bool operator()(Z3Content& lhs, IntervalContent& rhs) const {
		return (*this)(rhs, lhs);
	}


	bool operator()(ThomContent& lhs, ThomContent& rhs) const {
		return merge_if_identical(lhs, rhs, lhs == rhs);
	}

	bool operator()(Z3Content& lhs, Z3Content& rhs) const {
		if (lhs.get() == rhs.get()) return true;
		return merge_if_identical(
			lhs, rhs, lhs->equal(*rhs)
		);
	}

	bool operator()(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) const {
		CARL_LOG_DEBUG("carl.ran", "Comparing " << lhs << " and " << rhs);
		return std::visit(
			[this](auto& l, auto& r) {
				return (*this)(l, r);
			},
			lhs.mContent,
			rhs.mContent
		);
	}
};


template<typename Number>
struct less {
	using NumberContent = typename RealAlgebraicNumber<Number>::NumberContent;
	using IntervalContent = typename RealAlgebraicNumber<Number>::IntervalContent;
	using ThomContent = typename RealAlgebraicNumber<Number>::ThomContent;
	using Z3Content = typename RealAlgebraicNumber<Number>::Z3Content;

	template<typename T1, typename T2>
	bool operator()(const T1& lhs, const T2& rhs) const {
		CARL_LOG_ERROR("carl.ran", "Unsupported comparison " << lhs << " < " << rhs);
		CARL_LOG_ERROR("carl.ran", "Types: " << typeid(T1).name() << " and " << typeid(T2).name());
		CARL_LOG_ERROR("carl.ran", "Interval: " << typeid(IntervalContent).name());
		return false;
	}

	bool operator()(const NumberContent& lhs, const NumberContent& rhs) const {
		return lhs.value < rhs.value;
	}
	bool operator()(const NumberContent& lhs, IntervalContent& rhs) const {
		if (rhs.refineAvoiding(lhs.value)) return false;
		return lhs.value < rhs.interval().lower();
	}
	bool operator()(const NumberContent& lhs, const ThomContent& rhs) const {
		return lhs.value < rhs;
	}
	bool operator()(const NumberContent& lhs, const Z3Content& rhs) const {
		return rhs->greater(lhs.value);
	}

	bool operator()(IntervalContent& lhs, const NumberContent& rhs) const {
		if (lhs.refineAvoiding(rhs.value)) return false;
		return lhs.interval().upper() < rhs.value;
	}
	bool operator()(const ThomContent& lhs, const NumberContent& rhs) const {
		return lhs < rhs.value;
	}
	bool operator()(const Z3Content& lhs, const NumberContent& rhs) const {
		return lhs->less(rhs.value);
	}

	bool operator()(IntervalContent& lhs, IntervalContent& rhs) const {
		return lhs < rhs;
	}
	bool operator()(IntervalContent& lhs, Z3Content& rhs) const {
		carl::ran::IntervalContent tmp(rhs->getPolynomial(), rhs->getInterval());
		return (*this)(lhs, tmp);
	}
	bool operator()(Z3Content& lhs, IntervalContent& rhs) const {
		carl::ran::IntervalContent tmp(lhs->getPolynomial(), lhs->getInterval());
		return (*this)(tmp, rhs);
	}


	bool operator()(ThomContent& lhs, ThomContent& rhs) const {
		return lhs < rhs;
	}

	bool operator()(Z3Content& lhs, Z3Content& rhs) const {
		if (lhs.get() == rhs.get()) return false;
		return lhs->less(*rhs);
	}

	bool operator()(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) const {
		CARL_LOG_DEBUG("carl.ran", "Comparing " << lhs << " and " << rhs);
		return std::visit(
			[this](auto& l, auto& r) {
				return (*this)(l, r);
			},
			lhs.mContent,
			rhs.mContent
		);
	}
};

}

template<typename Number>
inline bool operator==(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	if (&lhs == &rhs) return true;
	return ran::equal<Number>()(lhs, rhs);
}
template<typename Number>
inline bool operator!=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return !(lhs == rhs);
}
template<typename Number>
inline bool operator<(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	if (&lhs == &rhs) return false;
	return ran::less<Number>()(lhs, rhs);
}
template<typename Number>
inline bool operator<=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return !(rhs < lhs);
}
template<typename Number>
inline bool operator>(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return rhs < lhs;
}
template<typename Number>
inline bool operator>=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return rhs <= lhs;
}
	
} // carl

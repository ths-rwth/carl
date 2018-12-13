#pragma once

namespace carl {

template<typename Number>
inline bool operator==(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	if (&lhs == &rhs) return true;
	return std::visit(
		[](auto& l, auto& r) { return l == r; },
		lhs.mContent,
		rhs.mContent
	);
}
template<typename Number>
inline bool operator!=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return !(lhs == rhs);
}
template<typename Number>
inline bool operator<(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	if (&lhs == &rhs) return false;
	return std::visit(
		[](auto& l, auto& r) { return l < r; },
		lhs.mContent,
		rhs.mContent
	);
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

namespace ran {

template<typename T1, typename T2>
void unsupported_operation(const std::string& op, const T1& lhs, const T2& rhs) {
	CARL_LOG_ERROR("carl.ran", "Unsupported comparison " << lhs << " " << op << " " << rhs);
	CARL_LOG_ERROR("carl.ran", "Types: " << typeid(T1).name() << " and " << typeid(T2).name());
	assert(false);
}

template<typename T1, typename T2>
T1 sample_between(const T1& lower, const T2& upper) {
	unsupported_operation("sample_between", lower, upper);
	return lower;
}

template<typename T1, typename T2>
inline bool operator==(const T1& lhs, const T2& rhs) {
	unsupported_operation("==", lhs, rhs);
	return false;
}

template<typename T1, typename T2>
inline bool operator<(const T1& lhs, const T2& rhs) {
	unsupported_operation("==", lhs, rhs);
	return false;
}

template<typename Number, typename Content>
const Interval<Number>& lower(const Content& n) {
	return get_interval(n).lower();
}

template<typename Number, typename Content>
const Interval<Number>& upper(const Content& n) {
	return get_interval(n).upper();
}

}

} // carl

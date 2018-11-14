#pragma once

#include "Interval.h"

#include <cassert>

namespace carl {

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> sin(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::sin(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void sin_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::sin(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> cos(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::cos(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void cos_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::cos(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> tan(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::tan(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void tan_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::tan(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> asin(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::asin(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void asin_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::asin(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> acos(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::acos(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void acos_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::acos(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> atan(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::atan(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void atan_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::atan(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> sinh(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::sinh(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void sinh_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::sinh(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> cosh(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::cosh(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void cosh_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::cosh(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> tanh(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::tanh(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void tanh_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::tanh(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> asinh(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::asinh(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void asinh_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::asinh(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> acosh(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::acosh(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void acosh_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::acosh(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> atanh(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::atanh(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void atanh_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::atanh(i.rContent()));
}

}
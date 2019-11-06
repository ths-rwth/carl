#pragma once

#include "../../../core/polynomialfunctions/Substitution.h"

namespace carl {
namespace ran {

template<typename Number>
struct NumberContent {
	Number mContent = carl::constant_zero<Number>::get();

public:

	NumberContent() {}
	NumberContent(const Number& n): mContent(n) {}

	const auto& value() const {
		return mContent;
	}

	std::size_t size() const {
		return carl::bitsize(value());
	}

	bool is_integral() const {
		return carl::isInteger(value());
	}
	bool is_zero() const {
		return carl::isZero(value());
	}
	bool contained_in(const Interval<Number>& i) const {
		return i.contains(value());
	}

	Number integer_below() const {
		return carl::floor(value());
	}
	Sign sgn() const {
		return carl::sgn(value());
	}
	Sign sgn(const UnivariatePolynomial<Number>& p) const {
		return carl::sgn(p.evaluate(value()));
	}
};

template<typename Number>
Number abs(const NumberContent<Number>& n) {
	return carl::abs(n.value());
}

template<typename Number>
const Number& branching_point(const NumberContent<Number>& n) {
	return n.value();
}

template<typename Number>
NumberContent<Number> evaluate(const MultivariatePolynomial<Number>& p, const std::map<Variable, NumberContent<Number>>& m) {
	MultivariatePolynomial<Number> res = p;
	for (const auto& e: m) {
		carl::substitute_inplace(res, e.first, MultivariatePolynomial<Number>(e.second.value()));
	}
	assert(res.isConstant());
	return res.constantPart();
}

template<typename Number, typename Poly>
bool evaluate(const Constraint<Poly>& c, const std::map<Variable, NumberContent<Number>>& m) {
	auto res = evaluate(c.lhs(), m);
	return evaluate(res.value(), c.relation());
}

template<typename Number>
Interval<Number> get_interval(const NumberContent<Number>& n) {
	return Interval<Number>(n.value());
}

template<typename Number>
const Number& get_number(const NumberContent<Number>& n) {
	return n.value();
}

template<typename Number>
UnivariatePolynomial<Number> get_polynomial(const NumberContent<Number>& n) {
	return UnivariatePolynomial<Number>(Variable::NO_VARIABLE);
}

template<typename Number>
bool is_number(const NumberContent<Number>&) {
	return true;
}

template<typename Number>
Number sample_above(const NumberContent<Number>& n) {
	return carl::floor(n.value()) + 1;
}
template<typename Number>
Number sample_below(const NumberContent<Number>& n) {
	return carl::ceil(n.value()) - 1;
}
template<typename Number>
Number sample_between(const NumberContent<Number>& lower, const NumberContent<Number>& upper) {
	return sample(Interval<Number>(lower.value(), upper.value()), false);
}

template<typename Number>
inline bool operator==(const NumberContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return lhs.value() == rhs.value();
}

template<typename Number>
inline bool operator<(const NumberContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return lhs.value() < rhs.value();
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const NumberContent<Num>& ran) {
	return os << "NR " << ran.value();
}

}
}
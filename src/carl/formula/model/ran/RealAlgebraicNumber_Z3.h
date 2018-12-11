#pragma once

#include "adaption_z3/Z3Ran.h"
#include "adaption_z3/Z3RanOperations.h"

#include <memory>

namespace carl {
namespace ran {

template<typename Number>
struct Z3Content {
	template<typename Num>
	friend bool operator==(const Z3Content<Num>& lhs, const Z3Content<Num>& rhs);
	template<typename Num>
	friend bool operator<(const Z3Content<Num>& lhs, const Z3Content<Num>& rhs);
private:
	struct Content {
		Z3Ran<Number> ran;

		Content(const Z3Ran<Number>& z):
			ran(z)
		{}
	};
	std::shared_ptr<Content> mContent;
public:
	Z3Content(const Z3Ran<Number>& ran):
		mContent(std::make_shared<Content>(ran))
	{}

	auto& z3_ran() {
		return mContent->ran;
	}
	const auto& z3_ran() const {
		return mContent->ran;
	}

	const auto& polynomial() const {
		return z3_ran().getPolynomial();
	}

	const auto& interval() const {
		return z3_ran().getInterval();
	}

	std::size_t size() const {
		return 0;
	}
	bool is_integral() const {
		return z3_ran().is_integral();
	}
	bool is_zero() const {
		return z3_ran().is_zero();
	}
	bool contained_in(const Interval<Number>& i) const {
		return z3_ran().containedIn(i);
	}

	Number integer_below() const {
		return z3_ran().integer_below();
	}
	Sign sgn() const {
		return z3_ran().sgn();
	}
	Sign sgn(const UnivariatePolynomial<Number>& p) const {
		return z3_ran().sgn(p);
	}
};

template<typename Number>
Z3Content<Number> evaluate(const MultivariatePolynomial<Number>& p, const std::map<Variable, Z3Content<Number>>& m) {
	return evaluateZ3(p, m);
}

template<typename Number>
Number get_number(const Z3Content<Number>& n) {
	return n.z3_ran().get_number();
}

template<typename Number>
bool is_integral(const Z3Content<Number>& n) {
	return n.z3_ran().is_integral();
}
template<typename Number>
bool is_number(const Z3Content<Number>& n) {
	return n.z3_ran().is_number();
}
template<typename Number>
bool is_zero(const Z3Content<Number>& n) {
	return n.z3_ran().is_zero();
}

template<typename Number>
Z3Content<Number> sample_above(const Z3Content<Number>& n) {
	return sampleAboveZ3(n.z3_ran());
}
template<typename Number>
Z3Content<Number> sample_below(const Z3Content<Number>& n) {
	return sampleBelowZ3(n.z3_ran());
}
template<typename Number>
Z3Content<Number> sample_between(const Z3Content<Number>& lower, const Z3Content<Number>& upper) {
	return sampleBetweenZ3(lower.z3_ran(), upper.z3_ran());
}
template<typename Number>
Z3Content<Number> sample_between(const Z3Content<Number>& lower, const NumberContent<Number>& upper) {
	return sampleBetweenZ3(lower.z3_ran(), Z3Ran<Number>(upper.value()));
}
template<typename Number>
Z3Content<Number> sample_between(const NumberContent<Number>& lower, const Z3Content<Number>& upper) {
	return sampleBetweenZ3(Z3Ran<Number>(lower.value()), upper.z3_ran());
}

template<typename Number>
Sign sgn(const Z3Content<Number>& n) {
	return n.z3_ran().sgn();
}
template<typename Number>
Sign sgn(const Z3Content<Number>& n, const UnivariatePolynomial<Number>& p) {
	return n.z3_ran().sgn(p);
}

template<typename Number>
bool operator==(const Z3Content<Number>& lhs, const Z3Content<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return true;
	return lhs.z3_ran().equal(rhs.z3_ran());
}

template<typename Number>
bool operator==(const Z3Content<Number>& lhs, const Number& rhs) {
	return lhs.z3_ran().equal(rhs);
}

template<typename Number>
bool operator<(const Z3Content<Number>& lhs, const Z3Content<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return false;
	return lhs.z3_ran().less(rhs.z3_ran());
}

template<typename Number>
bool operator<(const Z3Content<Number>& lhs, const Number& rhs) {
	return lhs.z3_ran().less(rhs);
}
template<typename Number>
bool operator<(const Number& lhs, const Z3Content<Number>& rhs) {
	return rhs.z3_ran().greater(lhs);
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const Z3Content<Num>& rhs) {
	return os << rhs.z3_ran();
}

}
}
#pragma once

#include <carl/config.h>

#ifdef RAN_USE_Z3

#include "Z3Ran.h"

#include "../ran_operations.h"
#include "../ran_operations_number.h"

#include <memory>

namespace carl {

template<typename Number>
class real_algebraic_number_z3 {

private:
	struct Content {
		Z3Ran<Number> ran;

		Content(const Z3Ran<Number>& z):
			ran(z)
		{}
	};
	
	mutable std::shared_ptr<Content> mContent;

	void set_content(const real_algebraic_number_z3<Number>& other) const {
		mContent = other.mContent;
	}

public: // should be private, but would be nasty

	auto& z3_ran() {
		return mContent->ran;
	}
	const auto& z3_ran() const {
		return mContent->ran;
	}

public:
	real_algebraic_number_z3(const Z3Ran<Number>& ran):
		mContent(std::make_shared<Content>(ran))
	{}

	real_algebraic_number_z3(const Number& number):
		mContent(std::make_shared<Content>(Z3Ran<Number>(number)))
	{}

	real_algebraic_number_z3(): real_algebraic_number_z3(0)
	{}

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
	bool is_numeric() const {
		return z3_ran().is_numeric();
	}
	Number value() const {
		return z3_ran().value();
	}
	bool contained_in(const Interval<Number>& i) const {
		return z3_ran().containedIn(i);
	}

	Number integer_below() const {
		if (is_numeric()) {
			return carl::floor(value());
		} else {
			return carl::floor(z3_ran().lower());
		}
	}
	Sign sgn() const {
		return z3_ran().sgn();
	}
	Sign sgn(const UnivariatePolynomial<Number>& p) const {
		return z3_ran().sgn(p);
	}
	real_algebraic_number_z3<Number> abs() const {
		return z3_ran().abs();
	}

	bool equal(const real_algebraic_number_z3<Number>& rhs) const {
		if (mContent.get() == rhs.mContent.get()) {
			return true;
		} else if (z3_ran().equal(rhs.z3_ran())) {
			set_content(rhs);
			return true;
		} else {
			return false;
		}
	}

	bool less(const real_algebraic_number_z3<Number>& rhs) const {
		if (mContent.get() == rhs.mContent.get()) return false;
		return z3_ran().less(rhs.z3_ran());
	}
};

template<typename Number>
Number branching_point(const real_algebraic_number_z3<Number>& n) {
	return n.z3_ran().branchingPoint();
}

template<typename Number>
real_algebraic_number_z3<Number> sample_above(const real_algebraic_number_z3<Number>& n);
template<typename Number>
real_algebraic_number_z3<Number> sample_below(const real_algebraic_number_z3<Number>& n);
template<typename Number>
real_algebraic_number_z3<Number> sample_between(const real_algebraic_number_z3<Number>& lower, const real_algebraic_number_z3<Number>& upper);
template<typename Number>
real_algebraic_number_z3<Number> sample_between(const real_algebraic_number_z3<Number>& lower, const Number& upper) {
	return sample_between(lower.z3_ran(), real_algebraic_number_z3<Number>(upper));
}
template<typename Number>
real_algebraic_number_z3<Number> sample_between(const Number& lower, const real_algebraic_number_z3<Number>& upper) {
	return sample_between(real_algebraic_number_z3<Number>(lower), upper.z3_ran());
}

template<typename Number>
Number is_root_of(const UnivariatePolynomial<Number>& p, const real_algebraic_number_z3<Number>& value) {
	return value.sgn(p) == Sign::ZERO;
}

template<typename Number>
Number floor(const real_algebraic_number_z3<Number>& n) {
	return carl::floor(n.interval().lower());
}
template<typename Number>
Number ceil(const real_algebraic_number_z3<Number>& n) {
	return carl::ceil(n.interval().upper());
}

template<typename Number>
bool compare(const real_algebraic_number_z3<Number>& lhs, const real_algebraic_number_z3<Number>& rhs, Relation rel) {
	switch (rel) {
		case Relation::EQ:
			return lhs.equal(rhs);
		case Relation::NEQ:
			return !lhs.equal(rhs);
		case Relation::LEQ:
			return lhs.equal(rhs) || lhs.less(rhs);
		case Relation::GEQ:
			return lhs.equal(rhs) || rhs.less(lhs);
		case Relation::LESS:
			return lhs.less(rhs);
		case Relation::GREATER:
			return rhs.less(lhs);		
		default:
			assert(false);
			return false;
	}
}

template<typename Number>
bool compare(const real_algebraic_number_z3<Number>& lhs, const Number& rhs, Relation rel) {
	switch (rel) {
		case Relation::EQ:
			return lhs.z3_ran().equal(rhs);
		case Relation::NEQ:
			return !lhs.z3_ran().equal(rhs);
		case Relation::LEQ:
			return lhs.z3_ran().equal(rhs) || lhs.z3_ran().less(rhs);
		case Relation::GEQ:
			return lhs.z3_ran().equal(rhs) || lhs.z3_ran().greater(rhs);
		case Relation::LESS:
			return lhs.z3_ran().less(rhs);
		case Relation::GREATER:
			return lhs.z3_ran().greater(rhs);		
		default:
			assert(false);
			return false;
	}
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const real_algebraic_number_z3<Num>& rhs) {
	return os << "(" << rhs.z3_ran() << ")"; 
}

template<typename Number>
struct is_ran<real_algebraic_number_z3<Number>> : std::true_type {};

}

#endif
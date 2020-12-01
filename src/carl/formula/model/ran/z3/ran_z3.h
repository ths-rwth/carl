#pragma once

#include <carl/config.h>

#ifdef RAN_USE_Z3

#include "../ran_operations.h"
#include "../ran_operations_number.h"

#include <memory>

namespace carl {

class Z3RanContent;

template<typename Number>
class real_algebraic_number_z3 {

private:
	mutable std::shared_ptr<Z3RanContent> mContent;

public: // should be private, but would be nasty
	Z3RanContent& content() const {
		return *mContent;
	}

public:
	explicit real_algebraic_number_z3();
	explicit real_algebraic_number_z3(const Z3RanContent& content);
	explicit real_algebraic_number_z3(Z3RanContent&& content);
	explicit real_algebraic_number_z3(const Number& r);

	const UnivariatePolynomial<Number> polynomial() const;
	const Interval<Number> interval() const;
	std::size_t size() const {
		return 0;
	}
	bool is_integral() const;
	bool is_zero() const;
	bool is_numeric() const;
	Number value() const;
	bool contained_in(const Interval<Number>& i) const;
	Number integer_below() const {
		if (is_numeric()) {
			return carl::floor(value());
		} else {
			return carl::floor(lower());
		}
	}
	Sign sgn() const;
	Sign sgn(const UnivariatePolynomial<Number>& p) const;
	real_algebraic_number_z3<Number> abs() const;

	template<typename Num>
	friend Num branching_point(const real_algebraic_number_z3<Num>& n);
	template<typename Num>
	friend bool compare(const real_algebraic_number_z3<Num>& lhs, const real_algebraic_number_z3<Num>& rhs, Relation rel);
	template<typename Num>
	friend bool compare(const real_algebraic_number_z3<Num>& lhs, const Num& rhs, Relation rel);

private:
	bool equal(const real_algebraic_number_z3<Number>& rhs) const;
	bool less(const real_algebraic_number_z3<Number>& rhs) const;
	bool equal(const Number& n) const;
    bool less(const Number& n) const;
	bool greater(const Number& n) const;

	const Number lower() const;
    const Number upper() const;
};


template<typename Number>
real_algebraic_number_z3<Number> sample_above(const real_algebraic_number_z3<Number>& n);
template<typename Number>
real_algebraic_number_z3<Number> sample_below(const real_algebraic_number_z3<Number>& n);
template<typename Number>
real_algebraic_number_z3<Number> sample_between(const real_algebraic_number_z3<Number>& lower, const real_algebraic_number_z3<Number>& upper);
template<typename Number>
real_algebraic_number_z3<Number> sample_between(const real_algebraic_number_z3<Number>& lower, const Number& upper) {
	return sample_between(lower, real_algebraic_number_z3<Number>(upper));
}
template<typename Number>
real_algebraic_number_z3<Number> sample_between(const Number& lower, const real_algebraic_number_z3<Number>& upper) {
	return sample_between(real_algebraic_number_z3<Number>(lower), upper);
}

template<typename Number>
Number floor(const real_algebraic_number_z3<Number>& n) {
	if (n.is_numeric()) {
		return carl::floor(n.interval().value());
	} else {
		return carl::floor(n.interval().lower());
	}
}
template<typename Number>
Number ceil(const real_algebraic_number_z3<Number>& n) {
	if (n.is_numeric()) {
		return carl::ceil(n.interval().value());
	} else {
		return carl::ceil(n.interval().upper());
	}
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
			return lhs.equal(rhs);
		case Relation::NEQ:
			return !lhs.equal(rhs);
		case Relation::LEQ:
			return lhs.equal(rhs) || lhs.less(rhs);
		case Relation::GEQ:
			return lhs.equal(rhs) || lhs.greater(rhs);
		case Relation::LESS:
			return lhs.less(rhs);
		case Relation::GREATER:
			return lhs.greater(rhs);		
		default:
			assert(false);
			return false;
	}
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const real_algebraic_number_z3<Num>& rhs);

template<typename Number>
struct is_ran<real_algebraic_number_z3<Number>> : std::true_type {};

}


namespace std {
template<typename Number>
struct hash<carl::real_algebraic_number_z3<Number>> {
    std::size_t operator()(const carl::real_algebraic_number_z3<Number>& n) const {
		return carl::hash_all(n.integer_below());
	}
};
}

#endif
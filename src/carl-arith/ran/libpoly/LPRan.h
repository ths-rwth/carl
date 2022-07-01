#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <limits>

#include <carl-arith/interval/Interval.h>

#include <boost/logic/tribool.hpp>
#include <list>

#include "../../converter/LibpolyConverter.h"
#include "../../core/Relation.h"

#include "../ran_operations.h"
#include "../ran_operations_number.h"


namespace carl {

class RealAlgebraicNumberLibpoly {

public:
	using NumberType = mpq_class;

private:
	lp_algebraic_number_t m_content;

	static const Variable auxVariable;

	

public:
	~RealAlgebraicNumberLibpoly();

	/**
	 * Construct as zero
	 */
	RealAlgebraicNumberLibpoly();

	/**
	 * Construct from libpoly Algebraic NumberType and takes ownership
	 * @param num, LibPoly Algebraic Number
	 */
	RealAlgebraicNumberLibpoly(const poly::AlgebraicNumber& num);

	/**
	 * Construct from libpoly Algebraic NumberType and takes ownership
	 * @param num, LibPoly Algebraic Number
	 */
	RealAlgebraicNumberLibpoly(const lp_algebraic_number_t& num);

	/**
	 * Move from libpoly Algebraic NumberType (C++ Interface)
	 * @param num, LibPoly Algebraic Number
	 */
	RealAlgebraicNumberLibpoly(poly::AlgebraicNumber&& num);

	/**
	 * Move from libpoly Algebraic NumberType (C Interface) 
	 * @param num, LibPoly Algebraic Number
	 */
	RealAlgebraicNumberLibpoly(lp_algebraic_number_t&& num);

	/**
	 * Construct from Polynomial and Interval
	 * Asserts that the polynomial has exactly one root in the given interval
	 * Libpoly uses dyadic intervals, to not approximate, we calculate the roots of the polynomial and filter out all roots not in the given interval
	 * @param p Polynomial, which roots represents the algebraic number
	 * @param i Interval, in which the root must lie
	 */
	RealAlgebraicNumberLibpoly(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i);

	/**
	 * Construct from NumberType (usually mpq_class)
	 */
	RealAlgebraicNumberLibpoly(const NumberType& num);

	RealAlgebraicNumberLibpoly(const RealAlgebraicNumberLibpoly& ran);
	RealAlgebraicNumberLibpoly(RealAlgebraicNumberLibpoly&& ran);
	RealAlgebraicNumberLibpoly& operator=(const RealAlgebraicNumberLibpoly& n);
	RealAlgebraicNumberLibpoly& operator=(RealAlgebraicNumberLibpoly&& n);

	/**
	 * Create from univariate polynomial and interval with correctness checks
	 */
	static RealAlgebraicNumberLibpoly create_safe(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i);

	/**
	 * Convert a libpoly Value into an algebraic number
	 * This does not free the value
	 * In case the value is none or infinity this conversion is not possible
	 * @param Libpoly Value (C interface)
	 * @return Copy of val as a algebraic number
	 */
	static RealAlgebraicNumberLibpoly create_from_value(const lp_value_t* val) ;

	/**
	 * @return Pointer to the internal libpoly algebraic number (C interface)
	 */
	inline lp_algebraic_number_t* get_internal() {
		return &m_content;
	}

	/**
	 * @return Pointer to the internal libpoly algebraic number (C interface)
	 */
	inline const lp_algebraic_number_t* get_internal() const {
		return &m_content;
	}

	bool is_zero() const;
	bool is_integral() const;
	NumberType integer_below() const;

	/**
	 * @return true if the interval is a point or the poly has degree 1
	 */
	bool is_numeric() const;

	const poly::UPolynomial libpoly_polynomial() const;

	const poly::DyadicInterval& libpoly_interval() const;

	const UnivariatePolynomial<NumberType> polynomial() const;

	const Interval<NumberType> interval() const;

	const NumberType get_upper_bound() const;

	const NumberType get_lower_bound() const;

	/**
	 * Converts to Algebraic NumberType to Number(usually mpq_class)
	 * Asserts that the interval is a point or the poly has max degree 1
	 */
	const NumberType value() const;

	/**
	 * Does not change the current value, checks the sign and returns a copy or negates if necessary
	 * @return Absolute Value of the stored RAN
	 */
	RealAlgebraicNumberLibpoly abs() const;

	std::size_t size() const;
	Sign sgn() const;
	Sign sgn(const UnivariatePolynomial<NumberType>& p) const;
	bool contained_in(const Interval<NumberType>& i) const;

	friend bool compare(const RealAlgebraicNumberLibpoly&, const RealAlgebraicNumberLibpoly&, const Relation);
	friend bool compare(const RealAlgebraicNumberLibpoly&, const NumberType&, const Relation);

	friend NumberType branching_point(const RealAlgebraicNumberLibpoly& n);
	friend NumberType sample_above(const RealAlgebraicNumberLibpoly& n);
	friend NumberType sample_below(const RealAlgebraicNumberLibpoly& n);
	friend NumberType sample_between(const RealAlgebraicNumberLibpoly& lower, const RealAlgebraicNumberLibpoly& upper);
	friend NumberType sample_between(const RealAlgebraicNumberLibpoly& lower, const NumberType& upper);
	friend NumberType sample_between(const NumberType& lower, const RealAlgebraicNumberLibpoly& upper);
	friend NumberType floor(const RealAlgebraicNumberLibpoly& n);
	friend NumberType ceil(const RealAlgebraicNumberLibpoly& n);
};

bool compare(const RealAlgebraicNumberLibpoly&, const RealAlgebraicNumberLibpoly&, const Relation);
bool compare(const RealAlgebraicNumberLibpoly&, const RealAlgebraicNumberLibpoly::NumberType&, const Relation);

RealAlgebraicNumberLibpoly::NumberType branching_point(const RealAlgebraicNumberLibpoly& n);
RealAlgebraicNumberLibpoly::NumberType sample_above(const RealAlgebraicNumberLibpoly& n);
RealAlgebraicNumberLibpoly::NumberType sample_below(const RealAlgebraicNumberLibpoly& n);
RealAlgebraicNumberLibpoly::NumberType sample_between(const RealAlgebraicNumberLibpoly& lower, const RealAlgebraicNumberLibpoly& upper);
RealAlgebraicNumberLibpoly::NumberType sample_between(const RealAlgebraicNumberLibpoly& lower, const RealAlgebraicNumberLibpoly::NumberType& upper);
RealAlgebraicNumberLibpoly::NumberType sample_between(const RealAlgebraicNumberLibpoly::NumberType& lower, const RealAlgebraicNumberLibpoly& upper);
RealAlgebraicNumberLibpoly::NumberType floor(const RealAlgebraicNumberLibpoly& n);
RealAlgebraicNumberLibpoly::NumberType ceil(const RealAlgebraicNumberLibpoly& n);

void refine(const RealAlgebraicNumberLibpoly& n);
void refine_using(const RealAlgebraicNumberLibpoly& n, const RealAlgebraicNumberLibpoly::NumberType& pivot);
void refine_using(const RealAlgebraicNumberLibpoly& n, const poly::DyadicRational& pivot);

inline bool is_zero(const RealAlgebraicNumberLibpoly& n) {
	refine(n);
	return n.is_zero();
}

std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberLibpoly& ran);

template<>
struct is_ran<RealAlgebraicNumberLibpoly> : std::true_type {};
} // namespace carl

namespace std {
template<>
struct hash<carl::RealAlgebraicNumberLibpoly> {
	std::size_t operator()(const carl::RealAlgebraicNumberLibpoly& n) const {
		//Todo test if the precisions needs to be adjusted
		return lp_algebraic_number_hash_approx(n.get_internal(), 0);
	}
};
} // namespace std

#endif
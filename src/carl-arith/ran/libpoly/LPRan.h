#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <limits>

#include <carl-arith/interval/Interval.h>

#include <boost/logic/tribool.hpp>
#include <list>

#include "../../converter/LibpolyConverter.h"
#include "../../core/Relation.h"

#include "../common/Operations.h"
#include "../common/NumberOperations.h"


namespace carl {

class LPRealAlgebraicNumber {

public:
	using NumberType = mpq_class;

private:
	lp_algebraic_number_t m_content;

	static const Variable auxVariable;

	

public:
	~LPRealAlgebraicNumber();

	/**
	 * Construct as zero
	 */
	LPRealAlgebraicNumber();

	/**
	 * Construct from libpoly Algebraic NumberType and takes ownership
	 * @param num, LibPoly Algebraic Number
	 */
	LPRealAlgebraicNumber(const poly::AlgebraicNumber& num);

	/**
	 * Construct from libpoly Algebraic NumberType and takes ownership
	 * @param num, LibPoly Algebraic Number
	 */
	LPRealAlgebraicNumber(const lp_algebraic_number_t& num);

	/**
	 * Move from libpoly Algebraic NumberType (C++ Interface)
	 * @param num, LibPoly Algebraic Number
	 */
	LPRealAlgebraicNumber(poly::AlgebraicNumber&& num);

	/**
	 * Move from libpoly Algebraic NumberType (C Interface) 
	 * @param num, LibPoly Algebraic Number
	 */
	LPRealAlgebraicNumber(lp_algebraic_number_t&& num);

	/**
	 * Construct from Polynomial and Interval
	 * Asserts that the polynomial has exactly one root in the given interval
	 * Libpoly uses dyadic intervals, to not approximate, we calculate the roots of the polynomial and filter out all roots not in the given interval
	 * @param p Polynomial, which roots represents the algebraic number
	 * @param i Interval, in which the root must lie
	 */
	LPRealAlgebraicNumber(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i);

	/**
	 * Construct from NumberType (usually mpq_class)
	 */
	LPRealAlgebraicNumber(const NumberType& num);

	LPRealAlgebraicNumber(const LPRealAlgebraicNumber& ran);
	LPRealAlgebraicNumber(LPRealAlgebraicNumber&& ran);
	LPRealAlgebraicNumber& operator=(const LPRealAlgebraicNumber& n);
	LPRealAlgebraicNumber& operator=(LPRealAlgebraicNumber&& n);

	/**
	 * Create from univariate polynomial and interval with correctness checks
	 */
	static LPRealAlgebraicNumber create_safe(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i);

	/**
	 * Convert a libpoly Value into an algebraic number
	 * This does not free the value
	 * In case the value is none or infinity this conversion is not possible
	 * @param Libpoly Value (C interface)
	 * @return Copy of val as a algebraic number
	 */
	static LPRealAlgebraicNumber create_from_value(const lp_value_t* val) ;

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

	friend bool compare(const LPRealAlgebraicNumber&, const LPRealAlgebraicNumber&, const Relation);
	friend bool compare(const LPRealAlgebraicNumber&, const NumberType&, const Relation);

	friend NumberType branching_point(const LPRealAlgebraicNumber& n);
	friend NumberType sample_above(const LPRealAlgebraicNumber& n);
	friend NumberType sample_below(const LPRealAlgebraicNumber& n);
	friend NumberType sample_between(const LPRealAlgebraicNumber& lower, const LPRealAlgebraicNumber& upper);
	friend NumberType sample_between(const LPRealAlgebraicNumber& lower, const NumberType& upper);
	friend NumberType sample_between(const NumberType& lower, const LPRealAlgebraicNumber& upper);
	friend NumberType floor(const LPRealAlgebraicNumber& n);
	friend NumberType ceil(const LPRealAlgebraicNumber& n);
};

bool compare(const LPRealAlgebraicNumber&, const LPRealAlgebraicNumber&, const Relation);
bool compare(const LPRealAlgebraicNumber&, const LPRealAlgebraicNumber::NumberType&, const Relation);

LPRealAlgebraicNumber::NumberType branching_point(const LPRealAlgebraicNumber& n);
LPRealAlgebraicNumber::NumberType sample_above(const LPRealAlgebraicNumber& n);
LPRealAlgebraicNumber::NumberType sample_below(const LPRealAlgebraicNumber& n);
LPRealAlgebraicNumber::NumberType sample_between(const LPRealAlgebraicNumber& lower, const LPRealAlgebraicNumber& upper);
LPRealAlgebraicNumber::NumberType sample_between(const LPRealAlgebraicNumber& lower, const LPRealAlgebraicNumber::NumberType& upper);
LPRealAlgebraicNumber::NumberType sample_between(const LPRealAlgebraicNumber::NumberType& lower, const LPRealAlgebraicNumber& upper);
LPRealAlgebraicNumber::NumberType floor(const LPRealAlgebraicNumber& n);
LPRealAlgebraicNumber::NumberType ceil(const LPRealAlgebraicNumber& n);

void refine(const LPRealAlgebraicNumber& n);
void refine_using(const LPRealAlgebraicNumber& n, const LPRealAlgebraicNumber::NumberType& pivot);
void refine_using(const LPRealAlgebraicNumber& n, const poly::DyadicRational& pivot);

inline bool is_zero(const LPRealAlgebraicNumber& n) {
	refine(n);
	return lp_algebraic_number_sgn(n.get_internal()) == 0;
}

bool is_integer(const LPRealAlgebraicNumber& n);
LPRealAlgebraicNumber::NumberType integer_below(const LPRealAlgebraicNumber& n);
LPRealAlgebraicNumber abs(const LPRealAlgebraicNumber& n);

std::size_t size(const LPRealAlgebraicNumber& n);
Sign sgn(const LPRealAlgebraicNumber& n);
Sign sgn(const LPRealAlgebraicNumber& n, const UnivariatePolynomial<LPRealAlgebraicNumber::NumberType>& p);
bool contained_in(const LPRealAlgebraicNumber& n, const Interval<LPRealAlgebraicNumber::NumberType>& i);

std::ostream& operator<<(std::ostream& os, const LPRealAlgebraicNumber& ran);

template<>
struct is_ran_type<LPRealAlgebraicNumber> : std::true_type {};
} // namespace carl

namespace std {
template<>
struct hash<carl::LPRealAlgebraicNumber> {
	std::size_t operator()(const carl::LPRealAlgebraicNumber& n) const {
		//Todo test if the precisions needs to be adjusted
		return lp_algebraic_number_hash_approx(n.get_internal(), 0);
	}
};
} // namespace std

#endif
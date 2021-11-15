#pragma once

#ifdef RAN_USE_LIBPOLY

#include <limits>

#include <carl/interval/Interval.h>


#include <boost/logic/tribool.hpp>
#include <carl/core/polynomialfunctions/SquareFreePart.h>

#include <list>

#include "../../converter/LibpolyConverter.h"
#include "../../core/Relation.h"

#include "../ran_common.h"
#include "../ran_operations.h"
#include "../ran_operations_number.h"


namespace carl {

template<typename Number>
class real_algebraic_number_libpoly {

	lp_algebraic_number_t mContent;

	static const Variable auxVariable;

	template<typename Num>
	friend bool compare(const real_algebraic_number_libpoly<Num>&, const real_algebraic_number_libpoly<Num>&, const Relation);

	template<typename Num>
	friend bool compare(const real_algebraic_number_libpoly<Num>&, const Num&, const Relation);

	template<typename Num, typename Poly>
	friend boost::tribool evaluate(const Constraint<Poly>&, const ran::ran_assignment_t<real_algebraic_number_libpoly<Num>>&, bool, bool);

	template<typename Num>
	friend std::optional<real_algebraic_number_libpoly<Num>> evaluate(MultivariatePolynomial<Num>, const ran::ran_assignment_t<real_algebraic_number_libpoly<Num>>&, bool);

	template<typename Num>
	friend Num branching_point(const real_algebraic_number_libpoly<Num>& n);

	template<typename Num>
	friend Num sample_above(const real_algebraic_number_libpoly<Num>& n);

	template<typename Num>
	friend Num sample_below(const real_algebraic_number_libpoly<Num>& n);

	template<typename Num>
	friend Num sample_between(const real_algebraic_number_libpoly<Num>& lower, const real_algebraic_number_libpoly<Num>& upper);

	template<typename Num>
	friend Num sample_between(const real_algebraic_number_libpoly<Num>& lower, const Num& upper);

	template<typename Num>
	friend Num sample_between(const Num& lower, const real_algebraic_number_libpoly<Num>& upper);

	template<typename Num>
	friend Num floor(const real_algebraic_number_libpoly<Num>& n);

	template<typename Num>
	friend Num ceil(const real_algebraic_number_libpoly<Num>& n);

public:
	~real_algebraic_number_libpoly() {
		lp_algebraic_number_destruct(get_internal());
	}

	/**
	 * Construct as zero
	 */
	real_algebraic_number_libpoly() {
		lp_algebraic_number_construct_zero(get_internal());
	}

	/**
	 * Construct from libpoly Algebraic Number and takes ownership
	 * @param num, LibPoly Algebraic Number
	 */
	real_algebraic_number_libpoly(const poly::AlgebraicNumber& num) {
		lp_algebraic_number_construct_copy(get_internal(), num.get_internal());
	}

	/**
	 * Construct from libpoly Algebraic Number and takes ownership
	 * @param num, LibPoly Algebraic Number
	 */
	real_algebraic_number_libpoly(const lp_algebraic_number_t& num) {
		lp_algebraic_number_construct_copy(get_internal(), &num);
	}

	/**
	 * Move from libpoly Algebraic Number (C++ Interface)
	 * @param num, LibPoly Algebraic Number
	 */
	real_algebraic_number_libpoly(poly::AlgebraicNumber&& num)
		: real_algebraic_number_libpoly() {
		lp_algebraic_number_swap(get_internal(), num.get_internal());
	}

	/**
	 * Move from libpoly Algebraic Number (C Interface) 
	 * @param num, LibPoly Algebraic Number
	 */
	real_algebraic_number_libpoly(lp_algebraic_number_t&& num)
		: real_algebraic_number_libpoly() {
		lp_algebraic_number_swap(get_internal(), &num);
	}

	/**
	 * Construct from Polynomial and Interval
	 * Asserts that the polynomial has exactly one root in the given interval
	 * Libpoly uses dyadic intervals, to not approximate, we calculate the roots of the polynomial and filter out all roots not in the given interval
	 * @param p Polynomial, which roots represents the algebraic number
	 * @param i Interval, in which the root must lie
	 */
	real_algebraic_number_libpoly(const carl::UnivariatePolynomial<Number>& p, const Interval<Number>& i) {
		CARL_LOG_DEBUG("carl.ran.libpoly", " Create safe from poly: " << p << " in interval: " << i);

		poly::UPolynomial upoly = LibpolyConverter::getInstance().toLibpolyUPolynomial(p);
		poly::Interval inter_poly = LibpolyConverter::getInstance().toInterval(i);

		CARL_LOG_DEBUG("carl.ran.libpoly", " Converted to poly: " << upoly << " in interval: " << inter_poly);

		//actual calculations
		std::vector<poly::AlgebraicNumber> roots = poly::isolate_real_roots(upoly);
		std::vector<poly::AlgebraicNumber> res;

		for (const auto& val : roots) {
			// filter out roots not in interval
			if (poly::contains(inter_poly, poly::Value(val))) {
				CARL_LOG_DEBUG("carl.ran.libpoly", " Found Root " << val);
				res.emplace_back(val);
			}
		}

		assert(res.size() == 1);
		lp_algebraic_number_construct_zero(get_internal());
		lp_algebraic_number_swap(get_internal(), res.front().get_internal());
		//We dont need to free the items in res, as they're from the C++ interface
	}

	/**
	 * Construct from Number (usually mpq_class)
	 */
	real_algebraic_number_libpoly(const Number& num) {
		poly::Rational rat = LibpolyConverter::getInstance().toLibpolyRational(num);
		lp_algebraic_number_construct_from_rational(get_internal(), rat.get_internal());
	}

	real_algebraic_number_libpoly(const real_algebraic_number_libpoly& ran) {
		lp_algebraic_number_construct_copy(get_internal(), ran.get_internal());
	}
	real_algebraic_number_libpoly(real_algebraic_number_libpoly&& ran)
		: real_algebraic_number_libpoly() {
		lp_algebraic_number_swap(get_internal(), ran.get_internal());
	}
	real_algebraic_number_libpoly& operator=(const real_algebraic_number_libpoly& n) {
		lp_algebraic_number_destruct(get_internal());
		lp_algebraic_number_construct_copy(get_internal(), n.get_internal());
		return *this;
	}
	real_algebraic_number_libpoly& operator=(real_algebraic_number_libpoly&& n) {
		lp_algebraic_number_swap(get_internal(), n.get_internal());
		return *this;
	}

	/**
	 * Create from univariate polynomial and interval with correctness checks
	 */
	static real_algebraic_number_libpoly<Number> create_safe(const carl::UnivariatePolynomial<Number>& p, const Interval<Number>& i) {
		return real_algebraic_number_libpoly<Number>(p, i);
	}

	/**
	 * Convert a libpoly Value into an algebraic number
	 * This does not free the value
	 * In case the value is none or infinity this conversion is not possible
	 * @param Libpoly Value (C interface)
	 * @return Copy of val as a algebraic number
	 */
	static real_algebraic_number_libpoly<Number> create_from_value(const lp_value_t* val) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Converting value into algebraic number");
		lp_algebraic_number_t mVal;
		if (val->type == lp_value_type_t::LP_VALUE_NONE || val->type == lp_value_type_t::LP_VALUE_MINUS_INFINITY || val->type == lp_value_type_t::LP_VALUE_PLUS_INFINITY) {
			//If value is not assigned at all or pm infty just return an error
			assert(false && "Invalid RAN creation");
		} else if (val->type == lp_value_type_t::LP_VALUE_ALGEBRAIC) {
			//val is already an algebraic number
			lp_algebraic_number_construct_copy(&mVal, &val->value.a);
			real_algebraic_number_libpoly<Number> ret(std::move(mVal));
			lp_algebraic_number_destruct(&mVal);
			return ret;
		} else if (lp_value_is_rational(val)) {
			//if the value is a rational number: either an integer, dyadic rational or a rational
			lp_rational_t rat;
			lp_rational_construct(&rat);
			lp_value_get_rational(val, &rat);
			lp_algebraic_number_construct_from_rational(&mVal, &rat);
			real_algebraic_number_libpoly<Number> ret(std::move(mVal));
			lp_algebraic_number_destruct(&mVal);
			lp_rational_destruct(&rat);
			return ret;
		} else {
			assert(false && "Invalid RAN creation");
		}
	}

	/**
	 * @return Pointer to the internal libpoly algebraic number (C interface)
	 */
	lp_algebraic_number_t* get_internal() {
		return &mContent;
	}

	/**
	 * @return Pointer to the internal libpoly algebraic number (C interface)
	 */
	const lp_algebraic_number_t* get_internal() const {
		return &mContent;
	}

	bool is_zero() const {
		return lp_algebraic_number_sgn(get_internal()) == 0;
	}
	bool is_integral() const {
		return lp_algebraic_number_is_integer(get_internal());
	}
	Number integer_below() const {
		poly::Integer val;
		lp_algebraic_number_floor(get_internal(), val.get_internal());
		return LibpolyConverter::getInstance().toNumber<Number>(val);
	}

	/**
	 * @return true if the interval is a point or the poly has degree 1
	 */
	bool is_numeric() const {
		return lp_algebraic_number_is_rational(get_internal());
	}

	const poly::UPolynomial libpoly_polynomial() const {
		return poly::UPolynomial(static_cast<const lp_upolynomial_t*>(get_internal()->f));
	}

	const poly::DyadicInterval& libpoly_interval() const {
		return *reinterpret_cast<const poly::DyadicInterval*>(&get_internal()->I);
	}

	const UnivariatePolynomial<mpq_class> polynomial() const {
		return LibpolyConverter::getInstance().toCarlUnivariatePolynomial(libpoly_polynomial(), auxVariable);
	}

	const Interval<Number> interval() const {
		const Number& lo = get_lower_bound();
		const Number& up = get_upper_bound();
		return Interval<Number>(lo, BoundType::STRICT, up, BoundType::STRICT);
	}

	const Number get_upper_bound() const {
		return LibpolyConverter::getInstance().toNumber<Number>(poly::get_upper(libpoly_interval()));
	}

	const Number get_lower_bound() const {
		return LibpolyConverter::getInstance().toNumber<Number>(poly::get_lower(libpoly_interval()));
	}

	/**
	 * Converts to Algebraic Number to Number(usually mpq_class)
	 * Asserts that the interval is a point or the poly has max degree 1
	 */
	const Number value() const {
		assert(is_numeric());
		lp_rational_t result ;
		if(lp_dyadic_interval_is_point(&get_internal()->I)){
		    // It's a point value, so we just get it
	      lp_rational_construct_from_dyadic(&result, lp_dyadic_interval_get_point(&get_internal()->I));
		}else{
			const lp_upolynomial_t* poly = get_internal()->f ;
			assert(lp_upolynomial_degree(poly) == 1) ;
	        // p = ax + b = 0 => x = -b/a
	        const lp_integer_t* b = lp_upolynomial_const_term(poly);
	        const lp_integer_t* a = lp_upolynomial_lead_coeff(poly);
			if(b){
				//b != 0
				//we can do this, as lp_rational_t == __mpq_struct
				mpq_init(&result);
 				mpq_set_num(&result, b);
 				mpq_set_den(&result, a);
 				mpq_canonicalize(&result);
				mpq_neg(&result, &result);
			}else{
				// b = 0
				mpq_init(&result);
			}
		}
		Number num = LibpolyConverter::getInstance().toNumber<Number>(&result) ;
		lp_rational_destruct(&result) ;
		return num ;
	}

	/**
	 * Does not change the current value, checks the sign and returns a copy or negates if necessary
	 * @return Absolute Value of the stored RAN
	 */
	real_algebraic_number_libpoly<Number> abs() const {

		if (is_numeric()) {
			CARL_LOG_DEBUG("carl.ran.libpoly", "Algebraic Number abs got numeric value");
			return real_algebraic_number_libpoly<Number>(carl::abs(value()));
		}

		int sign = lp_algebraic_number_sgn(get_internal());
		CARL_LOG_DEBUG("carl.ran.libpoly", "Algebraic Number abs got sign : " << sign << " of " << *this);


		if (sign >= 0) {
			return real_algebraic_number_libpoly<Number>(*this);
		} else {
			lp_algebraic_number_t val;
			lp_algebraic_number_construct_zero(&val);
			lp_algebraic_number_neg(&val, get_internal());
			auto ret = real_algebraic_number_libpoly<Number>(std::move(val));
			lp_algebraic_number_destruct(&val);
			return ret ;
		}
	}

	std::size_t size() const {
		//From ran_interval.h
		if (is_numeric()) {
			return carl::bitsize(get_lower_bound()) + carl::bitsize(get_upper_bound());
		} else {
			return carl::bitsize(get_lower_bound()) + carl::bitsize(get_upper_bound()) + poly::degree(libpoly_polynomial());
		}
	}

	Sign sgn() const {
		return static_cast<Sign>(lp_algebraic_number_sgn(get_internal()));
	}

	Sign sgn(const UnivariatePolynomial<Number>& p) const{
		//Add internal algebraic number to an libpoly assignment
		poly::Assignment assignment ;
		lp_value_t val;
		//Turn into value
		lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, get_internal());
		//That copies the value into the assignment
		assignment.set(VariableMapper::getInstance().getLibpolyVariable(p.mainVar()), poly::Value(&val));
		lp_value_destruct(&val);

		poly::Polynomial poly = LibpolyConverter::getInstance().toLibpolyPolynomial(carl::MultivariatePolynomial<Number>(p)) ;
		return static_cast<Sign>(poly::sgn(poly, assignment));
	}

	bool contained_in(const Interval<Number>& i) const {
		CARL_LOG_DEBUG("carl.ran.libpoly", "ran " << *this << " contained in " << i);

		poly::Interval inter = LibpolyConverter::getInstance().toInterval<Number>(i);

		lp_value_t val;
		lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, get_internal());

		bool b = lp_interval_contains(inter.get_internal(), &val);
		//Destruct the value, but do not free mContent
		lp_value_destruct(&val);

		return b;
	}
};

/**
 * Refine the number until the interval width is < 1 or the number is numeric (rational)
 * NOT CONST, the number is the same, but internally might change 
 */
template<typename Number>
void refine(const real_algebraic_number_libpoly<Number>& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Refining Algebraic Number : " << n);

	while (poly::log_size(n.libpoly_interval()) > 0 && !n.is_numeric()) {
		lp_algebraic_number_refine_const(n.get_internal());
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "Finished Refining Algebraic Number : " << n);
}

/**
 * Refine until n is numeric (rational) or until pivot is not in the isolating interval of n
 * NOT CONST, the number is the same, but internally might change 
 */
template<typename Number>
void refine_using(const real_algebraic_number_libpoly<Number>& n, const Number& pivot) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Refining Algebraic Number : " << n);
	//Convert pivot to libpoly rational
	poly::Rational pivot_libpoly = LibpolyConverter::getInstance().toLibpolyRational(pivot);
	while (!n.is_numeric() && n.libpoly_interval() == pivot_libpoly) {
		lp_algebraic_number_refine_const(n.get_internal());
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "Finished Refining Algebraic Number : " << n);
}

/**
 * Same as above, but with libpoly dyadic rational as pivot 
 */
template<typename Number>
void refine_using(const real_algebraic_number_libpoly<Number>& n, const poly::DyadicRational& pivot) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Refining Algebraic Number : " << n);
	while (!n.is_numeric() && n.libpoly_interval() == pivot) {
		lp_algebraic_number_refine_const(n.get_internal());
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "Finished Refining Algebraic Number : " << n);
}

template<typename Number>
Number branching_point(const real_algebraic_number_libpoly<Number>& n) {
	//return carl::sample(n.interval_int());
	refine(n);
	poly::DyadicRational res;
	lp_algebraic_number_get_dyadic_midpoint(n.get_internal(), res.get_internal());
	Number num = LibpolyConverter::getInstance().toNumber<Number>(res);
	return num;
}

template<typename Number>
Number sample_above(const real_algebraic_number_libpoly<Number>& n) {
	//return carl::floor(n.interval_int().upper()) + 1; From ran_interval.h
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling above: " << n);
	refine(n);

	return carl::floor(n.get_upper_bound()) + 1;
}

template<typename Number>
Number sample_below(const real_algebraic_number_libpoly<Number>& n) {
	//return carl::ceil(n.interval_int().lower()) - 1; From ran_interval.h
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling below: " << n);
	refine(n);

	return carl::ceil(n.get_lower_bound()) - 1;
}

template<typename Number>
Number sample_between(const real_algebraic_number_libpoly<Number>& lower, const real_algebraic_number_libpoly<Number>& upper) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);

	//Make sure that the intervals are disjoint
	while (lower.get_upper_bound() > upper.get_lower_bound()) {
		if(lower.is_numeric()){
			break ;
		}
		if(upper.is_numeric()){
			break ;
		}
		lp_algebraic_number_refine_const(lower.get_internal());
		lp_algebraic_number_refine_const(upper.get_internal());
	}

	if (lower.is_numeric()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample_between(lower.value(), upper));
		return sample_between(lower.value(), upper);
	} else if (upper.is_numeric()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample_between(lower, upper.value()));
		return sample_between(lower, upper.value());
	} else {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << carl::sample(Interval<Number>(lower.get_upper_bound(), upper.get_lower_bound()), true));
		return carl::sample(Interval<Number>(lower.get_upper_bound(), upper.get_lower_bound()), true);
	}
}
template<typename Number>
Number sample_between(const real_algebraic_number_libpoly<Number>& lower, const Number& upper) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);
	//Make sure that the intervals are disjoint
	while (!(lower.get_upper_bound() < upper) && !lower.is_numeric()) {
		lp_algebraic_number_refine_const(lower.get_internal());
	}

	if (lower.is_numeric()) {
		Number sample = sample_between(lower.value(), upper);
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample);
		return sample;
	} else {
		//sample from interval
		Number sample = carl::sample(Interval<Number>(lower.get_upper_bound(), BoundType::WEAK, upper, BoundType::STRICT), false);
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample);
		return sample;
	}
}

//Make sure that the intervals are disjoint
template<typename Number>
Number sample_between(const Number& lower, const real_algebraic_number_libpoly<Number>& upper) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);

	while (!(lower < upper.get_lower_bound()) && !upper.is_numeric()) {
		lp_algebraic_number_refine_const(upper.get_internal());
	}


	if (upper.is_numeric()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample_between(lower, upper.value()));
		return sample_between(lower, upper.value());
	} else {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << carl::sample(Interval<Number>(lower, BoundType::STRICT, upper.get_lower_bound(), BoundType::WEAK), false));
		return carl::sample(Interval<Number>(lower, BoundType::STRICT, upper.get_lower_bound(), BoundType::WEAK), false);
	}
}
template<typename Number>
Number floor(const real_algebraic_number_libpoly<Number>& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Floor of: " << n);
	refine(n);
	lp_integer_t val;
	lp_algebraic_number_floor(n.get_internal(), &val);
	Number ret =  LibpolyConverter::getInstance().toNumber<Number>(poly::detail::cast_from(&val));
	lp_integer_destruct(&val) ;
	return ret ;
}

template<typename Number>
Number ceil(const real_algebraic_number_libpoly<Number>& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Ceil of: " << n);
	refine(n);
	lp_integer_t val;
	lp_algebraic_number_ceiling(n.get_internal(), &val);
	Number ret =  LibpolyConverter::getInstance().toNumber<Number>(poly::detail::cast_from(&val));
	lp_integer_destruct(&val) ;
	return ret ;
}

template<typename Number>
bool isZero(const real_algebraic_number_libpoly<Number>& n) {
	refine(n);
	return n.is_zero();
}

template<typename Number>
bool compare(const real_algebraic_number_libpoly<Number>& lhs, const real_algebraic_number_libpoly<Number>& rhs, const Relation relation) {
	int cmp = lp_algebraic_number_cmp(lhs.get_internal(), rhs.get_internal());
	switch (relation) {
	case Relation::EQ:
		return cmp == 0;
	case Relation::NEQ:
		return cmp != 0;
	case Relation::LESS:
		return cmp < 0;
	case Relation::LEQ:
		return cmp <= 0;
	case Relation::GREATER:
		return cmp > 0;
	case Relation::GEQ:
		return cmp >= 0;
	default:
		assert(false);
		return false;
	}
}

template<typename Number>
const carl::Variable real_algebraic_number_libpoly<Number>::auxVariable = freshRealVariable("__r");

template<typename Number>
bool compare(const real_algebraic_number_libpoly<Number>& lhs, const Number& rhs, const Relation relation) {

	poly::Rational rat = LibpolyConverter::getInstance().toLibpolyRational<Number>(rhs);

	//refine unitl rhs in not in interval of lhs
	//Technically not necessary because libpoly does that in compare, but whatever
	refine_using(lhs, rhs);

	int cmp = lp_algebraic_number_cmp_rational(lhs.get_internal(), rat.get_internal());

	switch (relation) {
	case Relation::EQ:
		return cmp == 0;
	case Relation::NEQ:
		return cmp != 0;
	case Relation::LESS:
		return cmp < 0;
	case Relation::LEQ:
		return cmp <= 0;
	case Relation::GREATER:
		return cmp > 0;
	case Relation::GEQ:
		return cmp >= 0;
	default : 
		assert(false);
		return false ;
	}
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const real_algebraic_number_libpoly<Num>& ran) {
	char* str = lp_algebraic_number_to_string(ran.get_internal());
	os << str;
	free(str);
	return os;
}

template<typename Number>
struct is_ran<real_algebraic_number_libpoly<Number>> : std::true_type {};
} // namespace carl

namespace std {
template<typename Number>
struct hash<carl::real_algebraic_number_libpoly<Number>> {
	std::size_t operator()(const carl::real_algebraic_number_libpoly<Number>& n) const {
		//Todo test if the precisions needs to be adjusted
		return lp_algebraic_number_hash_approx(n.get_internal(), 0);
	}
};
} // namespace std

#endif
#include "LPRan.h"

#ifdef USE_LIBPOLY

namespace carl {

using NumberType = RealAlgebraicNumberLibpoly::NumberType;

RealAlgebraicNumberLibpoly::~RealAlgebraicNumberLibpoly() {
    lp_algebraic_number_destruct(get_internal());
}

RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly() {
    lp_algebraic_number_construct_zero(get_internal());
}

RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(const poly::AlgebraicNumber& num) {
    lp_algebraic_number_construct_copy(get_internal(), num.get_internal());
}

RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(const lp_algebraic_number_t& num) {
    lp_algebraic_number_construct_copy(get_internal(), &num);
}

RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(poly::AlgebraicNumber&& num)
    : RealAlgebraicNumberLibpoly() {
    lp_algebraic_number_swap(get_internal(), num.get_internal());
}

RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(lp_algebraic_number_t&& num)
    : RealAlgebraicNumberLibpoly() {
    lp_algebraic_number_swap(get_internal(), &num);
}

RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i) {
    CARL_LOG_DEBUG("carl.ran.libpoly", " Create safe from poly: " << p << " in interval: " << i);

    poly::UPolynomial upoly = to_libpoly_upolynomial(p);
    poly::Interval inter_poly = to_libpoly_interval(i);

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
 * Construct from NumberType (usually mpq_class)
 */
RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(const NumberType& num) {
    poly::Rational rat = to_libpoly_rational(num);
    lp_algebraic_number_construct_from_rational(get_internal(), rat.get_internal());
}

RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(const RealAlgebraicNumberLibpoly& ran) {
    lp_algebraic_number_construct_copy(get_internal(), ran.get_internal());
}
RealAlgebraicNumberLibpoly::RealAlgebraicNumberLibpoly(RealAlgebraicNumberLibpoly&& ran)
    : RealAlgebraicNumberLibpoly() {
    lp_algebraic_number_swap(get_internal(), ran.get_internal());
}
RealAlgebraicNumberLibpoly& RealAlgebraicNumberLibpoly::operator=(const RealAlgebraicNumberLibpoly& n) {
    lp_algebraic_number_destruct(get_internal());
    lp_algebraic_number_construct_copy(get_internal(), n.get_internal());
    return *this;
}
RealAlgebraicNumberLibpoly& RealAlgebraicNumberLibpoly::operator=(RealAlgebraicNumberLibpoly&& n) {
    lp_algebraic_number_swap(get_internal(), n.get_internal());
    return *this;
}

/**
 * Create from univariate polynomial and interval with correctness checks
 */
RealAlgebraicNumberLibpoly RealAlgebraicNumberLibpoly::create_safe(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i) {
    return RealAlgebraicNumberLibpoly(p, i);
}

/**
 * Convert a libpoly Value into an algebraic number
 * This does not free the value
 * In case the value is none or infinity this conversion is not possible
 * @param Libpoly Value (C interface)
 * @return Copy of val as a algebraic number
 */
RealAlgebraicNumberLibpoly RealAlgebraicNumberLibpoly::create_from_value(const lp_value_t* val) {
    CARL_LOG_DEBUG("carl.ran.libpoly", "Converting value into algebraic number");
    lp_algebraic_number_t mVal;
    if (val->type == lp_value_type_t::LP_VALUE_NONE || val->type == lp_value_type_t::LP_VALUE_MINUS_INFINITY || val->type == lp_value_type_t::LP_VALUE_PLUS_INFINITY) {
        //If value is not assigned at all or pm infty just return an error
        assert(false && "Invalid RAN creation");
    } else if (val->type == lp_value_type_t::LP_VALUE_ALGEBRAIC) {
        //val is already an algebraic number
        lp_algebraic_number_construct_copy(&mVal, &val->value.a);
        RealAlgebraicNumberLibpoly ret(std::move(mVal));
        lp_algebraic_number_destruct(&mVal);
        return ret;
    } else if (lp_value_is_rational(val)) {
        //if the value is a rational number: either an integer, dyadic rational or a rational
        lp_rational_t rat;
        lp_rational_construct(&rat);
        lp_value_get_rational(val, &rat);
        lp_algebraic_number_construct_from_rational(&mVal, &rat);
        RealAlgebraicNumberLibpoly ret(std::move(mVal));
        lp_algebraic_number_destruct(&mVal);
        lp_rational_destruct(&rat);
        return ret;
    } else {
        assert(false && "Invalid RAN creation");
    }
    return RealAlgebraicNumberLibpoly();
}

inline bool RealAlgebraicNumberLibpoly::is_zero() const {
    return lp_algebraic_number_sgn(get_internal()) == 0;
}
bool RealAlgebraicNumberLibpoly::is_integral() const {
    return lp_algebraic_number_is_integer(get_internal());
}
NumberType RealAlgebraicNumberLibpoly::integer_below() const {
    poly::Integer val;
    lp_algebraic_number_floor(get_internal(), val.get_internal());
    return to_rational(val);
}

/**
 * @return true if the interval is a point or the poly has degree 1
 */
bool RealAlgebraicNumberLibpoly::is_numeric() const {
    return lp_algebraic_number_is_rational(get_internal());
}

const poly::UPolynomial RealAlgebraicNumberLibpoly::libpoly_polynomial() const {
    return poly::UPolynomial(static_cast<const lp_upolynomial_t*>(get_internal()->f));
}

const poly::DyadicInterval& RealAlgebraicNumberLibpoly::libpoly_interval() const {
    return *reinterpret_cast<const poly::DyadicInterval*>(&get_internal()->I);
}

const UnivariatePolynomial<NumberType> RealAlgebraicNumberLibpoly::polynomial() const {
    return to_carl_univariate_polynomial(libpoly_polynomial(), auxVariable);
}

const Interval<NumberType> RealAlgebraicNumberLibpoly::interval() const {
    const NumberType& lo = get_lower_bound();
    const NumberType& up = get_upper_bound();
    return Interval<NumberType>(lo, BoundType::STRICT, up, BoundType::STRICT);
}

const NumberType RealAlgebraicNumberLibpoly::get_upper_bound() const {
    return to_rational(poly::get_upper(libpoly_interval()));
}

const NumberType RealAlgebraicNumberLibpoly::get_lower_bound() const {
    return to_rational(poly::get_lower(libpoly_interval()));
}

/**
 * Converts to Algebraic NumberType to Number(usually mpq_class)
 * Asserts that the interval is a point or the poly has max degree 1
 */
const NumberType RealAlgebraicNumberLibpoly::value() const {
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
    NumberType num = to_rational(&result) ;
    lp_rational_destruct(&result) ;
    return num ;
}

/**
 * Does not change the current value, checks the sign and returns a copy or negates if necessary
 * @return Absolute Value of the stored RAN
 */
RealAlgebraicNumberLibpoly RealAlgebraicNumberLibpoly::abs() const {

    if (is_numeric()) {
        CARL_LOG_DEBUG("carl.ran.libpoly", "Algebraic NumberType abs got numeric value");
        return RealAlgebraicNumberLibpoly(carl::abs(value()));
    }

    int sign = lp_algebraic_number_sgn(get_internal());
    CARL_LOG_DEBUG("carl.ran.libpoly", "Algebraic NumberType abs got sign : " << sign << " of " << *this);


    if (sign >= 0) {
        return RealAlgebraicNumberLibpoly(*this);
    } else {
        lp_algebraic_number_t val;
        lp_algebraic_number_construct_zero(&val);
        lp_algebraic_number_neg(&val, get_internal());
        auto ret = RealAlgebraicNumberLibpoly(std::move(val));
        lp_algebraic_number_destruct(&val);
        return ret ;
    }
}

std::size_t RealAlgebraicNumberLibpoly::size() const {
    //From ran_interval.h
    if (is_numeric()) {
        return carl::bitsize(get_lower_bound()) + carl::bitsize(get_upper_bound());
    } else {
        return carl::bitsize(get_lower_bound()) + carl::bitsize(get_upper_bound()) + poly::degree(libpoly_polynomial());
    }
}

Sign RealAlgebraicNumberLibpoly::sgn() const {
    return static_cast<Sign>(lp_algebraic_number_sgn(get_internal()));
}

Sign RealAlgebraicNumberLibpoly::sgn(const UnivariatePolynomial<NumberType>& p) const{
    //Add internal algebraic number to an libpoly assignment
    poly::Assignment assignment ;
    lp_value_t val;
    //Turn into value
    lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, get_internal());
    //That copies the value into the assignment
    assignment.set(VariableMapper::getInstance().getLibpolyVariable(p.main_var()), poly::Value(&val));
    lp_value_destruct(&val);

    poly::Polynomial poly = to_libpoly_polynomial(carl::MultivariatePolynomial<NumberType>(p)) ;
    return static_cast<Sign>(poly::sgn(poly, assignment));
}

bool RealAlgebraicNumberLibpoly::contained_in(const Interval<NumberType>& i) const {
    CARL_LOG_DEBUG("carl.ran.libpoly", "ran " << *this << " contained in " << i);

    poly::Interval inter = to_libpoly_interval(i);

    lp_value_t val;
    lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, get_internal());

    bool b = lp_interval_contains(inter.get_internal(), &val);
    //Destruct the value, but do not free m_content
    lp_value_destruct(&val);

    return b;
}


/**
 * Refine the number until the interval width is < 1 or the number is numeric (rational)
 * NOT CONST, the number is the same, but internally might change 
 */

void refine(const RealAlgebraicNumberLibpoly& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Refining Algebraic NumberType : " << n);

	while (poly::log_size(n.libpoly_interval()) > 0 && !n.is_numeric()) {
		lp_algebraic_number_refine_const(n.get_internal());
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "Finished Refining Algebraic NumberType : " << n);
}

/**
 * Refine until n is numeric (rational) or until pivot is not in the isolating interval of n
 * NOT CONST, the number is the same, but internally might change 
 */

void refine_using(const RealAlgebraicNumberLibpoly& n, const NumberType& pivot) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Refining Algebraic NumberType : " << n);
	//Convert pivot to libpoly rational
	poly::Rational pivot_libpoly = to_libpoly_rational(pivot);
	while (!n.is_numeric() && n.libpoly_interval() == pivot_libpoly) {
		lp_algebraic_number_refine_const(n.get_internal());
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "Finished Refining Algebraic NumberType : " << n);
}

/**
 * Same as above, but with libpoly dyadic rational as pivot 
 */

void refine_using(const RealAlgebraicNumberLibpoly& n, const poly::DyadicRational& pivot) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Refining Algebraic NumberType : " << n);
	while (!n.is_numeric() && n.libpoly_interval() == pivot) {
		lp_algebraic_number_refine_const(n.get_internal());
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "Finished Refining Algebraic NumberType : " << n);
}


NumberType branching_point(const RealAlgebraicNumberLibpoly& n) {
	//return carl::sample(n.interval_int());
	refine(n);
	poly::DyadicRational res;
	lp_algebraic_number_get_dyadic_midpoint(n.get_internal(), res.get_internal());
	NumberType num = to_rational(res);
	return num;
}


NumberType sample_above(const RealAlgebraicNumberLibpoly& n) {
	//return carl::floor(n.interval_int().upper()) + 1; From ran_interval.h
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling above: " << n);
	refine(n);

	return carl::floor(n.get_upper_bound()) + 1;
}


NumberType sample_below(const RealAlgebraicNumberLibpoly& n) {
	//return carl::ceil(n.interval_int().lower()) - 1; From ran_interval.h
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling below: " << n);
	refine(n);

	return carl::ceil(n.get_lower_bound()) - 1;
}


NumberType sample_between(const RealAlgebraicNumberLibpoly& lower, const RealAlgebraicNumberLibpoly& upper) {
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
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << carl::sample(Interval<NumberType>(lower.get_upper_bound(), upper.get_lower_bound()), true));
		return carl::sample(Interval<NumberType>(lower.get_upper_bound(), upper.get_lower_bound()), true);
	}
}

NumberType sample_between(const RealAlgebraicNumberLibpoly& lower, const NumberType& upper) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);
	//Make sure that the intervals are disjoint
	while (!(lower.get_upper_bound() < upper) && !lower.is_numeric()) {
		lp_algebraic_number_refine_const(lower.get_internal());
	}

	if (lower.is_numeric()) {
		NumberType sample = sample_between(lower.value(), upper);
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample);
		return sample;
	} else {
		//sample from interval
		NumberType sample = carl::sample(Interval<NumberType>(lower.get_upper_bound(), BoundType::WEAK, upper, BoundType::STRICT), false);
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample);
		return sample;
	}
}

//Make sure that the intervals are disjoint

NumberType sample_between(const NumberType& lower, const RealAlgebraicNumberLibpoly& upper) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);

	while (!(lower < upper.get_lower_bound()) && !upper.is_numeric()) {
		lp_algebraic_number_refine_const(upper.get_internal());
	}


	if (upper.is_numeric()) {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << sample_between(lower, upper.value()));
		return sample_between(lower, upper.value());
	} else {
		CARL_LOG_DEBUG("carl.ran.libpoly", "Found sample between: " << carl::sample(Interval<NumberType>(lower, BoundType::STRICT, upper.get_lower_bound(), BoundType::WEAK), false));
		return carl::sample(Interval<NumberType>(lower, BoundType::STRICT, upper.get_lower_bound(), BoundType::WEAK), false);
	}
}

NumberType floor(const RealAlgebraicNumberLibpoly& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Floor of: " << n);
	refine(n);
	lp_integer_t val;
	lp_algebraic_number_floor(n.get_internal(), &val);
	NumberType ret = to_rational(*poly::detail::cast_from(&val));
	lp_integer_destruct(&val) ;
	return ret ;
}


NumberType ceil(const RealAlgebraicNumberLibpoly& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Ceil of: " << n);
	refine(n);
	lp_integer_t val;
	lp_algebraic_number_ceiling(n.get_internal(), &val);
	NumberType ret = to_rational(*poly::detail::cast_from(&val));
	lp_integer_destruct(&val) ;
	return ret ;
}


bool compare(const RealAlgebraicNumberLibpoly& lhs, const RealAlgebraicNumberLibpoly& rhs, const Relation relation) {
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


const carl::Variable RealAlgebraicNumberLibpoly::auxVariable = fresh_real_variable("__r");


bool compare(const RealAlgebraicNumberLibpoly& lhs, const NumberType& rhs, const Relation relation) {

	poly::Rational rat = to_libpoly_rational(rhs);

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


std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberLibpoly& ran) {
	char* str = lp_algebraic_number_to_string(ran.get_internal());
	os << str;
	free(str);
	return os;
}

}

#endif
#include "LPRan.h"

#ifdef USE_LIBPOLY

namespace carl {

using NumberType = LPRealAlgebraicNumber::NumberType;

LPRealAlgebraicNumber::~LPRealAlgebraicNumber() {
}

LPRealAlgebraicNumber::LPRealAlgebraicNumber() : m_content(std::make_shared<LPRealAlgebraicNumber::Content>()) {
    lp_value_construct_zero(get_internal());
}

LPRealAlgebraicNumber::LPRealAlgebraicNumber(const lp_value_t& num) : m_content(std::make_shared<LPRealAlgebraicNumber::Content>()) {
    lp_value_construct_copy(get_internal(), &num);
}

LPRealAlgebraicNumber::LPRealAlgebraicNumber(lp_value_t&& num) : m_content(std::make_shared<LPRealAlgebraicNumber::Content>()) {
    *get_internal() = num;
}

LPRealAlgebraicNumber::LPRealAlgebraicNumber(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i) : LPRealAlgebraicNumber() {
    CARL_LOG_DEBUG("carl.ran.libpoly", " Create from poly: " << p << " in interval: " << i);

    lp_upolynomial_t* upoly = to_libpoly_upolynomial(p);
    lp_interval_t* inter_poly = to_libpoly_interval(i);

    lp_algebraic_number_t* roots = new lp_algebraic_number_t[lp_upolynomial_degree(upoly)];
    std::size_t roots_size;
    lp_upolynomial_roots_isolate(upoly, roots, &roots_size);

    bool found = false;
    for (std::size_t i = 0; i < roots_size; ++i) {
        lp_value_t tmp;
        lp_value_construct(&tmp, LP_VALUE_ALGEBRAIC, &roots[i]);
        if (lp_interval_contains(inter_poly, &tmp)) {
            *get_internal() = tmp;
            found = true;
            break;
        }
        lp_value_destruct(&tmp);
    }
    assert(found);

    for (std::size_t i = 0; i < roots_size; ++i) {
      lp_algebraic_number_destruct(&roots[i]);
    }
    delete[] roots;

    lp_upolynomial_delete(upoly);

    lp_interval_destruct(inter_poly);
    delete inter_poly;
}

/**
 * Construct from NumberType (usually mpq_class)
 */
LPRealAlgebraicNumber::LPRealAlgebraicNumber(const NumberType& num) : LPRealAlgebraicNumber() {
    lp_value_construct(get_internal(), LP_VALUE_RATIONAL, (lp_rational_t*)num.get_mpq_t());
}

LPRealAlgebraicNumber::LPRealAlgebraicNumber(const LPRealAlgebraicNumber& ran) : m_content(ran.m_content) {
}
LPRealAlgebraicNumber::LPRealAlgebraicNumber(LPRealAlgebraicNumber&& ran) : m_content(std::move(ran.m_content)) {
}
LPRealAlgebraicNumber& LPRealAlgebraicNumber::operator=(const LPRealAlgebraicNumber& n) {
    m_content = n.m_content;
    return *this;
}
LPRealAlgebraicNumber& LPRealAlgebraicNumber::operator=(LPRealAlgebraicNumber&& n) {
    m_content = std::move(n.m_content);
    return *this;
}

/**
 * Create from univariate polynomial and interval with correctness checks
 */
LPRealAlgebraicNumber LPRealAlgebraicNumber::create_safe(const carl::UnivariatePolynomial<NumberType>& p, const Interval<NumberType>& i) {
    return LPRealAlgebraicNumber(p, i);
}

bool is_integer(const LPRealAlgebraicNumber& n) {
    return lp_value_is_integer(n.get_internal());
}
LPRealAlgebraicNumber::NumberType integer_below(const LPRealAlgebraicNumber& n) {
    lp_integer_t result;
    lp_integer_construct(&result);
    lp_value_floor(n.get_internal(), &result);
    NumberType num = to_rational(&result) ;
    lp_integer_destruct(&result) ;
    return num;
}

/**
 * @return true if the interval is a point or the poly has degree 1
 */
bool LPRealAlgebraicNumber::is_numeric() const {
    return lp_value_is_rational(get_internal());
}

const UnivariatePolynomial<NumberType> LPRealAlgebraicNumber::polynomial() const {
    assert(!is_numeric());
    return to_carl_univariate_polynomial(get_internal()->value.a.f, auxVariable);
}

const Interval<NumberType> LPRealAlgebraicNumber::interval() const {
	if (is_numeric()) {
		return Interval<NumberType>(value());
	} else {
    	return Interval<NumberType>(get_lower_bound(), BoundType::STRICT, get_upper_bound(), BoundType::STRICT);
	}
}

const NumberType LPRealAlgebraicNumber::get_lower_bound() const {
    if (is_numeric()) return value();
    else return to_rational(&get_internal()->value.a.I.a);
}

const NumberType LPRealAlgebraicNumber::get_upper_bound() const {
    if (is_numeric()) return value();
    else return to_rational(&get_internal()->value.a.I.b);
}

/**
 * Converts to Algebraic NumberType to Number(usually mpq_class)
 * Asserts that the interval is a point or the poly has max degree 1
 */
const NumberType LPRealAlgebraicNumber::value() const {
    assert(is_numeric());
    lp_rational_t result;
    lp_rational_construct(&result);
    lp_value_get_rational(get_internal(), &result);
    NumberType num = to_rational(&result) ;
    lp_rational_destruct(&result) ;
    return num;
}

/**
 * Does not change the current value, checks the sign and returns a copy or negates if necessary
 * @return Absolute Value of the stored RAN
 */
LPRealAlgebraicNumber abs(const LPRealAlgebraicNumber& n) {
    if (n.is_numeric()) {
        CARL_LOG_DEBUG("carl.ran.libpoly", "Algebraic NumberType abs got numeric value");
        return LPRealAlgebraicNumber(carl::abs(n.value()));
    }

    int sign = lp_value_sgn(n.get_internal());
    CARL_LOG_DEBUG("carl.ran.libpoly", "Algebraic NumberType abs got sign : " << sign << " of " << n);

    if (sign >= 0) {
        return LPRealAlgebraicNumber(n);
    } else {
        lp_value_t val;
        lp_value_construct_none(&val);
        lp_value_neg(&val, n.get_internal());
        auto ret = LPRealAlgebraicNumber(std::move(val));
        return ret;
    }
}

std::size_t bitsize(const LPRealAlgebraicNumber& n) {
    if (n.is_numeric()) {
        return carl::bitsize(n.get_lower_bound()) + carl::bitsize(n.get_upper_bound());
    } else {
        return carl::bitsize(n.get_lower_bound()) + carl::bitsize(n.get_upper_bound()) + lp_upolynomial_degree(n.get_internal()->value.a.f);
    }
}

Sign sgn(const LPRealAlgebraicNumber& n) {
    return static_cast<Sign>(lp_value_sgn(n.get_internal()));
}

Sign sgn(const LPRealAlgebraicNumber&, const UnivariatePolynomial<LPRealAlgebraicNumber::NumberType>&) {
    assert(false);
    return Sign::ZERO;
}

bool contained_in(const LPRealAlgebraicNumber& n, const Interval<LPRealAlgebraicNumber::NumberType>& i) {
    CARL_LOG_DEBUG("carl.ran.libpoly", "ran " << n << " contained in " << i);
    lp_interval_t* inter = to_libpoly_interval(i);
    bool res = lp_interval_contains(inter, n.get_internal());
    lp_interval_destruct(inter); 
    delete inter;
    return res;
}


/**
 * Refine the number until the interval width is < 1 or the number is numeric (rational)
 * NOT CONST, the number is the same, but internally might change 
 */

void refine(const LPRealAlgebraicNumber& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Refining Algebraic NumberType : " << n);

    if (n.is_numeric()) return;

	while (lp_dyadic_interval_size(&n.get_internal()->value.a.I) > 0 && !n.is_numeric()) {
		lp_algebraic_number_refine_const(&n.get_internal()->value.a);
	}
	CARL_LOG_DEBUG("carl.ran.libpoly", "Finished Refining Algebraic NumberType : " << n);
}

void LPRealAlgebraicNumber::refine() const {
	carl::refine(*this);
}

NumberType branching_point(const LPRealAlgebraicNumber& n) {
    if (n.is_numeric()) return n.value();
    refine(n);
    lp_dyadic_rational_t res;
    lp_dyadic_rational_construct(&res);
	lp_algebraic_number_get_dyadic_midpoint(&n.get_internal()->value.a, &res);
    auto result = to_rational(&res);
    lp_dyadic_rational_destruct(&res);
	return result;
}


NumberType sample_above(const LPRealAlgebraicNumber& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling above: " << n);
	
    lp_value_t inf;
    lp_value_construct(&inf, LP_VALUE_PLUS_INFINITY, 0);

    lp_value_t res;
    lp_value_construct_none(&res);
    lp_value_get_value_between(n.get_internal(), true, &inf, true, &res);
    auto val = to_rational(&res);
    lp_value_destruct(&res);
    return val;
}


NumberType sample_below(const LPRealAlgebraicNumber& n) {
	//return carl::ceil(n.interval_int().lower()) - 1; From Ran.h
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling below: " << n);
	
    lp_value_t inf;
    lp_value_construct(&inf, LP_VALUE_MINUS_INFINITY, 0);

    lp_value_t res;
    lp_value_construct_none(&res);
    lp_value_get_value_between(&inf, true, n.get_internal(), true, &res);
    auto val = to_rational(&res);
    lp_value_destruct(&res);
    return val;
}


NumberType sample_between(const LPRealAlgebraicNumber& lower, const LPRealAlgebraicNumber& upper) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);

	lp_value_t res;
    lp_value_construct_none(&res);
    lp_value_get_value_between(lower.get_internal(), true, upper.get_internal(), true, &res);
    auto val = to_rational(&res);
    lp_value_destruct(&res);
    return val;
}

NumberType sample_between(const LPRealAlgebraicNumber& lower, const NumberType& upper) {
    CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);

    lp_value_t v_upper;
    lp_value_construct(&v_upper, LP_VALUE_RATIONAL, upper.get_mpq_t());

    lp_value_t res;
    lp_value_construct_none(&res);
    lp_value_get_value_between(lower.get_internal(), true, &v_upper, true, &res);

    lp_value_destruct(&v_upper);

    auto val = to_rational(&res);
    lp_value_destruct(&res);
    return val;
}

NumberType sample_between(const NumberType& lower, const LPRealAlgebraicNumber& upper) {
    CARL_LOG_DEBUG("carl.ran.libpoly", "Sampling between: " << lower << " and " << upper);

    lp_value_t v_lower;
    lp_value_construct(&v_lower, LP_VALUE_RATIONAL, lower.get_mpq_t());

    lp_value_t res;
    lp_value_construct_none(&res);
    lp_value_get_value_between(&v_lower, true, upper.get_internal(), true, &res);

    lp_value_destruct(&v_lower);

    auto val = to_rational(&res);
    lp_value_destruct(&res);
    return val;
}

NumberType floor(const LPRealAlgebraicNumber& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Floor of: " << n);
	lp_integer_t val;
    lp_integer_construct(&val);
	lp_value_floor(n.get_internal(), &val);
	NumberType ret = to_rational(&val);
	lp_integer_destruct(&val) ;
	return ret ;
}


NumberType ceil(const LPRealAlgebraicNumber& n) {
	CARL_LOG_DEBUG("carl.ran.libpoly", "Ceil of: " << n);
	lp_integer_t val;
    lp_integer_construct(&val);
	lp_value_ceiling(n.get_internal(), &val);
	NumberType ret = to_rational(&val);
	lp_integer_destruct(&val) ;
	return ret ;
}

bool compare(const LPRealAlgebraicNumber& lhs, const LPRealAlgebraicNumber& rhs, const Relation relation) {
    if (lhs.m_content == rhs.m_content) {
        switch (relation) {
        case Relation::EQ:
        case Relation::LEQ:
        case Relation::GEQ:
            return true;
        case Relation::NEQ:
        case Relation::LESS:
        case Relation::GREATER:
        default:
            return false;
        }
    }

	int cmp = lp_value_cmp(lhs.get_internal(), rhs.get_internal());
	switch (relation) {
	case Relation::EQ:
        if (cmp == 0) lhs.m_content = rhs.m_content;
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


const carl::Variable LPRealAlgebraicNumber::auxVariable = fresh_real_variable("__r");


bool compare(const LPRealAlgebraicNumber& lhs, const NumberType& rhs, const Relation relation) {
	int cmp = lp_value_cmp_rational(lhs.get_internal(), (lp_rational_t*)rhs.get_mpq_t());

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


std::ostream& operator<<(std::ostream& os, const LPRealAlgebraicNumber& ran) {
	char* str = lp_value_to_string(ran.get_internal());
	os << str;
	free(str);
	return os;
}

}

#endif
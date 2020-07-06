#include "ran_z3.h"

#ifdef RAN_USE_Z3

#include "../../../../converter/Z3Converter.h"

#include "Z3RanContent.h"

namespace carl {

template<typename Number>
real_algebraic_number_z3<Number>::real_algebraic_number_z3() {
    mContent = std::make_shared<Z3RanContent>();
}

template<typename Number>
real_algebraic_number_z3<Number>::real_algebraic_number_z3(const Z3RanContent& content) {
    mContent = std::make_shared<Z3RanContent>(content);
}

template<typename Number>
real_algebraic_number_z3<Number>::real_algebraic_number_z3(Z3RanContent&& content) {
    mContent = std::make_shared<Z3RanContent>(std::move(content));
}

template<typename Number>
real_algebraic_number_z3<Number>::real_algebraic_number_z3(const Number& r) : real_algebraic_number_z3() {
    mpq val = z3().toZ3MPQ(r);
    z3().anumMan().set(content(), val);
    z3().free(val);
}

template<typename Number>
bool real_algebraic_number_z3<Number>::is_numeric() const {
    return z3().anumMan().is_rational(content());
}
template<typename Number>
Number real_algebraic_number_z3<Number>::value() const {
    assert(is_numeric());
    mpq res;
    z3().anumMan().to_rational(content(), res);
    Number num = z3().toNumber<Number>(res);
    z3().free(res);
    return num;
}

template<typename Number>
bool real_algebraic_number_z3<Number>::is_zero() const {
    return z3().anumMan().is_zero(content());
}

template<typename Number>
bool real_algebraic_number_z3<Number>::is_integral() const {
    return z3().anumMan().is_int(content());
}


template<typename Number>
const Number real_algebraic_number_z3<Number>::lower() const {
    assert(!is_numeric());
    mpq res;
    z3().anumMan().get_lower(content(), res);
    auto lower = z3().toNumber<Number>(res);
    z3().free(res);
    return lower;
} 

template<typename Number>
const Number real_algebraic_number_z3<Number>::upper() const {
    assert(!is_numeric());
    mpq res;
    z3().anumMan().get_upper(content(), res);
    auto upper = z3().toNumber<Number>(res);
    z3().free(res);
    return upper;
} 

template<typename Number>
const Interval<Number> real_algebraic_number_z3<Number>::interval() const {
    assert(!is_numeric());
    const Number& lo = lower();
    const Number& up = upper();
    return Interval<Number>(lo, BoundType::STRICT, up, BoundType::STRICT);
}

template<typename Number>
Number branching_point(const real_algebraic_number_z3<Number>& n) {
    if (n.is_numeric()) {
        return n.value();
    } else {
        const Number& low = n.lower();
        const Number& up = n.upper();
        const Number& mid = (up-low)/2;
        
        const Number& midf = carl::floor(mid);
        if (low <= midf)
            return midf;
        const Number& midc = carl::ceil(mid);
        if (up >= midc)
            return midc;
        return mid;
    }
}

template<typename Number>
const UnivariatePolynomial<Number> real_algebraic_number_z3<Number>::polynomial() const {
    svector<mpz> res;
    z3().anumMan().get_polynomial(content(), res);
    auto polynomial = z3().toUnivPoly<Number>(res);
    for (unsigned i = 0; i < res.size(); i++) {
        z3().free(res[i]);
    }
    return polynomial;
}

template<typename Number>
Sign real_algebraic_number_z3<Number>::sgn() const {
    if (z3().anumMan().is_zero(content()))
        return Sign::ZERO;
    else if (z3().anumMan().is_pos(content()))
        return Sign::POSITIVE;
    else if (z3().anumMan().is_neg(content()))
        return Sign::NEGATIVE;
    else
        assert(false);
}

template<typename Number>
Sign real_algebraic_number_z3<Number>::sgn(const UnivariatePolynomial<Number>& p) const {
    polynomial::polynomial_ref poly = z3().toZ3IntCoeff(p);
    nlsat::assignment map(z3().anumMan());  // map frees its elements automatically
    polynomial::var var = z3().toZ3(p.mainVar());
    map.set(var, content());
    int rs = z3().anumMan().eval_sign_at(poly, map);
    if (rs < 0) return Sign::NEGATIVE;
    else if (rs == 0) return Sign::ZERO;
    else /* rs > 0 */ return Sign::POSITIVE;
}

template<typename Number>
bool real_algebraic_number_z3<Number>::contained_in(const Interval<Number>& i) const {
    if(i.lowerBoundType() != BoundType::INFTY) {
        if(i.lowerBoundType() == BoundType::STRICT &&
            z3().anumMan().le(content(), z3().toZ3MPQ(i.lower()))) return false;
        if(i.lowerBoundType() == BoundType::WEAK &&
            z3().anumMan().lt(content(), z3().toZ3MPQ(i.lower()))) return false;
    }
    if(i.upperBoundType() != BoundType::INFTY) {
        if(i.upperBoundType() == BoundType::STRICT &&
            z3().anumMan().ge(content(), z3().toZ3MPQ(i.upper()))) return false;
        if(i.upperBoundType() == BoundType::WEAK &&
            z3().anumMan().gt(content(), z3().toZ3MPQ(i.upper()))) return false;
    }
    return true;
}

template<typename Number>
real_algebraic_number_z3<Number> real_algebraic_number_z3<Number>::abs() const {
    if (z3().anumMan().is_pos(content())) {
        return *this;
    }
    else if (z3().anumMan().is_zero(content())) {
        return *this;
    }
    else if (z3().anumMan().is_neg(content())) {
        algebraic_numbers::anum res;
        z3().anumMan().set(res, content());
        z3().anumMan().neg(res);
        return real_algebraic_number_z3<Number>(std::move(res));
    }
    assert(false);
    return *this;
}

template<typename Number>
bool real_algebraic_number_z3<Number>::equal(const real_algebraic_number_z3<Number>& n) const {
    if (mContent.get() == n.mContent.get()) {
		return true;
	} else if (z3().anumMan().eq(content(),n.content())) {
        mContent = n.mContent;
        return true;
    } else {
        return false;
    }
}

template<typename Number>
bool real_algebraic_number_z3<Number>::less(const real_algebraic_number_z3<Number>& n) const {
    if (mContent.get() == n.mContent.get()) return false;
    return z3().anumMan().lt(content(),n.content());
}

template<typename Number>
bool real_algebraic_number_z3<Number>::equal(const Number& n) const {
    mpq zn = z3().toZ3MPQ(n);
    bool res = z3().anumMan().eq(content(), zn);
    z3().free(zn);
    return res;
}

template<typename Number>
bool real_algebraic_number_z3<Number>::less(const Number& n) const {
    mpq zn = z3().toZ3MPQ(n);
    bool res = z3().anumMan().lt(content(), zn);
    z3().free(zn);
    return res;
}

template<typename Number>
bool real_algebraic_number_z3<Number>::greater(const Number& n) const {
    mpq zn = z3().toZ3MPQ(n);
    bool res = z3().anumMan().gt(content(), zn);
    z3().free(zn);
    return res;
}

template<typename Number>
std::ostream& operator<<(std::ostream& os, const real_algebraic_number_z3<Number>& zr) {
    os << "(Z3 ";
    z3().anumMan().display_root(os, zr.content());
    os << ")";
    return os;
}

template<typename Number>
real_algebraic_number_z3<Number> sample_below(const real_algebraic_number_z3<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_lt(n.content(), val);
    return real_algebraic_number_z3<Number>(Z3RanContent(std::move(val)));
}

template<typename Number>
real_algebraic_number_z3<Number> sample_between(const real_algebraic_number_z3<Number>& lower, const real_algebraic_number_z3<Number>& upper) {
    algebraic_numbers::anum val;
    z3().anumMan().select(lower.content(), upper.content(), val);
    return real_algebraic_number_z3<Number>(Z3RanContent(std::move(val)));
}

template<typename Number>
real_algebraic_number_z3<Number> sample_above(const real_algebraic_number_z3<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_gt(n.content(), val);
    return real_algebraic_number_z3<Number>(Z3RanContent(std::move(val)));
}

template class real_algebraic_number_z3<mpq_class>;
template std::ostream& operator<<(std::ostream& os, const real_algebraic_number_z3<mpq_class>& zr);
template real_algebraic_number_z3<mpq_class> sample_below(const real_algebraic_number_z3<mpq_class>& n);
template real_algebraic_number_z3<mpq_class> sample_between(const real_algebraic_number_z3<mpq_class>& lower, const real_algebraic_number_z3<mpq_class>& upper);
template real_algebraic_number_z3<mpq_class> sample_above(const real_algebraic_number_z3<mpq_class>& n);

}

#endif
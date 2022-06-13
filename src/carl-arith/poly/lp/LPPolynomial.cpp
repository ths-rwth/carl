#include "LPPolynomial.h"

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

namespace carl {

LPPolynomial::LPPolynomial(const LPPolynomial& rhs)
    : mPoly(rhs.mPoly), mContext(rhs.mContext) {
    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(LPPolynomial&& rhs)
    : mPoly(std::move(rhs.mPoly)), mContext(std::move(rhs.mContext)) {
    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial& LPPolynomial::operator=(const LPPolynomial& rhs) {
    mPoly = rhs.mPoly;
    mContext = rhs.mContext;
    assert(lp_polynomial_check_order(mPoly.get_internal()));
    return *this;
}

LPPolynomial& LPPolynomial::operator=(LPPolynomial&& rhs) {
    mPoly = std::move(rhs.mPoly);
    mContext = std::move(rhs.mContext);
    assert(lp_polynomial_check_order(mPoly.get_internal()));
    return *this;
}

LPPolynomial::LPPolynomial(const LPContext& context)
    : mPoly(context.getContext()), mContext(context) {
    lp_polynomial_set_external(mPoly.get_internal());
    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const poly::Polynomial& p)
    : mPoly(p), mContext((lp_polynomial_context_t*)lp_polynomial_get_context(p.get_internal())) {
    lp_polynomial_set_external(mPoly.get_internal());

    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(poly::Polynomial&& p)
    : mPoly(std::move(p)) , mContext((lp_polynomial_context_t*)lp_polynomial_get_context(p.get_internal())) {
    lp_polynomial_set_external(mPoly.get_internal());

    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, long val) : mContext(context) {
    lp_polynomial_construct_simple(mPoly.get_internal(), context.getContext(), poly::Integer(val).get_internal(), 0, 0);
    lp_polynomial_set_external(mPoly.get_internal());

    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& var, const mpz_class& coeff, unsigned int degree) : mContext(context) {
    lp_polynomial_construct_simple(mPoly.get_internal(), context.getContext(), poly::Integer(coeff).get_internal(), VariableMapper::getInstance().getLibpolyVariable(var).get_internal(), degree);
    lp_polynomial_set_external(mPoly.get_internal());

    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& var) : mContext(context) {
    lp_polynomial_construct_simple(mPoly.get_internal(), context.getContext(), poly::Integer(1).get_internal(), VariableMapper::getInstance().getLibpolyVariable(var).get_internal(), 1);
    lp_polynomial_set_external(mPoly.get_internal());

    assert(lp_polynomial_check_order(mPoly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, const std::initializer_list<mpz_class>& coefficients)
    : mPoly(context.getContext()), mContext(context) {

    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
    unsigned int pow = coefficients.size();

    for (const mpz_class& coeff : coefficients) {
        pow--;
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(coeff).get_internal(), var, pow);
        mPoly += temp;
    }
    lp_polynomial_set_external(mPoly.get_internal());
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, const std::vector<mpz_class>& coefficients)
    : mPoly(context.getContext()), mContext(context) {
    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
    unsigned int pow = coefficients.size();

    for (const mpz_class& coeff : coefficients) {
        pow--;
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(coeff).get_internal(), var, pow);
        mPoly += temp;
    }
    lp_polynomial_set_external(mPoly.get_internal());
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, std::vector<mpz_class>&& coefficients)
    : mPoly(context.getContext()), mContext(context) {
    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
    unsigned int pow = coefficients.size();

    for (const mpz_class& coeff : coefficients) {
        pow--;
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(std::move(coeff)).get_internal(), var, pow);
        mPoly += temp;
    }
    lp_polynomial_set_external(mPoly.get_internal());
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, const std::map<unsigned int, mpz_class>& coefficients)
    : mPoly(context.getContext()), mContext(context) {
    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();

    for (const auto& coef : coefficients) {
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.getContext(), poly::Integer(coef.second).get_internal(), var, coef.first);
        mPoly += temp;
    }
    lp_polynomial_set_external(mPoly.get_internal());
}

bool LPPolynomial::has(const Variable& var) const {
    lp_variable_list_t varList;
    lp_variable_list_construct(&varList);
    lp_polynomial_get_variables(mPoly.get_internal(), &varList);
    poly::Variable lp_variable = VariableMapper::getInstance().getLibpolyVariable(var);
    bool contains = lp_variable_list_contains(&varList, lp_variable.get_internal());
    lp_variable_list_destruct(&varList);
    return contains;
}

bool operator==(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    return lp_polynomial_eq(lhs.get_internal(), rhs.get_internal());
}
bool operator==(const LPPolynomial& lhs, const mpz_class& rhs) {
    if (!lhs.isNumber()) {
        return false;
    }
    return lhs.constantPart() == rhs;
}
bool operator==(const mpz_class& lhs, const LPPolynomial& rhs) {
    return rhs == lhs;
}

bool operator!=(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    return !(lhs == rhs);
}
bool operator!=(const LPPolynomial& lhs, const mpz_class& rhs) {
    return !(lhs == rhs);
}
bool operator!=(const mpz_class& lhs, const LPPolynomial& rhs) {
    return !(lhs == rhs);
}

bool operator<(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    return lp_polynomial_cmp(lhs.get_internal(), rhs.get_internal()) < 0;
}
bool operator<(const LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    return lp_polynomial_cmp(lhs.get_internal(), tmp.get_internal()) < 0;
}
bool operator<(const mpz_class& lhs, const LPPolynomial& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(rhs.get_internal()));
    tmp += poly::Integer(lhs);
    return lp_polynomial_cmp(tmp.get_internal(), rhs.get_internal()) < 0;
}

bool operator<=(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    return lp_polynomial_cmp(lhs.get_internal(), rhs.get_internal()) <= 0;
}
bool operator<=(const LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    return lp_polynomial_cmp(lhs.get_internal(), tmp.get_internal()) <= 0;
}
bool operator<=(const mpz_class& lhs, const LPPolynomial& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(rhs.get_internal()));
    tmp += poly::Integer(lhs);
    return lp_polynomial_cmp(tmp.get_internal(), rhs.get_internal()) <= 0;
}

bool operator>(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    return lp_polynomial_cmp(lhs.get_internal(), rhs.get_internal()) > 0;
}
bool operator>(const LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    return lp_polynomial_cmp(lhs.get_internal(), tmp.get_internal()) > 0;
}
bool operator>(const mpz_class& lhs, const LPPolynomial& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(rhs.get_internal()));
    tmp += poly::Integer(lhs);
    return lp_polynomial_cmp(tmp.get_internal(), rhs.get_internal()) > 0;
}

bool operator>=(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    return lp_polynomial_cmp(lhs.get_internal(), rhs.get_internal()) >= 0;
}
bool operator>=(const LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    return lp_polynomial_cmp(lhs.get_internal(), tmp.get_internal()) >= 0;
}
bool operator>=(const mpz_class& lhs, const LPPolynomial& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(rhs.get_internal()));
    tmp += poly::Integer(lhs);
    return lp_polynomial_cmp(tmp.get_internal(), rhs.get_internal()) >= 0;
}

LPPolynomial operator+(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    assert(lp_polynomial_context_equal(lp_polynomial_get_context(lhs.get_internal()), lp_polynomial_get_context(rhs.get_internal())));
    LPPolynomial result(lp_polynomial_get_context(lhs.get_internal()));
    lp_polynomial_add(result.get_internal(), lhs.get_internal(), rhs.get_internal());
    return result;
}
LPPolynomial operator+(const LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    return lhs + tmp;
}
LPPolynomial operator+(const mpz_class& lhs, const LPPolynomial& rhs) {
    return rhs + lhs;
}

LPPolynomial operator-(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    assert(lp_polynomial_context_equal(lp_polynomial_get_context(lhs.get_internal()), lp_polynomial_get_context(rhs.get_internal())));
    LPPolynomial result(lp_polynomial_get_context(lhs.get_internal()));
    lp_polynomial_sub(result.get_internal(), lhs.get_internal(), rhs.get_internal());
    return result;
}
LPPolynomial operator-(const LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    return lhs - tmp;
}
LPPolynomial operator-(const mpz_class& lhs, const LPPolynomial& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(rhs.get_internal()));
    tmp += poly::Integer(lhs);
    return tmp - rhs;
}

LPPolynomial operator*(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    assert(lp_polynomial_context_equal(lp_polynomial_get_context(lhs.get_internal()), lp_polynomial_get_context(rhs.get_internal())));
    LPPolynomial result(lp_polynomial_get_context(lhs.get_internal()));
    lp_polynomial_mul(result.get_internal(), lhs.get_internal(), rhs.get_internal());
    return result;
}
LPPolynomial operator*(const LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    return lhs * tmp;
}
LPPolynomial operator*(const mpz_class& lhs, const LPPolynomial& rhs) {
    return rhs * lhs;
}

LPPolynomial& operator+=(LPPolynomial& lhs, const LPPolynomial& rhs) {
    assert(lp_polynomial_context_equal(lp_polynomial_get_context(lhs.get_internal()), lp_polynomial_get_context(rhs.get_internal())));
    lp_polynomial_add(lhs.get_internal(), lhs.get_internal(), rhs.get_internal());
    return lhs;
}
LPPolynomial& operator+=(LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    lp_polynomial_add(lhs.get_internal(), lhs.get_internal(), tmp.get_internal());
    return lhs;
}

LPPolynomial& operator-=(LPPolynomial& lhs, const LPPolynomial& rhs) {
    assert(lp_polynomial_context_equal(lp_polynomial_get_context(lhs.get_internal()), lp_polynomial_get_context(rhs.get_internal())));
    lp_polynomial_sub(lhs.get_internal(), lhs.get_internal(), rhs.get_internal());
    return lhs;
}
LPPolynomial& operator-=(LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    lp_polynomial_sub(lhs.get_internal(), lhs.get_internal(), tmp.get_internal());
    return lhs;
}

LPPolynomial& operator*=(LPPolynomial& lhs, const LPPolynomial& rhs) {
    assert(lp_polynomial_context_equal(lp_polynomial_get_context(lhs.get_internal()), lp_polynomial_get_context(rhs.get_internal())));
    lp_polynomial_mul(lhs.get_internal(), lhs.get_internal(), rhs.get_internal());
    return lhs;
}
LPPolynomial& operator*=(LPPolynomial& lhs, const mpz_class& rhs) {
    poly::Polynomial tmp(lp_polynomial_get_context(lhs.get_internal()));
    tmp += poly::Integer(rhs);
    lp_polynomial_mul(lhs.get_internal(), lhs.get_internal(), tmp.get_internal());
    return lhs;
}

mpz_class LPPolynomial::coprimeFactor() const {
    // TODO: can this be done with content/primitive part?
    struct coprime_factor_travers {
        std::vector<mpz_class> coefficients; // coefficients of the polynomial
    };

    auto getCoeffs = [](const lp_polynomial_context_t* ctx,
                        lp_monomial_t* m,
                        void* d) {
        coprime_factor_travers& v = *static_cast<coprime_factor_travers*>(d);
        v.coefficients.push_back(*reinterpret_cast<mpz_class*>(&m->a));
    };

    // first get the coefficients of every monomial
    coprime_factor_travers travers;
    lp_polynomial_traverse(get_internal(), getCoeffs, &travers);

    if (travers.coefficients.size() == 0) {
        return 0;
    }
    mpz_class res = travers.coefficients[0];
    for (size_t i = 1; i < travers.coefficients.size(); i++) {
        res = gcd(res, travers.coefficients[i]);
    }

    return res;
}

LPPolynomial LPPolynomial::coprimeCoefficients() const {

    mpz_class g = coprimeFactor();

    if (g == 1) {
        return *this ;
    }

    LPPolynomial temp(lp_polynomial_get_context(get_internal()));
    temp += g ; 

    return poly::div(getPolynomial(), temp.getPolynomial());
}

std::ostream& operator<<(std::ostream& os, const LPPolynomial& p) {
    os << p.mPoly;
    return os;
}

} // namespace carl

#endif
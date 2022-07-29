#include "LPPolynomial.h"

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

namespace carl {

LPPolynomial::LPPolynomial(const LPPolynomial& rhs)
    : m_poly(rhs.m_poly), m_context(rhs.m_context) {
    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(LPPolynomial&& rhs)
    : m_poly(std::move(rhs.m_poly)), m_context(std::move(rhs.m_context)) {
    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial& LPPolynomial::operator=(const LPPolynomial& rhs) {
    m_poly = rhs.m_poly;
    m_context = rhs.m_context;
    assert(lp_polynomial_check_order(m_poly.get_internal()));
    return *this;
}

LPPolynomial& LPPolynomial::operator=(LPPolynomial&& rhs) {
    m_poly = std::move(rhs.m_poly);
    m_context = std::move(rhs.m_context);
    assert(lp_polynomial_check_order(m_poly.get_internal()));
    return *this;
}

LPPolynomial::LPPolynomial(const LPContext& context)
    : m_poly(context.context()), m_context(context) {
    lp_polynomial_set_external(m_poly.get_internal());
    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(const poly::Polynomial& p)
    : m_poly(p), m_context((lp_polynomial_context_t*)lp_polynomial_get_context(p.get_internal())) {
    lp_polynomial_set_external(m_poly.get_internal());

    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(poly::Polynomial&& p)
    : m_poly(std::move(p)), m_context((lp_polynomial_context_t*)lp_polynomial_get_context(p.get_internal())) {
    lp_polynomial_set_external(m_poly.get_internal());

    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, long val)
    : m_context(context) {
    lp_polynomial_construct_simple(m_poly.get_internal(), context.context(), poly::Integer(val).get_internal(), 0, 0);
    lp_polynomial_set_external(m_poly.get_internal());

    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const mpz_class& val)
    : m_context(context) {
    
    lp_polynomial_construct_simple(m_poly.get_internal(), context.context(), val.get_mpz_t(), lp_variable_null, 0) ;


    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const mpq_class& val) : LPPolynomial(context, carl::get_num(val)) {}



LPPolynomial::LPPolynomial(const LPContext& context, const Variable& var, const mpz_class& coeff, unsigned int degree)
    : m_context(context) {
    lp_polynomial_construct_simple(m_poly.get_internal(), context.context(), poly::Integer(coeff).get_internal(), VariableMapper::getInstance().getLibpolyVariable(var).get_internal(), degree);
    lp_polynomial_set_external(m_poly.get_internal());

    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& var)
    : m_context(context) {
    lp_polynomial_construct_simple(m_poly.get_internal(), context.context(), poly::Integer(1).get_internal(), VariableMapper::getInstance().getLibpolyVariable(var).get_internal(), 1);
    lp_polynomial_set_external(m_poly.get_internal());

    assert(lp_polynomial_check_order(m_poly.get_internal()));
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, const std::initializer_list<mpz_class>& coefficients)
    : m_poly(context.context()), m_context(context) {

    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
    auto pow = coefficients.size();

    for (const mpz_class& coeff : coefficients) {
        pow--;
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.context(), poly::Integer(coeff).get_internal(), var, (unsigned int)pow);
        m_poly += temp;
    }
    lp_polynomial_set_external(m_poly.get_internal());
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, const std::vector<mpz_class>& coefficients)
    : m_poly(context.context()), m_context(context) {
    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
    auto pow = coefficients.size();

    for (const mpz_class& coeff : coefficients) {
        pow--;
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.context(), poly::Integer(coeff).get_internal(), var, (unsigned int)pow);
        m_poly += temp;
    }
    lp_polynomial_set_external(m_poly.get_internal());
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, std::vector<mpz_class>&& coefficients)
    : m_poly(context.context()), m_context(context) {
    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();
    auto pow = coefficients.size();

    for (const mpz_class& coeff : coefficients) {
        pow--;
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.context(), poly::Integer(std::move(coeff)).get_internal(), var, (unsigned int)pow);
        m_poly += temp;
    }
    lp_polynomial_set_external(m_poly.get_internal());
}

LPPolynomial::LPPolynomial(const LPContext& context, const Variable& mainVar, const std::map<unsigned int, mpz_class>& coefficients)
    : m_poly(context.context()), m_context(context) {
    assert(context.has(mainVar));
    lp_variable_t var = VariableMapper::getInstance().getLibpolyVariable(mainVar).get_internal();

    for (const auto& coef : coefficients) {
        poly::Polynomial temp;
        lp_polynomial_construct_simple(temp.get_internal(), context.context(), poly::Integer(coef.second).get_internal(), var, coef.first);
        m_poly += temp;
    }
    lp_polynomial_set_external(m_poly.get_internal());
}

bool LPPolynomial::has(const Variable& var) const {
    lp_variable_list_t varList;
    lp_variable_list_construct(&varList);
    lp_polynomial_get_variables(m_poly.get_internal(), &varList);
    poly::Variable lp_variable = VariableMapper::getInstance().getLibpolyVariable(var);
    bool contains = lp_variable_list_contains(&varList, lp_variable.get_internal());
    lp_variable_list_destruct(&varList);
    return contains;
}

bool operator==(const LPPolynomial& lhs, const LPPolynomial& rhs) {
    return lp_polynomial_eq(lhs.get_internal(), rhs.get_internal());
}
bool operator==(const LPPolynomial& lhs, const mpz_class& rhs) {
    if (!is_number(lhs)) {
        return false;
    }
    return lhs.constant_part() == rhs;
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

mpz_class LPPolynomial::coprime_factor() const {
    // TODO: can this be done with content/primitive part?
    struct coprime_factor_travers {
        std::vector<mpz_class> coefficients; // coefficients of the polynomial
    };

    auto getCoeffs = [](const lp_polynomial_context_t* /*ctx*/,
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

LPPolynomial LPPolynomial::coprime_coefficients() const {

    mpz_class g = coprime_factor();

    if (g == 1) {
        return *this;
    }

    LPPolynomial temp(lp_polynomial_get_context(get_internal()));
    temp += g;

    return poly::div(get_polynomial(), temp.get_polynomial());
}

std::size_t LPPolynomial::total_degree() const {

    struct degree_travers {
        std::size_t degree = 0;
    };

    auto getDegree = [](const lp_polynomial_context_t* /*ctx*/,
                        lp_monomial_t* m,
                        void* d) {
        degree_travers& v = *static_cast<degree_travers*>(d);

        size_t current_degree = 0;
        // iterate over the number of variables and add up their degrees
        for (size_t i = 0; i < m->n; i++) {
            current_degree += m->p[i].d;
        }
        v.degree = std::max(v.degree, current_degree);
    };

    degree_travers travers;
    lp_polynomial_traverse(get_internal(), getDegree, &travers);

    return travers.degree;
}

std::size_t LPPolynomial::degree(Variable::Arg var) const {
    struct degree_travers {
        std::size_t degree = 0;
        lp_variable_t var; // the variable we are looking for
    };

    auto getDegree = [](const lp_polynomial_context_t* /*ctx*/,
                        lp_monomial_t* m,
                        void* d) {
        degree_travers& v = *static_cast<degree_travers*>(d);

        size_t current_degree = 0;
        // iterate over the number of variables and add up their degrees
        for (size_t i = 0; i < m->n; i++) {
            if (m->p[i].x == v.var) {
                current_degree = m->p[i].d;
                break;
            }
        }
        v.degree = std::max(v.degree, current_degree);
    };

    degree_travers travers;
    travers.var = VariableMapper::getInstance().getLibpolyVariable(var).get_internal();
    lp_polynomial_traverse(get_internal(), getDegree, &travers);

    return travers.degree;
}

mpz_class LPPolynomial::unit_part() const {
    //As we can only have integer coefficients, they do not form a field
    //Thus the unit part is the sign of the leading coefficient, if it is not zero
    //Is the Poly is zero unit part is one
    if(is_zero(*this)) {
        return 1 ; 
    }
    return poly::lc_sgn(this->get_polynomial()) ;
    
}

LPPolynomial LPPolynomial::normalized() const {
    auto res = coprime_coefficients();
    auto unit = res.unit_part() ; 
    assert(!is_zero(unit)) ;
    return res * unit ;  

    /*
    auto unit = unit_part() ; 
    assert(!is_zero(unit)) ;
    return (*this) * unit ;  
    */
}

LPPolynomial LPPolynomial::coeff(Variable::Arg var, std::size_t exp) const {
    struct coeff_travers {
        std::vector<lp_monomial_t> coeff;
        const lp_polynomial_context_t* ctx; // context for the newly created monomials
        lp_variable_t var;            // the variable we are looking for
        std::size_t exp;              // the exponent we are looking for
    };


    auto getCoeff = [](const lp_polynomial_context_t* /*ctx*/,
                       lp_monomial_t* m,
                       void* d) {
        coeff_travers& v = *static_cast<coeff_travers*>(d);

        bool found = false;
        // iterate each monomials, and add the ones to add
        for (size_t i = 0; i < m->n; i++) {
            if (m->p[i].x == v.var && m->p[i].d == v.exp) {
                found = true;
                break;
            }
        }

        if (!found) {
            return;
        }

        // Make a copy (without var^exp) of each monomial and add it to the vector
        lp_monomial_t new_monomial;
        lp_monomial_construct(v.ctx, &new_monomial);
        new_monomial.n = m->n -1  ; // copy the number of variables (-1 because we remove var^exp)
        new_monomial.capacity = m->capacity ; // copy the capacity
        lp_integer_assign(v.ctx->K, &new_monomial.a ,&m->a); // copy the coefficient
        new_monomial.p = (power_t*)realloc(new_monomial.p, sizeof(power_t) * new_monomial.capacity); //allocate the memory for the power array

        size_t current_counter = 0 ; 
        for(size_t i = 0; i < m->n; i++){
            if(m->p[i].x == v.var && m->p[i].d == v.exp){
                continue;
            }
            new_monomial.p[current_counter].x = m->p[i].x;
            new_monomial.p[current_counter].d = m->p[i].d;
            current_counter++;
        }
        assert(current_counter == new_monomial.n);
        v.coeff.push_back(std::move(new_monomial));
    };

    coeff_travers travers;
    travers.var = VariableMapper::getInstance().getLibpolyVariable(var).get_internal();
    travers.exp = exp;
	travers.ctx = lp_polynomial_get_context(get_internal());
    lp_polynomial_traverse(get_internal(), getCoeff, &travers);

    LPPolynomial res(lp_polynomial_get_context(get_internal()));
    for (auto m : travers.coeff) {
        lp_polynomial_add_monomial(res.get_internal(), &m);
    }

    //free the memory allocated for the monomials
    for(auto& m : travers.coeff){
        lp_monomial_destruct(&m);
    }

    return res;
}

bool LPPolynomial::is_normal() const {
    return carl::is_one(this->unit_part()) ; 
    //return carl::is_one(carl::abs(this->unit_part())) ; 
}

std::ostream& operator<<(std::ostream& os, const LPPolynomial& p) {
    os << p.m_poly;
    return os;
}

} // namespace carl

#endif
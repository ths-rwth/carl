/**
 * @file LPPolynomial.h
 */

#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "LPContext.h"

#include <carl-arith/core/Variables.h>
#include <carl-logging/carl-logging.h>

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <poly/polyxx.h>
#include <vector>

#include <carl-arith/ran/libpoly/LPRan.h>
#include <carl-arith/converter/LibpolyVariableMapper.h>

namespace carl {

class LPPolynomial {
private:
	/// The libpoly polynomial.
	poly::Polynomial m_poly;

	LPContext m_context;

public:
	
	//Defines for real roots 
	using CoeffType = mpq_class ;
	using RootType = LPRealAlgebraicNumber;
	using ContextType = LPContext;

	//For compatibility with MultivariatePolynomial
	using NumberType = mpq_class;

	/**
	 * Default constructor shall not exist. Use LPPolynomial(Context) instead.
	 */
	LPPolynomial() = delete;
	/**
	 * Copy constructor.
	 */
	LPPolynomial(const LPPolynomial& p);
	/**
	 * Move constructor.
	 */
	LPPolynomial(LPPolynomial&& p);
	/**
	 * Copy assignment operator.
	 */
	LPPolynomial& operator=(const LPPolynomial& p);
	/**
	 * Move assignment operator.
	 */
	LPPolynomial& operator=(LPPolynomial&& p);

	/**
	 * Construct a zero polynomial with the given main variable.
	 * @param context Context of libpoly polynomial
	 */
	explicit LPPolynomial(const LPContext& context);

	/**
	 * Construct a LPPolynomial with the given libpoly polynomial.
	 * Also uses the given context.
	 * @param mainPoly Libpoly Polynomial.
	 */
	LPPolynomial(const poly::Polynomial& mainPoly);

	/**
	 * Moves a LPPolynomial with the given libpoly polynomial.
	 * Also uses the given context.
	 * @param mainPoly Libpoly Polynomial.
	 */
	LPPolynomial(poly::Polynomial&& mainPoly);

	/**
	 * Construct a LPPolynomial with only a integer.
	 * @param mainPoly Libpoly Polynomial.
	 */
	LPPolynomial(const LPContext& context, long val);

	/**
	 * Construct a LPPolynomial with only a rational.
	 * Attention: Just the numerator is taken!
	 * @param mainPoly Libpoly Polynomial.
	 */
	LPPolynomial(const LPContext& context, const mpq_class& val);

	/**
	 * Construct a LPPolynomial with only a integer.
	 * @param mainPoly Libpoly Polynomial.
	 */
	LPPolynomial(const LPContext& context, const mpz_class& val);

	/**
	 * Construct from context and variable
	 *  @param context Context of libpoly polynomial
	 * @param var The main variable of the polynomial
	 */
	LPPolynomial(const LPContext& context, const Variable& var);

	/**
	 * Construct \f$ coeff \cdot mainVar^{degree} \f$.
	 * @param mainVar New main variable.
	 * @param context Context of libpoly polynomial
	 * @param coeff Leading coefficient.
	 * @param degree Degree.
	 */
	LPPolynomial(const LPContext& context, const Variable& var, const mpz_class& coeff, unsigned int degree = 0);

	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients List of coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const LPContext& context, const Variable& mainVar, const std::initializer_list<mpz_class>& coefficients);

	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Vector of coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const LPContext& context, const Variable& mainVar, const std::vector<mpz_class>& coefficients);
	/**
	 * Construct polynomial with the given coefficients, moving the coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Vector of coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const LPContext& context, const Variable& mainVar, std::vector<mpz_class>&& coefficients);
	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Assignment of degree to coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const LPContext& context, const Variable& mainVar, const std::map<unsigned int, mpz_class>& coefficients);

	/**
	 * Destructor.
	 */
	~LPPolynomial() = default;

	// Polynomial interface implementations.

	/**
	 * Creates a polynomial of value one with the same context
	 * @return One.
	 */
	LPPolynomial one() const {
		// Construct zero-poly with the same context
		poly::Polynomial temp(lp_polynomial_get_context(m_poly.get_internal()));
		temp += poly::Integer(1);
		return temp;
	}

	/**
	 * For terms with exactly one variable, get this variable.
	 * @return The only variable occuring in the term.
	 */
	Variable single_variable() const {
		assert(poly::is_univariate(m_poly));
		return VariableMapper::getInstance().getCarlVariable(poly::main_variable(m_poly));
	}

	/**
	 * Returns the leading coefficient.
	 * @return The leading coefficient.
	 */
	LPPolynomial lcoeff() const {
		return LPPolynomial(poly::leading_coefficient(m_poly));
	}

	/** Obtain all non-zero coefficients of a polynomial. */
	std::vector<LPPolynomial> coefficients() const {
		std::vector<LPPolynomial> res;
		for (std::size_t deg = 0; deg <= poly::degree(m_poly); ++deg) {
			auto coeff = poly::coefficient(m_poly, deg);
			if (lp_polynomial_is_zero(coeff.get_internal())) continue;
			res.emplace_back(std::move(coeff));
		}
		return res;
	}

	/**
	 * Returns the constant part of this polynomial.
	 * @return Constant part.
	 */
	mpz_class constant_part() const {
		struct LPPolynomial_constantPart_visitor {
			mpz_class part = 0;
		};

		auto getConstantPart = [](const lp_polynomial_context_t* /*ctx*/,
								  lp_monomial_t* m,
								  void* d) {
			auto& v = *static_cast<LPPolynomial_constantPart_visitor*>(d);
			if (m->n == 0) {
				v.part += *reinterpret_cast<mpz_class*>(&m->a);
			}
		};

		LPPolynomial_constantPart_visitor visitor;
		lp_polynomial_traverse(m_poly.get_internal(), getConstantPart, &visitor);
		return visitor.part;
	}

	/**
	 * Get the maximal exponent of the main variable.
	 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
	 * @see @cite GCL92, page 38
	 * @return Degree.
	 */
	size_t degree() const {
		return poly::degree(m_poly);
	}

	/**
	 * Removes the leading term from the polynomial.
	 */
	void truncate() {
		m_poly -= poly::leading_coefficient(m_poly);
	}

	/**
	 * Retrieves the main variable of this polynomial.
	 * @return Main variable.
	 */
	Variable main_var() const {
		if (poly::is_constant(m_poly)) return carl::Variable::NO_VARIABLE;
		else return VariableMapper::getInstance().getCarlVariable(poly::main_variable(m_poly));
	}

	/**
	 * Retrieves a non-const pointer to the libpoly polynomial.
	 * [Handle with care]
	 * @return Libpoly Polynomial.
	 */
	lp_polynomial_t* get_internal() {
		return m_poly.get_internal();
	}

	/**
	 * Retrieves a constant pointer to the libpoly polynomial.
	 * @return Libpoly Polynomial.
	 */
	const lp_polynomial_t* get_internal() const {
		return m_poly.get_internal();
	}

	/**
	 * @brief Get the underlying Polynomial object
	 *
	 * @return const poly::Polynomial&
	 */
	const poly::Polynomial& get_polynomial() const {
		return m_poly;
	}

	/**
	 * @brief Get the Context object
	 *
	 * @return const LPContext
	 */
	const LPContext& context() const {
		return m_context;
	}

	/**
	 * @brief Get the Context object
	 *
	 * @return LPContext
	 */
	LPContext& context() {
		return m_context;
	}

	/**
	 * Checks if the given variable occurs in the polynomial.
	 * @param v Variable.
	 * @return If v occurs in the polynomial.
	 */
	bool has(const Variable& v) const;

	/**
	 * Calculates a factor that would make the coefficients of this polynomial coprime integers.
	 *
	 * We consider a set of integers coprime, if they share no common factor.
	 * As we can only have integer coefficients, we calculate the gcd of the coefficients of the monomial
	 * @return Coprime factor of this polynomial.
	 */
	mpz_class coprime_factor() const;

	/**
	 * Constructs a new polynomial that is scaled such that the coefficients are coprime.
	 * It is calculated by multiplying it with the coprime factor.
	 * By definition, this results in a polynomial with integral coefficients.
	 * @return This polynomial multiplied with the coprime factor.
	 */

	LPPolynomial coprime_coefficients() const;

	/**
	 * Checks whether the polynomial is unit normal.
	 * A polynomial is unit normal, if the leading coefficient is unit normal, that is if it is either one or minus one.
	 * @see @cite GCL92, page 39
	 * @return If polynomial is normal.
	 */
	bool is_normal() const;
	/**
	 * The normal part of a polynomial is the polynomial divided by the unit part.
	 * 
	 * HACK: At the moment, this is equal to coprime_coefficients().
	 * @see @cite GCL92, page 42.
	 * @return This polynomial divided by the unit part.
	 */
	LPPolynomial normalized() const;

	/**
	 * The unit part of a polynomial over a field is its leading coefficient for nonzero polynomials,
	 * and one for zero polynomials.
	 * The unit part of a polynomial over a ring is the sign of the polynomial for nonzero polynomials,
	 * and one for zero polynomials.
	 * @see @cite GCL92, page 42.
	 * @return The unit part of the polynomial.
	 */
	mpz_class unit_part() const;

	/**
	 * Constructs a new polynomial `q` such that \f$ q(x) = p(-x) \f$ where `p` is this polynomial.
	 * @return New polynomial with negated variable.
	 */
	LPPolynomial negate_variable() const {
		CARL_LOG_NOTIMPLEMENTED();
		return LPPolynomial(m_context);
	}

	/**
	 * Checks if this polynomial is divisible by the given divisor, that is if the remainder is zero.
	 * @param divisor Polynomial.
	 * @return If divisor divides this polynomial.
	 */
	bool divides(const LPPolynomial& divisor) const;

	/**
	 * Replaces every coefficient `c` by `c mod modulus`.
	 * @param modulus Modulus.
	 * @return This.
	 */
	LPPolynomial& mod(const mpz_class& modulus);
	/**
	 * Constructs a new polynomial where every coefficient `c` is replaced by `c mod modulus`.
	 * @param modulus Modulus.
	 * @return New polynomial.
	 */
	LPPolynomial mod(const mpz_class& modulus) const;

	/**
	 * Compute the main denominator of all numeric coefficients of this polynomial.
	 * This method only applies if the Coefficient type is a number.
	 * @return the main denominator of all coefficients of this polynomial.
	 */
	mpz_class main_denom() const {
		CARL_LOG_NOTIMPLEMENTED();
		return mpz_class(0);
	}

	std::size_t total_degree() const ;

	std::size_t degree(Variable::Arg var) const ;

	/**
	 * Calculates the coefficient of var^exp.
	 * @param var Variable.
	 * @param exp Exponent.
	 * @return Coefficient of var^exp.
	 */
	LPPolynomial coeff(Variable::Arg var, std::size_t exp) const ;

	friend std::ostream& operator<<(std::ostream& os, const LPPolynomial& rhs);
};

bool operator==(const LPPolynomial& lhs, const LPPolynomial& rhs);
bool operator==(const LPPolynomial& lhs, const mpz_class& rhs);
bool operator==(const mpz_class& lhs, const LPPolynomial& rhs);

bool operator!=(const LPPolynomial& lhs, const LPPolynomial& rhs);
bool operator!=(const LPPolynomial& lhs, const mpz_class& rhs);
bool operator!=(const mpz_class& lhs, const LPPolynomial& rhs);

bool operator<(const LPPolynomial& lhs, const LPPolynomial& rhs);
bool operator<(const LPPolynomial& lhs, const mpz_class& rhs);
bool operator<(const mpz_class& lhs, const LPPolynomial& rhs);

bool operator<=(const LPPolynomial& lhs, const LPPolynomial& rhs);
bool operator<=(const LPPolynomial& lhs, const mpz_class& rhs);
bool operator<=(const mpz_class& lhs, const LPPolynomial& rhs);

bool operator>(const LPPolynomial& lhs, const LPPolynomial& rhs);
bool operator>(const LPPolynomial& lhs, const mpz_class& rhs);
bool operator>(const mpz_class& lhs, const LPPolynomial& rhs);

bool operator>=(const LPPolynomial& lhs, const LPPolynomial& rhs);
bool operator>=(const LPPolynomial& lhs, const mpz_class& rhs);
bool operator>=(const mpz_class& lhs, const LPPolynomial& rhs);

LPPolynomial operator+(const LPPolynomial& lhs, const LPPolynomial& rhs);
LPPolynomial operator+(const LPPolynomial& lhs, const mpz_class& rhs);
LPPolynomial operator+(const mpz_class& lhs, const LPPolynomial& rhs);

LPPolynomial operator-(const LPPolynomial& lhs, const LPPolynomial& rhs);
LPPolynomial operator-(const LPPolynomial& lhs, const mpz_class& rhs);
LPPolynomial operator-(const mpz_class& lhs, const LPPolynomial& rhs);

LPPolynomial operator*(const LPPolynomial& lhs, const LPPolynomial& rhs);
LPPolynomial operator*(const LPPolynomial& lhs, const mpz_class& rhs);
LPPolynomial operator*(const mpz_class& lhs, const LPPolynomial& rhs);

LPPolynomial& operator+=(LPPolynomial& lhs, const LPPolynomial& rhs);
LPPolynomial& operator+=(LPPolynomial& lhs, const mpz_class& rhs);

LPPolynomial& operator-=(LPPolynomial& lhs, const LPPolynomial& rhs);
LPPolynomial& operator-=(LPPolynomial& lhs, const mpz_class& rhs);

LPPolynomial& operator*=(LPPolynomial& lhs, const LPPolynomial& rhs);
LPPolynomial& operator*=(LPPolynomial& lhs, const mpz_class& rhs);

/**
 * Checks if the polynomial is equal to zero.
 * @return If polynomial is zero.
 */
inline bool is_zero(const LPPolynomial& p) {
	return poly::is_zero(p.get_polynomial());
}

// bool isNegative(LPPolynomial<mpz_class>& p) {
// 	// return poly::is_zero(*p.mainPoly());
// 	return true;
// }

/**
 * Checks if the polynomial is linear or not.
 * @return If polynomial is linear.
 */
inline bool is_constant(const LPPolynomial& p) {
	return poly::is_constant(p.get_polynomial());
}

/**
 * Checks if the polynomial is equal to one.
 * @return If polynomial is one.
 */
inline bool is_one(const LPPolynomial& p) {
	if (!is_constant(p)) {
		return false;
	}
	poly::Polynomial temp(lp_polynomial_get_context(p.get_internal()));
	temp += poly::Integer(mpz_class(1));
	return lp_polynomial_eq(p.get_internal(), temp.get_internal());
}

/**
 * Checks whether the polynomial is only a number.
 * @return If polynomial is a number.
 */
inline bool is_number(const LPPolynomial& p) {
	return is_constant(p);
}

/**
 * @brief Check if the given polynomial is linear.
 */
inline bool is_linear(const LPPolynomial& p) {
	return poly::is_linear(p.get_polynomial());
}

/**
 * @brief Check if the given polynomial is univariate.
 */
inline bool is_univariate(const LPPolynomial& p) {
	return poly::is_univariate(p.get_polynomial());
}

inline std::size_t level_of(const LPPolynomial& p) {
	if (is_number(p)) return 0;
	auto it = std::find(p.context().variable_ordering().begin(), p.context().variable_ordering().end(), p.main_var());
	assert(it != p.context().variable_ordering().end());
	return std::distance(p.context().variable_ordering().begin(), it)+1;
}

/// Add the variables of the given polynomial to the variables.
inline void variables(const LPPolynomial& p, carlVariables& vars) {
	// vars.clear();

	auto collectVars = [](const lp_polynomial_context_t* /*ctx*/,
						  lp_monomial_t* m,
						  void* d) {
		carlVariables* varList = static_cast<carlVariables*>(d);
		for (size_t i = 0; i < m->n; i++) {
			carl::Variable var = VariableMapper::getInstance().getCarlVariable(m->p[i].x);
			varList->add(var);
		}
	};

	lp_polynomial_traverse(p.get_internal(), collectVars, &vars);
	return;
}

template<>
struct needs_context_type<LPPolynomial> : std::true_type {};

template<>
struct is_polynomial_type<LPPolynomial> : std::true_type {};

} // namespace carl

namespace std {
/**
 * Specialization of `std::hash` for univariate polynomials.
 */
template<>
struct hash<carl::LPPolynomial> {
	/**
	 * Calculates the hash of univariate polynomial.s
	 * @param p LPPolynomial.
	 * @return Hash of p.
	 */
	std::size_t operator()(const carl::LPPolynomial& p) const {
		return lp_polynomial_hash(p.get_internal());
	}
};

} // namespace std

#endif
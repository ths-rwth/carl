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

#include <carl-arith/ran/libpoly/ran_libpoly.h>

namespace carl {

class LPPolynomial {
private:
	/// The libpoly polynomial.
	poly::Polynomial mPoly;

	LPContext mContext;

public:
	
	//Defines for real roots 
	using CoeffType = mpq_class ;
	template<typename Number>
	using RootType = RealAlgebraicNumberLibpoly<Number>;

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
		poly::Polynomial temp(lp_polynomial_get_context(mPoly.get_internal()));
		temp += poly::Integer(1);
		return temp;
	}

	/**
	 * For terms with exactly one variable, get this variable.
	 * @return The only variable occuring in the term.
	 */
	Variable getSingleVariable() const {
		assert(poly::is_univariate(mPoly));
		return VariableMapper::getInstance().getCarlVariable(poly::main_variable(mPoly));
	}

	/**
	 * Returns the leading coefficient Numerical Value.
	 * @return The leading coefficient.
	 */
	const mpz_class lcoeff() const {
		// TODO: Libpoly has a different Monomial order, this the leading coefficient is different.

		struct LPPolynomial_lcoeff_visitor {
			mpz_class coeff = 1;
		};

		auto getCoeff = [](const lp_polynomial_context_t* ctx,
						   lp_monomial_t* m,
						   void* d) {
			auto& v = *static_cast<LPPolynomial_lcoeff_visitor*>(d);
			v.coeff *= *reinterpret_cast<mpz_class*>(&m->a);
		};

		poly::Polynomial leadingPoly = poly::leading_coefficient(mPoly); // This can also have other variables
		LPPolynomial_lcoeff_visitor visitor;
		lp_polynomial_traverse(leadingPoly.get_internal(), getCoeff, &visitor);
		return visitor.coeff;
	};
	/**
	 * Returns the leading coefficient.
	 * @return The leading coefficient.
	 */
	LPPolynomial lterm() const {
		return LPPolynomial(poly::leading_coefficient(mPoly));
	}

	/**
	 * Checks whether the polynomial is only a number.
	 * @return If polynomial is a number.
	 */
	bool isNumber() const {
		return poly::is_constant(mPoly);
	}

	/**
	 * Returns the constant part of this polynomial.
	 * @return Constant part.
	 */
	mpz_class constant_part() const {
		struct LPPolynomial_constantPart_visitor {
			mpz_class part = 0;
		};

		auto getConstantPart = [](const lp_polynomial_context_t* ctx,
								  lp_monomial_t* m,
								  void* d) {
			auto& v = *static_cast<LPPolynomial_constantPart_visitor*>(d);
			if (m->n == 0) {
				v.part += *reinterpret_cast<mpz_class*>(&m->a);
			}
		};

		LPPolynomial_constantPart_visitor visitor;
		lp_polynomial_traverse(mPoly.get_internal(), getConstantPart, &visitor);
		return visitor.part;
	}

	/**
	 * Get the maximal exponent of the main variable.
	 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
	 * @see @cite GCL92, page 38
	 * @return Degree.
	 */
	size_t degree() const {
		return poly::degree(mPoly);
	}

	/**
	 * Removes the leading term from the polynomial.
	 */
	void truncate() {
		// TODO: leading coefficient is not the same as the leading term in libpoly... right?
		mPoly -= poly::leading_coefficient(mPoly);
	}

	/**
	 * Retrieves the main variable of this polynomial.
	 * @return Main variable.
	 */
	Variable mainVar() const {
		assert(!isNumber());
		return VariableMapper::getInstance().getCarlVariable(poly::main_variable(mPoly));
	}

	/**
	 * Retrieves a non-const pointer to the libpoly polynomial.
	 * [Handle with care]
	 * @return Libpoly Polynomial.
	 */
	lp_polynomial_t* get_internal() {
		return mPoly.get_internal();
	}

	/**
	 * Retrieves a constant pointer to the libpoly polynomial.
	 * @return Libpoly Polynomial.
	 */
	const lp_polynomial_t* get_internal() const {
		return mPoly.get_internal();
	}

	/**
	 * @brief Get the underlying Polynomial object
	 *
	 * @return const poly::Polynomial&
	 */
	const poly::Polynomial& getPolynomial() const {
		return mPoly;
	}

	/**
	 * @brief Get the Context object
	 *
	 * @return const LPContext
	 */
	const LPContext getContext() const {
		return mContext;
	}

	/**
	 * @brief Get the Context object
	 *
	 * @return LPContext
	 */
	LPContext getContext() {
		return mContext;
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
	mpz_class coprimeFactor() const;

	/**
	 * Constructs a new polynomial that is scaled such that the coefficients are coprime.
	 * It is calculated by multiplying it with the coprime factor.
	 * By definition, this results in a polynomial with integral coefficients.
	 * @return This polynomial multiplied with the coprime factor.
	 */

	LPPolynomial coprimeCoefficients() const;

	/**
	 * Checks whether the polynomial is unit normal.
	 * A polynomial is unit normal, if the leading coefficient is unit normal, that is if it is either one or minus one.
	 * @see @cite GCL92, page 39
	 * @return If polynomial is normal.
	 */
	bool isNormal() const;
	/**
	 * The normal part of a polynomial is the polynomial divided by the unit part.
	 * @see @cite GCL92, page 42.
	 * @return This polynomial divided by the unit part.
	 */
	LPPolynomial normalized() const;

	/**
	 * For a polynomial p, returns p/lc(p)
	 * @return
	 */
	LPPolynomial normalize() const;

	/**
	 * The unit part of a polynomial over a field is its leading coefficient for nonzero polynomials,
	 * and one for zero polynomials.
	 * The unit part of a polynomial over a ring is the sign of the polynomial for nonzero polynomials,
	 * and one for zero polynomials.
	 * @see @cite GCL92, page 42.
	 * @return The unit part of the polynomial.
	 */
	mpz_class unitPart() const;

	/**
	 * Constructs a new polynomial `q` such that \f$ q(x) = p(-x) \f$ where `p` is this polynomial.
	 * @return New polynomial with negated variable.
	 */
	LPPolynomial negateVariable() const {
		CARL_LOG_NOTIMPLEMENTED();
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
	mpz_class mainDenom() const {
		CARL_LOG_NOTIMPLEMENTED();
	}

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
inline bool is_zero(LPPolynomial& p) {
	return lp_polynomial_is_zero(p.get_internal());
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
	return lp_polynomial_is_constant(p.get_internal());
}

/**
 * Checks if the polynomial is equal to one.
 * @return If polynomial is one.
 */
inline bool is_one(LPPolynomial& p) {
	if (!is_constant(p)) {
		return false;
	}
	poly::Polynomial temp(lp_polynomial_get_context(p.get_internal()));
	temp += poly::Integer(mpz_class(1));
	return lp_polynomial_eq(p.get_internal(), temp.get_internal());
}

/// Add the variables of the given polynomial to the variables.
inline void variables(const LPPolynomial& p, carlVariables& vars) {
	vars.clear();

	auto collectVars = [](const lp_polynomial_context_t* ctx,
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
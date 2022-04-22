/**
 * @file LPPolynomial.h
 */

#pragma once

#include "../../core/Polynomial.h"
#include "../../core/Sign.h"
#include "../../core/Variable.h"
#include "../../core/VariableInformation.h"
#include "../../numbers/numbers.h"
#include "../../util/SFINAE.h"
#include "../../util/hash.h"
#include "LPContext.h"

#include <carl-logging/carl-logging.h>

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <poly/polyxx.h>
#include <vector>

namespace carl {

} // namespace carl

namespace carl {

class LPPolynomial : public Polynomial {
private:
	/// The libpoly polynomial.
	poly::Polynomial mPoly;

public:
	// Rule of five
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
	LPPolynomial(long val, const LPContext& context);

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
	LPPolynomial(const LPContext& context, const Variable& var, const mpz_class& coeff, std::size_t degree = 0);

	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients List of coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const Variable& mainVar, const LPContext& context, const std::initializer_list<mpz_class>& coefficients);

	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Vector of coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const Variable& mainVar, const LPContext& context, const std::vector<mpz_class>& coefficients);
	/**
	 * Construct polynomial with the given coefficients, moving the coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Vector of coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const Variable& mainVar, const LPContext& context, std::vector<mpz_class>&& coefficients);
	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Assignment of degree to coefficients.
	 * @param context Context of libpoly polynomial
	 */
	LPPolynomial(const Variable& mainVar, const LPContext& context, const std::map<uint, mpz_class>& coefficients);

	/**
	 * Destructor.
	 */
	~LPPolynomial() override = default;

	// Polynomial interface implementations.

	/**
	 * Checks if the polynomial is represented univariately.
	 * @see Polynomial#isUnivariateRepresented
	 * @return true.
	 */
	bool isUnivariateRepresented() const override {
		return poly::is_univariate(mPoly);
	}

	/**
	 * Checks if the polynomial is represented multivariately.
	 * @see Polynomial#isMultivariateRepresented
	 * @return false.
	 */
	bool isMultivariateRepresented() const override {
		return !isUnivariateRepresented();
	}

    bool isUnivariate() const {
        return isUnivariateRepresented();
    }

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
		assert(isUnivariate());
		return VariableMapper::getInstance().getCarlVariable(main_variable(mPoly));
	}

	/**
	 * Returns the leading coefficient Numerical Value.
	 * @return The leading coefficient.
	 */
	const mpz_class lcoeff() const {

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

	}; // TODO use poly::lcoeff and then traverse the given monomial for constant coefficient.
	/**
	 * Returns the leading coefficient.
	 * @return The leading coefficient.
	 */
	const LPPolynomial lterm() const {
		// Todo: make this return a reference?
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
	mpz_class constantPart() const {
		CARL_LOG_NOTIMPLEMENTED();
	}

	/**
	 * Get the maximal exponent of the main variable.
	 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
	 * @see @cite GCL92, page 38
	 * @return Degree.
	 */
	uint degree() const {
		return poly::degree(mPoly);
	}

	/**
	 * Removes the leading term from the polynomial.
	 */
	void truncate() {
		CARL_LOG_NOTIMPLEMENTED();
	}

	/**
	 * Retrieves the main variable of this polynomial.
	 * @return Main variable.
	 */
	Variable mainVar() const {
		return VariableMapper::getInstance().getCarlVariable(poly::main_variable(mPoly));
	}

	/**
	 * Retrieves a non-const pointer to the libpoly polynomial.
	 * [Handle with care]
	 * @return Libpoly Polynomial.
	 */
	poly::Polynomial* mainPoly() {
		return &mPoly;
	}

	/**
	 * Retrieves a constant pointer to the libpoly polynomial.
	 * @return Libpoly Polynomial.
	 */
	const poly::Polynomial* mainPoly() const {
		return &mPoly;
	}

	/**
	 * Checks if the given variable occurs in the polynomial.
	 * @param v Variable.
	 * @return If v occurs in the polynomial.
	 */
	bool has(Variable v) const {
		CARL_LOG_NOTIMPLEMENTED();
	}

	/**
	 * Calculates a factor that would make the coefficients of this polynomial coprime integers.
	 *
	 * We consider a set of integers coprime, if they share no common factor.
	 * Technically, the coprime factor is \f$ lcm(N) / gcd(D) \f$ where `N` is the set of the numerators and `D` is the set of the denominators of all coefficients.
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

	LPPolynomial coprimeCoefficientsSignPreserving() const;

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
	 * Reverse coefficients safely.
	 */
	LPPolynomial reverseCoefficients() const {
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

	LPPolynomial evaluateCoefficient(const std::map<Variable, mpz_class>&) const {
		CARL_LOG_NOTIMPLEMENTED();
	}


	/**
	 * Returns the numeric content part of the i'th coefficient.
	 *
	 * If the coefficients are numbers, this is simply the i'th coefficient.
	 * If the coefficients are polynomials, this is the numeric content part of the i'th coefficient.
	 * @param i number of the coefficient
	 * @return numeric content part of i'th coefficient.
	 */
	mpz_class numericContent(std::size_t i) const {
		CARL_LOG_NOTIMPLEMENTED();
	}

	/**
	 * Returns the numeric unit part of the polynomial.
	 *
	 * If the coefficients are numbers, this is the sign of the leading coefficient.
	 * If the coefficients are polynomials, this is the unit part of the leading coefficient.s
	 * @return unit part of the polynomial.
	 */
	mpz_class numericUnit() const {
		CARL_LOG_NOTIMPLEMENTED();
	}

	/**
	 * Obtains the numeric content part of this polynomial.
	 *
	 * The numeric content part of a polynomial is defined as the gcd() of the numeric content parts of all coefficients.
	 * This is only possible if the underlying number type is either integral or fractional.
	 *
	 * As for fractional numbers, we consider the following definition:
	 *		gcd( a/b, c/d ) = gcd( a/b*l, c/d*l ) / l
	 * where l = lcm(b,d).
	 * @return numeric content part of the polynomial.
	 * @see LPPolynomials::numericContent(std::size_t)
	 */
	mpz_class numericContent() const{
        CARL_LOG_NOTIMPLEMENTED();
    }

	/**
	 * Compute the main denominator of all numeric coefficients of this polynomial.
	 * This method only applies if the Coefficient type is a number.
	 * @return the main denominator of all coefficients of this polynomial.
	 */
	mpz_class mainDenom() const{
        CARL_LOG_NOTIMPLEMENTED();
    }

	mpz_class syntheticDivision(const mpz_class& zeroOfDivisor){
        CARL_LOG_NOTIMPLEMENTED();
    }

	/**
	 * Checks if zero is a real root of this polynomial.
	 * @return True if zero is a root.
	 */
	bool zeroIsRoot() const {
		CARL_LOG_NOTIMPLEMENTED();
	}

	/// @name Equality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are equal.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs == rhs`
	 */
	friend bool operator==(const mpz_class& lhs, const LPPolynomial& rhs){
        if(!rhs.isNumber()) return false ; 
        return lhs == rhs.lcoeff();
    }
    friend bool operator==(const LPPolynomial& lhs, const mpz_class& rhs){
       return rhs == lhs;
    }
	friend bool operator==(const LPPolynomial& lhs, const LPPolynomial& rhs){
        return *lhs.mainPoly() == *rhs.mainPoly();

    }
	/// @}

	/// @name Inequality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are not equal.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs != rhs`
	 */
	friend bool operator!=(const LPPolynomial& lhs, const LPPolynomial& rhs);

	bool less(const LPPolynomial& rhs, const PolynomialComparisonOrder& order = PolynomialComparisonOrder::Default) const;

	friend bool operator<(const LPPolynomial& lhs, const LPPolynomial& rhs);

	LPPolynomial operator-() const;

	/// @name In-place addition operators
	/// @{
	/**
	 * Add something to this polynomial and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	LPPolynomial& operator+=(const mpz_class& rhs);
	LPPolynomial& operator+=(const LPPolynomial& rhs);
	/// @}

	/// @name Addition operators
	/// @{
	/**
	 * Performs an addition involving a polynomial.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs + rhs`
	 */
	friend LPPolynomial operator+(const LPPolynomial& lhs, const LPPolynomial& rhs);
	friend LPPolynomial operator+(const mpz_class& lhs, const LPPolynomial& rhs);
	/// @}

	/// @name In-place subtraction operators
	/// @{
	/**
	 * Subtract something from this polynomial and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	LPPolynomial& operator-=(const mpz_class& rhs);
	LPPolynomial& operator-=(const LPPolynomial& rhs);
	/// @}

	/// @name Subtraction operators
	/// @{
	/**
	 * Performs a subtraction involving a polynomial.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs - rhs`
	 */
	friend LPPolynomial operator-(const LPPolynomial& lhs, const LPPolynomial& rhs);
	friend LPPolynomial operator-(const mpz_class& lhs, const LPPolynomial& rhs);
	friend LPPolynomial operator-(const LPPolynomial& lhs, const mpz_class& rhs);
	/// @}

	/// @name In-place multiplication operators
	/// @{
	/**
	 * Multiply this polynomial with something and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	LPPolynomial& operator*=(Variable rhs);
	LPPolynomial& operator*=(const mpz_class& rhs);
	LPPolynomial& operator*=(const LPPolynomial& rhs);
	/// @}

	/// @name Multiplication operators
	/// @{
	/**
	 * Perform a multiplication involving a polynomial.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs * rhs`
	 */
	friend LPPolynomial operator*(const LPPolynomial& lhs, const LPPolynomial& rhs);
	friend LPPolynomial operator*(const LPPolynomial& lhs, Variable rhs);
	friend LPPolynomial operator*(const mpz_class& lhs, const LPPolynomial& rhs);
	/// @}

	/**
	 * Streaming operator for univariate polynomials.
	 * @param os Output stream.
	 * @param rhs Polynomial.
	 * @return `os`
	 */
	friend std::ostream& operator<<(std::ostream& os, const LPPolynomial& rhs);

};

/**
 * Checks if the polynomial is equal to zero.
 * @return If polynomial is zero.
 */
bool is_zero(LPPolynomial& p) {
	return poly::is_zero(*p.mainPoly());
}

// bool isNegative(LPPolynomial<mpz_class>& p) {
// 	// return poly::is_zero(*p.mainPoly());
// 	return true;
// }

/**
 * Checks if the polynomial is linear or not.
 * @return If polynomial is linear.
 */
bool is_constant(LPPolynomial& p) {
	return poly::is_constant(*p.mainPoly());
}

/**
 * Checks if the polynomial is equal to one.
 * @return If polynomial is one.
 */
bool is_one(LPPolynomial& p) {
	if (!is_constant(p)) {
		return 0;
	}
	poly::Polynomial temp(lp_polynomial_get_context(p.mainPoly()->get_internal()));
	temp += poly::Integer(mpz_class(1));
	return temp == *p.mainPoly();
}

/**
 * Helper function to collect Variables in a polynomial (by lp_polynomial_traverse)
 */
static void collectVars(const lp_polynomial_context_t* ctx,
						lp_monomial_t* m,
						void* d) {
	carlVariables* varList = static_cast<carlVariables*>(d);
	for (size_t i = 0; i < m->n; i++) {
		carl::Variable var = VariableMapper::getInstance().getCarlVariable(m->p[i].x);
		varList->add(var);
	}
}

/// Add the variables of the given polynomial to the variables.
void variables(const LPPolynomial& p, carlVariables& vars) {
	vars.clear();
	lp_polynomial_traverse(p.mainPoly()->get_internal(), collectVars, &vars);
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
		return poly::hash(*p.mainPoly());
	}
};

/**
 * Specialization of `std::less` for univariate polynomials.
 */
template<>
struct less<carl::LPPolynomial> {
	carl::PolynomialComparisonOrder order;
	explicit less(carl::PolynomialComparisonOrder _order = carl::PolynomialComparisonOrder::Default) noexcept
		: order(_order) {}
	/**
	 * Compares two univariate polynomials.
	 * @param lhs First polynomial.
	 * @param rhs Second polynomial
	 * @return `lhs < rhs`.
	 */
	bool operator()(const carl::LPPolynomial& lhs, const carl::LPPolynomial& rhs) const {
		return lhs.less(rhs, order);
	}
	/**
	 * Compares two pointers to univariate polynomials.
	 * @param lhs First polynomial.
	 * @param rhs Second polynomial
	 * @return `lhs < rhs`.
	 */
	bool operator()(const carl::LPPolynomial* lhs, const carl::LPPolynomial* rhs) const {
		if (lhs == nullptr) return rhs != nullptr;
		if (rhs == nullptr) return true;
		return lhs->less(*rhs, order);
	}

};

} // namespace std

#include "LPPolynomial.tpp"

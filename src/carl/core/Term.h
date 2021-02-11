/*
 * @file Term.h
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

#include "Monomial.h"
#include "VariablesInformation.h"
#include "../numbers/numbers.h"

#include <memory>

namespace carl {

/**
 * Represents a single term, that is a numeric coefficient and a monomial.
 * @ingroup multirp
 */
template<typename Coefficient>
class Term {
private:
	using CoefficientType = Coefficient;
	CoefficientType mCoeff = constant_zero<CoefficientType>::get();
	Monomial::Arg mMonomial;

public:
	/**
		 * Default constructor. Constructs a term of value zero.
		 */
	Term() = default;
	/**
		 * Constructs a term of value \f$ c \f$.
		 * @param c Coefficient.
		 */
	explicit Term(const Coefficient& c);
	/**
		 * Constructs a term of value \f$ v \f$.
		 * @param v Variable.
		 */
	explicit Term(Variable v);
	/**
		 * Constructs a term of value \f$ m \f$.
		 * @param m Monomial pointer.
		 */
	explicit Term(Monomial::Arg m);
	/**
		 * Constructs a term of value \f$ m \f$.
		 * @param m Monomial pointer.
		 */
	explicit Term(Monomial::Arg&& m);

	/**
		 * Constructs a term of value \f$ c \cdot m \f$.
		 * @param c Coefficient.
		 * @param m Monomial pointer.
		 */
	Term(const Coefficient& c, Monomial::Arg m);

	/**
		 * Constructs a term of value \f$ c \cdot m \f$.
		 * @param c Coefficient.
		 * @param m Monomial pointer.
		 */
	Term(Coefficient&& c, Monomial::Arg&& m);
	/**
		 * Constructs a term of value \f$ c \cdot v^e \f$.
		 * @param c Coefficient.
		 * @param v Variable.
		 * @param e Exponent.
		 */
	Term(const Coefficient& c, Variable v, uint e);

	/**
		 * Get the coefficient.
		 * @return Coefficient.
		 */
	Coefficient& coeff() {
		return mCoeff;
	}
	const Coefficient& coeff() const {
		return mCoeff;
	}

	/**
		 * Get the monomial.
		 * @return Monomial.
		 */
	Monomial::Arg& monomial() {
		return mMonomial;
	}
	const Monomial::Arg& monomial() const {
		return mMonomial;
	}
	/**
	 * Gives the total degree, i.e. the sum of all exponents.
	 * @return Total degree.
	 */
	uint tdeg() const {
		if (!mMonomial) return 0;
		return mMonomial->tdeg();
	}

	/**
		 * Checks whether the term is zero.
		 * @return 
		 */
	[[deprecated("use carl::is_zero(t) instead.")]]
	bool isZero() const {
		return carl::isZero(mCoeff); //change this to mCoeff.isZero() at some point
	}

	/**
		 * Checks whether the term equals one.
         * @return 
         */
	[[deprecated("use carl::is_one(t) instead.")]]
	bool isOne() const {
		return isConstant() && carl::isOne(mCoeff); //change this to mCoeff.isOne() at some point
	}
	/**
		 * Checks whether the monomial is a constant.
		 * @return 
		 */
	bool isConstant() const {
		return !mMonomial;
	}

	/**
         * @return true, if the image of this term is integer-valued.
         */
	bool integerValued() const {
		if (!carl::isInteger(mCoeff)) return false;
		return !mMonomial || mMonomial->integerValued();
	}

	/**
		 * Checks whether the monomial has exactly the degree one.
		 * @return 
		 */
	bool isLinear() const {
		if (!mMonomial) return true;
		return mMonomial->isLinear();
	}
	/**
		 * 
		 * @return 
		 */
	std::size_t getNrVariables() const {
		if (!mMonomial) return 0;
		return mMonomial->nrVariables();
	}

	/**
		 * @param v The variable to check for its occurrence.
		 * @return true, if the variable occurs in this term.
		 */
	bool has(Variable v) const {
		if (!mMonomial) return false;
		return mMonomial->has(v);
	}

	/**
		 * Removes the given variable from the term.
		 */
	Term dropVariable(Variable v) const {
		return Term(coeff(), monomial()->dropVariable(v));
	}

	/**
		 * Checks if the monomial is either a constant or the only variable occuring is the variable v.
		 * @param v The variable which may occur.
		 * @return true if no variable occurs, or just v occurs. 
		 */
	bool hasNoOtherVariable(Variable v) const {
		if (!mMonomial) return true;
		return mMonomial->hasNoOtherVariable(v);
	}

	bool isSingleVariable() const {
		if (!mMonomial) return false;
		return mMonomial->isLinear();
	}
	/**
		 * For terms with exactly one variable, get this variable.
		 * @return The only variable occuring in the term.
		 */
	Variable getSingleVariable() const {
		assert(getNrVariables() == 1);
		return mMonomial->getSingleVariable();
	}

	/**
		 * Checks if the term is a square.
		 * @return If this is square.
		 */
	bool isSquare() const {
		return (mCoeff >= CoefficientType(0)) && ((!mMonomial) || mMonomial->isSquare());
	}

	/**
		 * Set the term to zero with the canonical representation.
		 */
	void clear() {
		mCoeff = carl::constant_zero<CoefficientType>().get();
		mMonomial = nullptr;
	}

	/**
		 * Negates the term by negating the coefficient.
		 */
	void negate() {
		mCoeff = -mCoeff;
	}

	/**
		 * 
		 * @param c a non-zero coefficient.
		 * @return 
		 */
	[[deprecated("Use carl::divide() instead.")]]
	Term divide(const Coefficient& c) const;
	[[deprecated("Use carl::divide() instead.")]]
	bool divide(const Coefficient& c, Term& res) const;
	[[deprecated("Use carl::divide() instead.")]]
	bool divide(Variable v, Term& res) const;

	bool divide(const Monomial::Arg& m, Term& res) const;

	bool divide(const Term& t, Term& res) const;

	Term calcLcmAndDivideBy(const Monomial::Arg& m) const;

	/**
		 * Calculates the square root of this term.
		 * Returns true, iff the term is a square as checked by isSquare().
		 * In that case, res will changed to be the square root.
		 * Otherwise, res is undefined.
		 * @param res Square root of this term.
		 * @return If square root could be calculated.
		 */
	bool sqrt(Term& res) const;

	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	bool divisible(const Term& t) const;
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy>
	bool divisible(const Term& t) const;

	template<bool gatherCoeff, typename CoeffType>
	void gatherVarInfo(Variable var, VariableInformation<gatherCoeff, CoeffType>& varinfo) const;

	template<bool gatherCoeff, typename CoeffType>
	void gatherVarInfo(VariablesInformation<gatherCoeff, CoeffType>& varinfo) const;

	bool isConsistent() const;

	/// @name Division operators
	/// @{
	/**
		 * Perform a division involving a term.
		 * @param lhs Left hand side.
		 * @param rhs Right hand side.
		 * @return `lhs / rhs`
		 */
	template<typename Coeff>
	friend const Term<Coeff> operator/(const Term<Coeff>& lhs, uint rhs);
	/// @}

	/**
		 * Streaming operator for Term.
		 * @param os Output stream.
		 * @param rhs Term.
		 * @return `os`
		 */
	template<typename Coeff>
	friend std::ostream& operator<<(std::ostream& os, const Term<Coeff>& rhs);

	/**
		 * Checks if two terms have the same monomial.
         * @param lhs First term.
         * @param rhs Second term.
         * @return If both terms have the same monomial.
         */
	static bool monomialEqual(const Term& lhs, const Term& rhs) {
		return lhs.mMonomial == rhs.mMonomial;
	}
	static bool monomialEqual(const std::shared_ptr<const Term>& lhs, const std::shared_ptr<const Term>& rhs) {
		if (lhs == rhs) return true;
		if (lhs && rhs) return monomialEqual(*lhs, *rhs);
		return false;
	}
	static bool monomialLess(const Term& lhs, const Term& rhs) {
		return lhs.mMonomial < rhs.mMonomial;
	}
	static bool monomialLess(const std::shared_ptr<const Term>& lhs, const std::shared_ptr<const Term>& rhs) {
		if (lhs && rhs) return monomialLess(*lhs, *rhs);
		return !lhs;
	}
};

/**
 * Checks whether a term is zero.
 */
template<typename Coeff>
inline bool isZero(const Term<Coeff>& term) {
	return carl::isZero(term.coeff());
}

/// Add the variables of the given term to the variables.
template<typename Coeff>
void variables(const Term<Coeff>& t, carlVariables& vars) {
	if (t.monomial()) variables(*t.monomial(), vars);
}

/**
 * Checks whether a term is one.
 */
template<typename Coeff>
inline bool isOne(const Term<Coeff>& term) {
	return term.isConstant() && carl::isOne(term.coeff());
}

template<typename Coeff>
Term<Coeff> operator-(const Term<Coeff>& rhs) {
	return Term<Coeff>(-rhs.coeff(), rhs.monomial());
}

/// @name In-place multiplication operators
/// @{
/**
 * Multiply a term with something and return the changed term.
 * @param lhs Left hand side.
 * @param rhs Right hand side.
 * @return Changed `lhs`.
 */
template<typename Coeff>
Term<Coeff>& operator*=(Term<Coeff>& lhs, const Coeff& rhs) {
	if (carl::isZero(rhs)) {
		lhs.clear();
	} else {
		lhs.coeff() *= rhs;
	}
	return lhs;
}
template<typename Coeff>
Term<Coeff>& operator*=(Term<Coeff>& lhs, Variable rhs) {
	if (carl::isZero(lhs.coeff())) {
		return lhs;
	}
	if (lhs.monomial()) {
		lhs.monomial() = lhs.monomial() * rhs;
	} else {
		lhs.monomial() = createMonomial(rhs, 1);
	}
	return lhs;
}
template<typename Coeff>
Term<Coeff>& operator*=(Term<Coeff>& lhs, const Monomial::Arg& rhs) {
	if (carl::isZero(lhs.coeff())) {
		return lhs;
	}
	if (lhs.monomial()) {
		lhs.monomial() = lhs.monomial() * rhs;
	} else {
		lhs.monomial() = rhs;
	}
	return lhs;
}
template<typename Coeff>
Term<Coeff>& operator*=(Term<Coeff>& lhs, const Term<Coeff>& rhs) {
	if (carl::isZero(lhs.coeff())) {
		return lhs;
	}
	if (carl::isZero(rhs.coeff())) {
		lhs.clear();
		return lhs;
	}
	lhs.monomial() = lhs.monomial() * rhs.monomial();
	lhs.coeff() *= rhs.coeff();
	return lhs;
}
/// @}

/// @name Comparison operators
/// @{
/**
	 * Compares two arguments where one is a term and the other is either a term, a monomial or a variable.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs ~ rhs`, `~` being the relation that is checked.
	 */
template<typename Coeff>
inline bool operator==(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
template<typename Coeff>
inline bool operator==(const Term<Coeff>& lhs, const Monomial& rhs);
template<typename Coeff>
inline bool operator==(const Term<Coeff>& lhs, Variable rhs);
template<typename Coeff>
inline bool operator==(const Term<Coeff>& lhs, const Coeff& rhs);
template<typename Coeff>
inline bool operator==(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
	return rhs == lhs;
}
template<typename Coeff>
inline bool operator==(Variable lhs, const Term<Coeff>& rhs) {
	return rhs == lhs;
}
template<typename Coeff>
inline bool operator==(const Coeff& lhs, const Term<Coeff>& rhs) {
	return rhs == lhs;
}

template<typename Coeff>
inline bool operator!=(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
	return !(lhs == rhs);
}
template<typename Coeff>
inline bool operator!=(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
	return !(lhs == rhs);
}
template<typename Coeff>
inline bool operator!=(const Term<Coeff>& lhs, Variable rhs) {
	return !(lhs == rhs);
}
template<typename Coeff>
inline bool operator!=(const Term<Coeff>& lhs, const Coeff& rhs) {
	return !(lhs == rhs);
}
template<typename Coeff>
inline bool operator!=(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
	return !(lhs == rhs);
}
template<typename Coeff>
inline bool operator!=(Variable lhs, const Term<Coeff>& rhs) {
	return !(lhs == rhs);
}
template<typename Coeff>
inline bool operator!=(const Coeff& lhs, const Term<Coeff>& rhs) {
	return !(lhs == rhs);
}

template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, const Monomial::Arg& rhs);
template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, Variable rhs);
template<typename Coeff>
bool operator<(const Term<Coeff>& lhs, const Coeff& rhs);
template<typename Coeff>
bool operator<(const Monomial::Arg& lhs, const Term<Coeff>& rhs);
template<typename Coeff>
bool operator<(Variable lhs, const Term<Coeff>& rhs);
template<typename Coeff>
bool operator<(const Coeff& lhs, const Term<Coeff>& rhs);

template<typename Coeff>
inline bool operator<=(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
	return !(rhs < lhs);
}
template<typename Coeff>
inline bool operator<=(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
	return !(rhs < lhs);
}
template<typename Coeff>
inline bool operator<=(const Term<Coeff>& lhs, Variable rhs) {
	return !(rhs < lhs);
}
template<typename Coeff>
inline bool operator<=(const Term<Coeff>& lhs, const Coeff& rhs) {
	return !(rhs < lhs);
}
template<typename Coeff>
inline bool operator<=(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
	return !(rhs < lhs);
}
template<typename Coeff>
inline bool operator<=(Variable lhs, const Term<Coeff>& rhs) {
	return !(rhs < lhs);
}
template<typename Coeff>
inline bool operator<=(const Coeff& lhs, const Term<Coeff>& rhs) {
	return !(rhs < lhs);
}

template<typename Coeff>
inline bool operator>(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
	return rhs < lhs;
}
template<typename Coeff>
inline bool operator>(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
	return rhs < lhs;
}
template<typename Coeff>
inline bool operator>(const Term<Coeff>& lhs, Variable rhs) {
	return rhs < lhs;
}
template<typename Coeff>
inline bool operator>(const Term<Coeff>& lhs, const Coeff& rhs) {
	return rhs < lhs;
}
template<typename Coeff>
inline bool operator>(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
	return rhs < lhs;
}
template<typename Coeff>
inline bool operator>(Variable lhs, const Term<Coeff>& rhs) {
	return rhs < lhs;
}
template<typename Coeff>
inline bool operator>(const Coeff& lhs, const Term<Coeff>& rhs) {
	return rhs < lhs;
}

template<typename Coeff>
inline bool operator>=(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
	return rhs <= lhs;
}
template<typename Coeff>
inline bool operator>=(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
	return rhs <= lhs;
}
template<typename Coeff>
inline bool operator>=(const Term<Coeff>& lhs, Variable rhs) {
	return rhs <= lhs;
}
template<typename Coeff>
inline bool operator>=(const Term<Coeff>& lhs, const Coeff& rhs) {
	return rhs <= lhs;
}
template<typename Coeff>
inline bool operator>=(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
	return rhs <= lhs;
}
template<typename Coeff>
inline bool operator>=(Variable lhs, const Term<Coeff>& rhs) {
	return rhs <= lhs;
}
template<typename Coeff>
inline bool operator>=(const Coeff& lhs, const Term<Coeff>& rhs) {
	return rhs <= lhs;
}
/// @}

/// @name Multiplication operators
/// @{
/**
	 * Perform a multiplication involving a term.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs * rhs`
	 */
template<typename Coeff>
inline Term<Coeff> operator*(Term<Coeff> lhs, const Term<Coeff>& rhs) {
	return lhs *= rhs;
}
template<typename Coeff>
inline Term<Coeff> operator*(Term<Coeff> lhs, const Monomial::Arg& rhs) {
	return lhs *= rhs;
}
template<typename Coeff>
inline Term<Coeff> operator*(Term<Coeff> lhs, Variable rhs) {
	return lhs *= rhs;
}
template<typename Coeff>
inline Term<Coeff> operator*(Term<Coeff> lhs, const Coeff& rhs) {
	return lhs *= rhs;
}
template<typename Coeff>
inline Term<Coeff> operator*(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
	return rhs * lhs;
}
template<typename Coeff, EnableIf<carl::is_number<Coeff>> = dummy>
inline Term<Coeff> operator*(const Monomial::Arg& lhs, const Coeff& rhs) {
	return Term<Coeff>(rhs, lhs);
}
template<typename Coeff>
inline Term<Coeff> operator*(Variable lhs, const Term<Coeff>& rhs) {
	return rhs * lhs;
}
template<typename Coeff>
inline Term<Coeff> operator*(Variable lhs, const Coeff& rhs) {
	return Term<Coeff>(rhs, lhs, 1);
}
template<typename Coeff>
inline Term<Coeff> operator*(const Coeff& lhs, const Term<Coeff>& rhs) {
	return rhs * lhs;
}
template<typename Coeff, EnableIf<carl::is_number<Coeff>> = dummy>
inline Term<Coeff> operator*(const Coeff& lhs, const Monomial::Arg& rhs) {
	return rhs * lhs;
}
template<typename Coeff>
inline Term<Coeff> operator*(const Coeff& lhs, Variable rhs) {
	return rhs * lhs;
}
template<typename Coeff, EnableIf<carl::is_subset_of_rationals<Coeff>> = dummy>
inline Term<Coeff> operator/(const Term<Coeff>& lhs, const Coeff& rhs) {
	return lhs * reciprocal(rhs);
}
template<typename Coeff, EnableIf<carl::is_subset_of_rationals<Coeff>> = dummy>
inline Term<Coeff> operator/(const Monomial::Arg& lhs, const Coeff& rhs) {
	return lhs * reciprocal(rhs);
}
template<typename Coeff, EnableIf<carl::is_subset_of_rationals<Coeff>> = dummy>
inline Term<Coeff> operator/(Variable& lhs, const Coeff& rhs) {
	return lhs * reciprocal(rhs);
}
/// @}

} // namespace carl

namespace std {

/**
 * Specialization of `std::hash` for a Term.
 */
template<typename Coefficient>
struct hash<carl::Term<Coefficient>> {
	/**
	 * Calculates the hash of a Term.
	 * @param term Term.
	 * @return Hash of term.
	 */
	std::size_t operator()(const carl::Term<Coefficient>& term) const {
		if (term.isConstant()) {
			return carl::hash_all(term.coeff());
		} else {
			return carl::hash_all(term.coeff(), *term.monomial());
		}
	}
};
} // namespace std

#include "Term.tpp"

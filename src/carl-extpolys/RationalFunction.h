/** 
 * @file:   RationalFunction.h
 * @author: Sebastian Junges
 * @author: Florian Corzilius
 */

#pragma once
#include <carl/numbers/numbers.h>
#include <carl/util/hash.h>
#include "FactorizedPolynomial.h"
#include <carl/core/polynomialfunctions/Evaluation.h>
#include <carl/core/polynomialfunctions/GCD.h>

#include "evaluation.h"

#include <boost/optional.hpp>

namespace carl {

template<typename Pol, bool AutoSimplify = false>
class RationalFunction {
public:
	using PolyType = Pol;
	using CoeffType = typename Pol::CoeffType;
	using NumberType = typename Pol::NumberType;

private:
	boost::optional<std::pair<Pol, Pol>> mPolynomialQuotient;
	CoeffType mNumberQuotient;
	bool mIsSimplified;

public:
	RationalFunction()
		: mNumberQuotient(0),
		  mIsSimplified(true) {}

	explicit RationalFunction(int v)
		: mNumberQuotient(v),
		  mIsSimplified(true) {}

	explicit RationalFunction(const CoeffType& c)
		: mNumberQuotient(c),
		  mIsSimplified(true) {}

	template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
	explicit RationalFunction(Variable v)
		: mPolynomialQuotient(std::pair<Pol, Pol>(std::move(P(v)), std::move(Pol(1)))),
		  mNumberQuotient(),
		  mIsSimplified(true) {}

	explicit RationalFunction(const Pol& p)
		: mNumberQuotient(),
		  mIsSimplified(true) {
		if (p.isConstant())
			mNumberQuotient = p.constantPart();
		else
			mPolynomialQuotient = std::pair<Pol, Pol>(p, std::move(Pol(1)));
	}

	explicit RationalFunction(Pol&& p)
		: mNumberQuotient(),
		  mIsSimplified(true) {
		if (p.isConstant())
			mNumberQuotient = p.constantPart();
		else
			mPolynomialQuotient = std::pair<Pol, Pol>(std::move(p), std::move(Pol(1)));
	}

	explicit RationalFunction(const Pol& nom, const Pol& denom)
		: mNumberQuotient(),
		  mIsSimplified(false) {
		if (nom.isConstant() && denom.isConstant()) {
			mNumberQuotient = nom.constantPart() / denom.constantPart();
			mIsSimplified = true;
		} else {
			mPolynomialQuotient = std::pair<Pol, Pol>(nom, denom);
			eliminateCommonFactor(!AutoSimplify);
			assert(isConstant() || !carl::isZero(denominatorAsPolynomial()));
		}
	}

	explicit RationalFunction(Pol&& nom, Pol&& denom)
		: mPolynomialQuotient(std::pair<Pol, Pol>(std::move(nom), std::move(denom))),
		  mNumberQuotient(),
		  mIsSimplified(false) {
		eliminateCommonFactor(!AutoSimplify);
		assert(isConstant() || !carl::isZero(denominatorAsPolynomial()));
	}

	explicit RationalFunction(boost::optional<std::pair<Pol, Pol>>&& quotient, const CoeffType& num, bool simplified)
		: mPolynomialQuotient(std::move(quotient)),
		  mNumberQuotient(num),
		  mIsSimplified(simplified) {}

	RationalFunction(const RationalFunction& _rf) = default;

	RationalFunction(RationalFunction&& _rf) = default;

	~RationalFunction() noexcept = default;

	RationalFunction& operator=(const RationalFunction& _rf) = default;

	RationalFunction& operator=(RationalFunction&& _rf) = default;

	/**
	 * @return The nominator
	 */
	Pol nominator() const {
		if (isConstant())
			return Pol(carl::getNum(mNumberQuotient));
		else
			return mPolynomialQuotient->first;
	}

	/**
	 * @return  The denominator
	 */
	Pol denominator() const {
		if (isConstant())
			return Pol(carl::getDenom(mNumberQuotient));
		return mPolynomialQuotient->second;
	}

	/**
	 * @return The nominator as a polynomial.
	 */
	const Pol& nominatorAsPolynomial() const {
		assert(!isConstant());
		return mPolynomialQuotient->first;
	}

	/**
	 * @return  The denominator as a polynomial.
	 */
	const Pol& denominatorAsPolynomial() const {
		assert(!isConstant());
		return mPolynomialQuotient->second;
	}

	/**
	 * @return The nominator as a polynomial.
	 */
	CoeffType nominatorAsNumber() const {
		assert(isConstant());
		return carl::getNum(mNumberQuotient);
	}

	/**
	 * @return  The denominator as a polynomial.
	 */
	CoeffType denominatorAsNumber() const {
		assert(isConstant());
		return carl::getDenom(mNumberQuotient);
	}

	/**
	 * Checks if this rational function has been simplified since it's last modification.
	 * Note that if AutoSimplify is true, this should always return true.
	 * @return If this is simplified.
	 */
	bool isSimplified() const {
		return mIsSimplified;
	}

	void simplify() {
		if (AutoSimplify) {
			CARL_LOG_WARN("carl.core", "Calling simplify on rational function with AutoSimplify");
		}
		eliminateCommonFactor(false);
	}

	/**
	 * Returns the inverse of this rational function.
	 * @return Inverse of this.
	 */
	RationalFunction inverse() const {
		assert(!this->isZero());
		if (isConstant()) {
			return RationalFunction(boost::none, 1 / mNumberQuotient, mIsSimplified);
		} else {
			return RationalFunction(boost::optional<std::pair<Pol, Pol>>(std::pair<Pol, Pol>(mPolynomialQuotient->second, mPolynomialQuotient->first)), carl::constant_zero<CoeffType>().get(), mIsSimplified);
		}
	}

	/**
	 * Check whether the rational function is zero
	 * @return true if it is
	 */
	bool isZero() const {
		if (isConstant())
			return carl::isZero(mNumberQuotient);
		assert(!carl::isZero(denominatorAsPolynomial()));
		return carl::isZero(nominatorAsPolynomial());
	}

	bool isOne() const {
		if (isConstant())
			return carl::isOne(mNumberQuotient);
		assert(!carl::isZero(denominatorAsPolynomial()));
		return nominatorAsPolynomial() == denominatorAsPolynomial();
	}

	bool isConstant() const {
		return !mPolynomialQuotient;
	}

	CoeffType constantPart() const {
		if (isConstant())
			return mNumberQuotient;
		return nominatorAsPolynomial().constantPart() / denominatorAsPolynomial().constantPart();
	}

	/**
	 * Collect all occurring variables
	 * @return All occcurring variables
	 */
	std::set<Variable> gatherVariables() const {
		std::set<Variable> vars;
		gatherVariables(vars);
		return vars;
	}

	/**
	 * Add all occurring variables to the set vars
	 * @param vars
	 */
	void gatherVariables(std::set<Variable>& vars) const {
		if (isConstant())
			return;
		nominatorAsPolynomial().gatherVariables(vars);
		denominatorAsPolynomial().gatherVariables(vars);
	}

	/**
	 * Evaluate the polynomial at the point described by substitutions.
	 * @param substitutions A mapping from variable to constant values.
	 * @return The result of the substitution
	 */
	CoeffType evaluate(const std::map<Variable, CoeffType>& substitutions) const {
		if (isConstant()) {
			return mNumberQuotient;
		} else {
			return carl::evaluate(nominatorAsPolynomial(), substitutions) / carl::evaluate(denominatorAsPolynomial(), substitutions);
		}
	}

	RationalFunction substitute(const std::map<Variable, CoeffType>& substitutions) const {
		if (isConstant())
			return *this;
		else {
			return RationalFunction(carl::substitute(nominatorAsPolynomial(), substitutions), carl::substitute(denominatorAsPolynomial(), substitutions));
		}
	}

	/**
	 * Derivative of the rational function with respect to variable x
	 * @param x the main variable
	 * @param nth which derivative one should take
	 * @return 
	 * 
	 * @todo Currently only nth = 1 is supported
	 * @todo Curretnly only factorized polynomials are supported
	 */
	RationalFunction derivative(const Variable& x, unsigned nth = 1) const;

	std::string toString(bool infix = true, bool friendlyNames = true) const;

private:
	/**
	 * Helper function for simplify which eliminates the common factor.
	 * @param _justNormalize
	 */
	void eliminateCommonFactor(bool _justNormalize);

	template<bool byInverse = false>
	RationalFunction& add(const RationalFunction& rhs);

	template<bool byInverse = false>
	RationalFunction& add(const Pol& rhs);

	template<bool byInverse = false, typename P = Pol, DisableIf<needs_cache<P>> = dummy>
	RationalFunction& add(Variable rhs);

	template<bool byInverse = false>
	RationalFunction& add(const CoeffType& rhs);

public:
	/// @name In-place addition operators
	/// @{
	/**
	 * Add something to this rational function and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator+=(const RationalFunction& rhs) {
		return this->template add<false>(rhs);
	}

	RationalFunction& operator+=(const Pol& rhs) {
		return this->template add<false>(rhs);
	}

	RationalFunction& operator+=(const Term<CoeffType>& rhs) {
		auto tmp = Pol(rhs);
		return this->template add<false>(tmp);
	}

	RationalFunction& operator+=(const Monomial::Arg& rhs) {
		auto tmp = Pol(rhs);
		return this->template add<false>(tmp);
	}

	template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
	RationalFunction& operator+=(Variable rhs) {
		return this->template add<false>(rhs);
	}

	RationalFunction& operator+=(const CoeffType& rhs) {
		return this->template add<false>(rhs);
	}
	/// @}

	/// @name In-place subtraction operators
	/// @{
	/**
	 * Subtract something from this rational function and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator-=(const RationalFunction& rhs) {
		return this->template add<true>(rhs);
	}

	RationalFunction& operator-=(const Pol& rhs) {
		return this->template add<true>(rhs);
	}

	RationalFunction& operator-=(const Term<CoeffType>& rhs) {
		return (*this -= Pol(rhs));
	}

	RationalFunction& operator-=(const Monomial::Arg& rhs) {
		return (*this -= Pol(rhs));
	}

	template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
	RationalFunction& operator-=(Variable rhs) {
		return this->template add<true>(rhs);
	}

	RationalFunction& operator-=(const CoeffType& rhs) {
		return this->template add<true>(rhs);
	}
	/// @}

	/// @name In-place multiplication operators
	/// @{
	/**
	 * Multiply something with this rational function and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator*=(const RationalFunction& rhs);
	RationalFunction& operator*=(const Pol& rhs);
	RationalFunction& operator*=(const Term<CoeffType>& rhs) {
		return (*this *= Pol(rhs));
	}
	RationalFunction& operator*=(const Monomial::Arg& rhs) {
		return (*this *= Pol(rhs));
	}
	template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
	RationalFunction& operator*=(Variable rhs);
	RationalFunction& operator*=(const CoeffType& rhs);
	RationalFunction& operator*=(carl::sint rhs);
	/// @}

	/**
	 * Divide this rational function by something and return the changed rational function.
	 * @param rhs Right hand side.
	 * @return Changed rational function.
	 */
	RationalFunction& operator/=(const RationalFunction& rhs);
	RationalFunction& operator/=(const Pol& rhs);
	RationalFunction& operator/=(const Term<CoeffType>& rhs) {
		return (*this /= Pol(rhs));
	}
	RationalFunction& operator/=(const Monomial::Arg& rhs) {
		return (*this /= Pol(rhs));
	}
	template<typename P = Pol, DisableIf<needs_cache<P>> = dummy>
	RationalFunction& operator/=(Variable rhs);
	RationalFunction& operator/=(const CoeffType& rhs);
	RationalFunction& operator/=(unsigned long rhs);
	/// @}

	template<typename PolA, bool ASA>
	friend bool operator==(const RationalFunction<PolA, ASA>& lhs, const RationalFunction<PolA, ASA>& rhs);

	template<typename PolA, bool ASA>
	friend bool operator<(const RationalFunction<PolA, ASA>& lhs, const RationalFunction<PolA, ASA>& rhs);

	template<typename PolA, bool ASA>
	friend std::ostream& operator<<(std::ostream& os, const RationalFunction<PolA, ASA>& rhs);
};

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs) {
	return RationalFunction<Pol, AS>(lhs) += rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Pol& rhs) {
	return RationalFunction<Pol, AS>(lhs) += rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs) {
	return RationalFunction<Pol, AS>(lhs) += rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs) {
	return RationalFunction<Pol, AS>(lhs) += rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, Variable rhs) {
	return RationalFunction<Pol, AS>(lhs) += rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator+(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs) {
	return RationalFunction<Pol, AS>(lhs) += rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs) {
	return RationalFunction<Pol, AS>(lhs) *= typename Pol::CoeffType(-1);
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs) {
	return RationalFunction<Pol, AS>(lhs) -= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Pol& rhs) {
	return RationalFunction<Pol, AS>(lhs) -= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs) {
	return RationalFunction<Pol, AS>(lhs) -= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs) {
	return RationalFunction<Pol, AS>(lhs) -= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, Variable rhs) {
	return RationalFunction<Pol, AS>(lhs) -= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator-(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs) {
	return RationalFunction<Pol, AS>(lhs) -= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs) {
	return RationalFunction<Pol, AS>(lhs) *= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Pol& rhs) {
	return RationalFunction<Pol, AS>(lhs) *= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs) {
	return RationalFunction<Pol, AS>(lhs) *= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs) {
	return RationalFunction<Pol, AS>(lhs) *= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, Variable rhs) {
	return RationalFunction<Pol, AS>(lhs) *= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs) {
	return RationalFunction<Pol, AS>(lhs) *= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator*(const typename Pol::CoeffType& lhs, const RationalFunction<Pol, AS>& rhs) {
	return RationalFunction<Pol, AS>(rhs) *= lhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator*(const RationalFunction<Pol, AS>& lhs, carl::sint rhs) {
	return RationalFunction<Pol, AS>(lhs) *= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator*(carl::sint lhs, const RationalFunction<Pol, AS>& rhs) {
	return RationalFunction<Pol, AS>(rhs) *= lhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs) {
	return RationalFunction<Pol, AS>(lhs) /= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Pol& rhs) {
	return RationalFunction<Pol, AS>(lhs) /= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Term<typename Pol::CoeffType>& rhs) {
	return RationalFunction<Pol, AS>(lhs) /= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const Monomial::Arg& rhs) {
	return RationalFunction<Pol, AS>(lhs) /= rhs;
}

template<typename Pol, bool AS, DisableIf<needs_cache<Pol>> = dummy>
RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, Variable rhs) {
	return RationalFunction<Pol, AS>(lhs) /= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, const typename Pol::CoeffType& rhs) {
	return RationalFunction<Pol, AS>(lhs) /= rhs;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS> operator/(const RationalFunction<Pol, AS>& lhs, unsigned long rhs) {
	return RationalFunction<Pol, AS>(lhs) /= rhs;
}

// TODO: Make more efficient :).
template<typename Pol, bool AS>
RationalFunction<Pol, AS> pow(unsigned exp, const RationalFunction<Pol, AS>& rf) {
	static_assert(!std::is_same<Pol, Pol>::value, "Use carl::pow(rf, exp) instead. ");
	RationalFunction<Pol, AS> res = carl::constant_one<RationalFunction<Pol, AS>>().get();
	for (unsigned i = exp; i > 0; --i) {
		res *= rf;
	}
	return res;
}

template<typename Pol, bool AS>
bool operator!=(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs) {
	return !(lhs == rhs);
}

} // namespace carl

namespace std {
template<typename Pol, bool AS>
struct hash<carl::RationalFunction<Pol, AS>> {
	std::size_t operator()(const carl::RationalFunction<Pol, AS>& r) const {
		if (r.isConstant())
			return carl::hash_all(r.nominatorAsNumber(), r.denominatorAsNumber());
		else
			return carl::hash_all(r.nominatorAsPolynomial(), r.denominatorAsPolynomial());
	}
};
} // namespace std

#include "RationalFunction.tpp"

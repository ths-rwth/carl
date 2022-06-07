#pragma once
/** 
 * @file:   RationalFunction.tpp
 * @author: Sebastian Junges
 * @author: Florian Corzilius
 *
 * @since March 16, 2014
 */

#include "RationalFunction.h"

#include <carl-arith/poly/umvpoly/functions/LCM.h>

namespace carl {

template<typename Pol, bool AS>
RationalFunction<Pol, AS> RationalFunction<Pol, AS>::derivative(const Variable& x, unsigned nth) const {
	assert(nth == 1);
	if (is_constant()) {
		return RationalFunction<Pol, AS>(0);
	}

	// TODO use factorization whenever possible.
	// TODO specialize if it is just a polynomial.
	CARL_LOG_INEFFICIENT();
	// (u/v)' = (u'v - uv')/(v^2)
	const auto& u = nominatorAsPolynomial();
	const auto& v = denominatorAsPolynomial();
	return RationalFunction<Pol, AS>(u.derivative(x) * v - u * v.derivative(x), v.pow(2));
}

template<typename Pol, bool AS>
void RationalFunction<Pol, AS>::eliminateCommonFactor(bool _justNormalize) {
	if (mIsSimplified) return;
	assert(!is_constant());
	if (carl::is_zero(nominatorAsPolynomial())) {
		mPolynomialQuotient.reset();
		mNumberQuotient = std::move(CoeffType(0));
		mIsSimplified = true;
		return;
	}
	if (nominatorAsPolynomial() == denominatorAsPolynomial()) {
		mPolynomialQuotient.reset();
		mNumberQuotient = std::move(CoeffType(1));
		mIsSimplified = true;
		return;
	}
	CoeffType cpFactorNom(std::move(nominatorAsPolynomial().coprime_factor()));
	CoeffType cpFactorDen(std::move(denominatorAsPolynomial().coprime_factor()));
	mPolynomialQuotient->first *= cpFactorNom;
	mPolynomialQuotient->second *= cpFactorDen;
	CoeffType cpFactor(std::move(cpFactorDen / cpFactorNom));
	if (!_justNormalize && !denominatorAsPolynomial().is_constant()) {
		carl::gcd(nominatorAsPolynomial(), denominatorAsPolynomial());
		auto ret = carl::lazyDiv(nominatorAsPolynomial(), denominatorAsPolynomial());
		mPolynomialQuotient->first = std::move(ret.first);
		mPolynomialQuotient->second = std::move(ret.second);
		CoeffType cpFactorNom(nominatorAsPolynomial().coprime_factor());
		CoeffType cpFactorDen(denominatorAsPolynomial().coprime_factor());
		mPolynomialQuotient->first *= cpFactorNom;
		mPolynomialQuotient->second *= cpFactorDen;
		cpFactor *= cpFactorDen / cpFactorNom;
		mIsSimplified = true;
	}
	mPolynomialQuotient->first *= carl::get_num(cpFactor);
	mPolynomialQuotient->second *= carl::get_denom(cpFactor);
	if (nominatorAsPolynomial().is_constant() && denominatorAsPolynomial().is_constant()) {
		mNumberQuotient = std::move(constant_part());
		mPolynomialQuotient.reset();
		mIsSimplified = true;
	}
}

template<typename Pol, bool AS>
template<bool byInverse>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(const RationalFunction<Pol, AS>& rhs) {
	if (this->is_constant() && rhs.is_constant()) {
		if (byInverse)
			this->mNumberQuotient -= rhs.mNumberQuotient;
		else
			this->mNumberQuotient += rhs.mNumberQuotient;
		return *this;
	} else if (this->is_constant()) {
		CoeffType c = this->mNumberQuotient;
		if (byInverse)
			*this = -rhs;
		else
			*this = rhs;
		return *this += c;
	} else if (rhs.is_constant()) {
		if (byInverse)
			return *this -= rhs.mNumberQuotient;
		else
			return *this += rhs.mNumberQuotient;
	}
	mIsSimplified = false;
	if (denominatorAsPolynomial().is_constant() && rhs.denominatorAsPolynomial().is_constant()) {
		mPolynomialQuotient->first *= rhs.denominatorAsPolynomial().constant_part();
		if (byInverse)
			mPolynomialQuotient->first -= rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constant_part();
		else
			mPolynomialQuotient->first += rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constant_part();
		mPolynomialQuotient->second *= rhs.denominatorAsPolynomial().constant_part();
	} else {
		if (denominatorAsPolynomial().is_constant()) {
			// TODO use more efficient elimination
			mPolynomialQuotient->first *= rhs.denominatorAsPolynomial();
			if (byInverse)
				mPolynomialQuotient->first -= rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constant_part();
			else
				mPolynomialQuotient->first += rhs.nominatorAsPolynomial() * denominatorAsPolynomial().constant_part();
			// TODO use info that it is faster
			mPolynomialQuotient->second *= rhs.denominatorAsPolynomial();
		} else if (rhs.denominatorAsPolynomial().is_constant()) {
			mPolynomialQuotient->first *= rhs.denominatorAsPolynomial().constant_part();
			if (byInverse)
				mPolynomialQuotient->first -= rhs.nominatorAsPolynomial() * denominatorAsPolynomial();
			else
				mPolynomialQuotient->first += rhs.nominatorAsPolynomial() * denominatorAsPolynomial();
			mPolynomialQuotient->second *= rhs.denominatorAsPolynomial().constant_part();
		} else {
			Pol leastCommonMultiple(std::move(carl::lcm(this->denominatorAsPolynomial(), rhs.denominatorAsPolynomial())));
			if (byInverse) {
				mPolynomialQuotient->first = std::move(this->nominatorAsPolynomial() * quotient(leastCommonMultiple, this->denominatorAsPolynomial()) - rhs.nominatorAsPolynomial() * quotient(leastCommonMultiple, rhs.denominatorAsPolynomial()));
			} else {
				mPolynomialQuotient->first = std::move(this->nominatorAsPolynomial() * quotient(leastCommonMultiple, this->denominatorAsPolynomial()) + rhs.nominatorAsPolynomial() * quotient(leastCommonMultiple, rhs.denominatorAsPolynomial()));
			}
			mPolynomialQuotient->second = std::move(leastCommonMultiple);
		}
	}
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
template<bool byInverse>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(const Pol& rhs) {
	if (this->is_constant()) {
		CoeffType c = this->mNumberQuotient;
		Pol resultNum(std::move(byInverse ? (rhs * CoeffType(get_denom(c)) - CoeffType(get_num(c))) : (rhs * CoeffType(get_denom(c)) + CoeffType(get_num(c)))));
		*this = std::move(RationalFunction<Pol, AS>(std::move(resultNum), std::move(Pol(CoeffType(get_denom(c))))));
		return *this;
	}
	mIsSimplified = false;
	if (byInverse)
		mPolynomialQuotient->first -= std::move(rhs * denominatorAsPolynomial());
	else
		mPolynomialQuotient->first += std::move(rhs * denominatorAsPolynomial());
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
template<bool byInverse, typename P, DisableIf<needs_cache_type<P>>>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(Variable rhs) {
	if (this->is_constant()) {
		CoeffType c(this->mNumberQuotient);
		Pol resultNum(rhs);
		resultNum *= CoeffType(get_denom(c));
		if (byInverse)
			resultNum -= CoeffType(get_num(c));
		else
			resultNum += CoeffType(get_num(c));
		*this = std::move(RationalFunction<Pol, AS>(std::move(resultNum), std::move(Pol(CoeffType(get_denom(c))))));
		return *this;
	}
	mIsSimplified = false;
	if (byInverse)
		mPolynomialQuotient->first -= std::move(rhs * denominatorAsPolynomial());
	else
		mPolynomialQuotient->first += std::move(rhs * denominatorAsPolynomial());
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
template<bool byInverse>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::add(const typename Pol::CoeffType& rhs) {
	if (this->is_constant()) {
		if (byInverse)
			this->mNumberQuotient -= rhs;
		else
			this->mNumberQuotient += rhs;
		return *this;
	}
	mIsSimplified = false;
	if (byInverse)
		mPolynomialQuotient->first -= std::move(rhs * denominatorAsPolynomial());
	else
		mPolynomialQuotient->first += std::move(rhs * denominatorAsPolynomial());
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const RationalFunction<Pol, AS>& rhs) {
	if (this->is_constant() && rhs.is_constant()) {
		this->mNumberQuotient *= rhs.mNumberQuotient;
		return *this;
	} else if (this->is_constant()) {
		CoeffType c(this->mNumberQuotient);
		*this = rhs;
		return *this *= c;
	} else if (rhs.is_constant()) {
		return *this *= rhs.mNumberQuotient;
	}
	mIsSimplified = false;
	mPolynomialQuotient->first *= rhs.nominatorAsPolynomial();
	mPolynomialQuotient->second *= rhs.denominatorAsPolynomial();
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const Pol& rhs) {
	if (this->is_constant()) {
		CoeffType c = this->mNumberQuotient;
		Pol resultNum(rhs);
		resultNum *= CoeffType(get_num(c));
		*this = std::move(RationalFunction<Pol, AS>(std::move(resultNum), std::move(Pol(CoeffType(get_denom(c))))));
		return *this;
	}
	mIsSimplified = false;
	mPolynomialQuotient->first *= rhs;
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
template<typename P, DisableIf<needs_cache_type<P>>>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(Variable rhs) {
	if (this->is_constant()) {
		CoeffType c(this->mNumberQuotient);
		Pol resultNum(rhs);
		resultNum *= CoeffType(get_num(c));
		*this = std::move(RationalFunction<Pol, AS>(std::move(resultNum), std::move(Pol(CoeffType(get_denom(c))))));
		return *this;
	}
	mIsSimplified = false;
	mPolynomialQuotient->first *= rhs;
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(const typename Pol::CoeffType& rhs) {
	// TODO handle rhs == 0
	if (this->is_constant()) {
		this->mNumberQuotient *= rhs;
		return *this;
	}
	mIsSimplified = false;
	mPolynomialQuotient->first *= rhs;
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator*=(carl::sint rhs) {
	return *this *= carl::rationalize<typename Pol::CoeffType>(rhs);
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const RationalFunction<Pol, AS>& rhs) {
	if (this->is_constant() && rhs.is_constant()) {
		this->mNumberQuotient /= rhs.mNumberQuotient;
		return *this;
	} else if (this->is_constant()) {
		CoeffType c(this->mNumberQuotient);
		*this = rhs.inverse();
		return *this *= c;
	} else if (rhs.is_constant()) {
		return *this /= rhs.mNumberQuotient;
	}
	mIsSimplified = false;
	if (carl::is_one(rhs.denominatorAsPolynomial())) {
		return *this /= rhs.nominatorAsPolynomial();
	}
	mPolynomialQuotient->first *= rhs.denominatorAsPolynomial();
	mPolynomialQuotient->second *= rhs.nominatorAsPolynomial();
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const Pol& rhs) {
	if (this->is_constant()) {
		CoeffType c(this->mNumberQuotient);
		Pol resultNum(rhs);
		resultNum *= CoeffType(get_denom(c));
		*this = std::move(RationalFunction<Pol, AS>(std::move(Pol(CoeffType(get_num(c)))), std::move(resultNum)));
		return *this;
	}
	mIsSimplified = false;
	if (rhs.is_constant()) {
		mPolynomialQuotient->first /= rhs.constant_part();
	} else {
		mPolynomialQuotient->second *= rhs;
		eliminateCommonFactor(!AS);
	}
	return *this;
}

template<typename Pol, bool AS>
template<typename P, DisableIf<needs_cache_type<P>>>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(Variable rhs) {
	if (this->is_constant()) {
		CoeffType c(this->mNumberQuotient);
		Pol resultNum(rhs);
		resultNum *= CoeffType(get_denom(c));
		*this = RationalFunction<Pol, AS>(Pol(CoeffType(get_num(c))), std::move(resultNum));
		return *this;
	}
	mIsSimplified = false;
	mPolynomialQuotient->second *= rhs;
	eliminateCommonFactor(!AS);
	return *this;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(unsigned long rhs) {
	if (this->is_constant()) {
		this->mNumberQuotient /= CoeffType(rhs);
		return *this;
	}
	mIsSimplified = false;
	mPolynomialQuotient->first /= rhs;
	return *this;
}

template<typename Pol, bool AS>
RationalFunction<Pol, AS>& RationalFunction<Pol, AS>::operator/=(const typename Pol::CoeffType& rhs) {
	if (this->is_constant()) {
		this->mNumberQuotient /= rhs;
		return *this;
	}
	mIsSimplified = false;
	mPolynomialQuotient->first /= rhs;
	return *this;
}

template<typename Pol, bool AS>
bool operator==(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs) {
	if (lhs.is_constant()) {
		if (rhs.is_constant())
			return lhs.mNumberQuotient == rhs.mNumberQuotient;
		else
			return false;
	}
	if (rhs.is_constant())
		return false;
	return lhs.nominatorAsPolynomial() == rhs.nominatorAsPolynomial() && lhs.denominatorAsPolynomial() == rhs.denominatorAsPolynomial();
}

template<typename Pol, bool AS>
bool operator<(const RationalFunction<Pol, AS>& lhs, const RationalFunction<Pol, AS>& rhs) {
	if (lhs.is_constant()) {
		if (rhs.is_constant())
			return lhs.mNumberQuotient < rhs.mNumberQuotient;
		else
			return true;
	}
	if (rhs.is_constant())
		return false;
	return lhs.nominatorAsPolynomial() * rhs.denominatorAsPolynomial() < rhs.nominatorAsPolynomial() * lhs.denominatorAsPolynomial();
}

template<typename Pol, bool AS>
std::string RationalFunction<Pol, AS>::toString(bool infix, bool friendlyNames) const {

	std::string numeratorString = is_constant() ? carl::toString(nominatorAsNumber()) : nominatorAsPolynomial().toString(infix, friendlyNames);
	std::string denominatorString = is_constant() ? carl::toString(denominatorAsNumber()) : denominatorAsPolynomial().toString(infix, friendlyNames);

	if (denominator().is_one()) {
		return numeratorString;
	}

	if (infix) {
		return "(" + numeratorString + ")/(" + denominatorString + ")";
	} else {
		return "(/ " + numeratorString + " " + denominatorString + ")";
	}
}

template<typename Pol, bool AS>
std::ostream& operator<<(std::ostream& os, const RationalFunction<Pol, AS>& rhs) {
	if (rhs.is_constant())
		return os << rhs.mNumberQuotient;
	return os << "(" << rhs.nominatorAsPolynomial() << ")/(" << rhs.denominatorAsPolynomial() << ")";
}
} // namespace carl

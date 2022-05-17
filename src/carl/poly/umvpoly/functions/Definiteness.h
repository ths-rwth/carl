#pragma once

#include "Degree.h"
#include "SoSDecomposition.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

#include <iostream>

namespace carl {

/**
 * Regarding a polynomial \f$p\f$ as a function \f$p: X \rightarrow Y\f$, its definiteness gives information about the codomain \f$Y\f$.
 */
enum class Definiteness {
	/// Indicates that \f$y < 0 \forall y \in Y\f$.
	NEGATIVE = 0,
	/// Indicates that \f$y \leq 0 \forall y \in Y\f$.
	NEGATIVE_SEMI = 1,
	/// Indicates that values may be positive and negative.
	NON = 2,
	/// Indicates that \f$y \geq 0 \forall y \in Y\f$.
	POSITIVE_SEMI = 3,
	/// Indicates that \f$y > 0 \forall y \in Y\f$.
	POSITIVE = 4
};

inline std::ostream& operator<<(std::ostream& os, Definiteness d) {
	switch (d) {
		case Definiteness::NEGATIVE:
			return os << "negative";
		case Definiteness::NEGATIVE_SEMI:
			return os << "seminegative";
		case Definiteness::NON:
			return os << "none";
		case Definiteness::POSITIVE_SEMI:
			return os << "semipositive";
		case Definiteness::POSITIVE:
			return os << "positive";
	}
	return os;
}

template<typename Coeff>
Definiteness definiteness(const Term<Coeff>& t) {
	if (t.monomial()) {
		if (t.monomial()->isSquare()) {
			return (t.coeff() < Coeff(0) ? Definiteness::NEGATIVE_SEMI : Definiteness::POSITIVE_SEMI);
		}
	} else if (!carl::is_zero(t.coeff())) {
		return (t.coeff() < Coeff(0) ? Definiteness::NEGATIVE : Definiteness::POSITIVE);
	}
	return Definiteness::NON;
}

template<typename C, typename O, typename P>
Definiteness definiteness(const MultivariatePolynomial<C,O,P>& p, bool full_effort = true) {
	// Todo: handle constant polynomials
	if (is_linear(p)) {
		CARL_LOG_DEBUG("carl.core", "Linear and hence " << Definiteness::NON);
		return Definiteness::NON;
	}
	auto term = p.rbegin();
	if (term == p.rend()) return Definiteness::NON;
	Definiteness result = definiteness(*term);
	CARL_LOG_DEBUG("carl.core", "Got " << result << " from first term " << *term);
	++term;
	if (term == p.rend()) return result;
	if (result > Definiteness::NON) {
		for (; term != p.rend(); ++term) {
			Definiteness termDefin = definiteness(*term);
			if (termDefin > Definiteness::NON) {
				if( termDefin > result ) result = termDefin;
			} else {
				result = Definiteness::NON;
				break;
			}
		}
	} else if (result < Definiteness::NON) {
		for (; term != p.rend(); ++term) {
			Definiteness termDefin = definiteness(*term);
			if (termDefin < Definiteness::NON) {
				if( termDefin < result ) result = termDefin;
			} else {
				result = Definiteness::NON;
				break;
			}
		}
	}
	CARL_LOG_DEBUG("carl.core", "Eventually got " << result);
	if( full_effort && result == Definiteness::NON && total_degree(p) == 2 )
	{
		assert( !is_constant(p) );
		bool lTermNegative = carl::is_negative( p.lterm().coeff() );
		MultivariatePolynomial<C,O,P> tmp = p;
		if( p.hasConstantTerm() )
		{
			bool constPartNegative = carl::is_negative( p.constantPart() );
			if( constPartNegative != lTermNegative ) return Definiteness::NON;
			result = lTermNegative ? Definiteness::NEGATIVE : Definiteness::POSITIVE;
			tmp -= p.constantPart();
		}
		else
		{
			result = lTermNegative ? Definiteness::NEGATIVE_SEMI : Definiteness::POSITIVE_SEMI;
		}
		if( lTermNegative )
			tmp = -tmp;
		if( !sos_decomposition(tmp, true ).empty() ) return result;
		return Definiteness::NON;
	}
	return result;
}

}
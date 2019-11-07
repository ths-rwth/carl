#pragma once

#include "Degree.h"
#include "SoSDecomposition.h"

#include "../Definiteness.h"
#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

template<typename C, typename O, typename P>
Definiteness definiteness(const MultivariatePolynomial<C,O,P>& p, bool full_effort = true) {
	// Todo: handle constant polynomials
	if (is_linear(p)) {
		CARL_LOG_DEBUG("carl.core", "Linear and hence " << Definiteness::NON);
		return Definiteness::NON;
	}
	auto term = p.rbegin();
	if (term == p.rend()) return Definiteness::NON;
	Definiteness result = term->definiteness();
	CARL_LOG_DEBUG("carl.core", "Got " << result << " from first term " << *term);
	++term;
	if (term == p.rend()) return result;
	if (result > Definiteness::NON) {
		for (; term != p.rend(); ++term) {
			Definiteness termDefin = (term)->definiteness();
			if (termDefin > Definiteness::NON) {
				if( termDefin > result ) result = termDefin;
			} else {
				result = Definiteness::NON;
				break;
			}
		}
	} else if (result < Definiteness::NON) {
		for (; term != p.rend(); ++term) {
			Definiteness termDefin = (term)->definiteness();
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
		bool lTermNegative = carl::isNegative( p.lterm().coeff() );
		MultivariatePolynomial<C,O,P> tmp = p;
		if( p.hasConstantTerm() )
		{
			bool constPartNegative = carl::isNegative( p.constantPart() );
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
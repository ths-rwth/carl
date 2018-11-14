/** 
 * @file   IntervalEvaluation.h
 * @author: Sebastian Junges
 * @author: Stefan Schupp
 */


#pragma once
#include "Interval.h"
#include "power.h"

#include "../core/Monomial.h"
#include "../core/Term.h"
#include "../core/FactorizedPolynomial.h"
#include "../core/MultivariatePolynomial.h"



namespace carl
{

template<typename PolynomialType, class strategy  >
class MultivariateHorner; 

class IntervalEvaluation
{
public:
	template<typename Numeric>
	static Interval<Numeric> evaluate(const Monomial& m, const std::map<Variable, Interval<Numeric>>&);

	template<typename Coeff, typename Numeric, EnableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>&);
	
	template<typename Coeff, typename Numeric, DisableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>&);

	template<typename Coeff, typename Policy, typename Ordering, typename Numeric>
	static Interval<Numeric> evaluate(const MultivariatePolynomial<Coeff, Policy, Ordering>& p, const std::map<Variable, Interval<Numeric>>&);
    
	template<typename P, typename Numeric>
	static Interval<Numeric> evaluate(const FactorizedPolynomial<P>& p, const std::map<Variable, Interval<Numeric>>&);

	template<typename Numeric, typename Coeff, EnableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map);
	
	template<typename Numeric, typename Coeff, DisableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map);
	
	template<typename PolynomialType, typename Number, class strategy>
	static Interval<Number> evaluate(const MultivariateHorner<PolynomialType, strategy>& mvH, const std::map<Variable, Interval<Number>>& map);
    
private:

};


template<typename Numeric>
inline Interval<Numeric> IntervalEvaluation::evaluate(const Monomial& m, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(1);
	// TODO use iterator.
	CARL_LOG_TRACE("carl.core.monomial", "Iterating over " << m);
	for(unsigned i = 0; i < m.nrVariables(); ++i)
	{
		CARL_LOG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
		// We expect every variable to be in the map.
		CARL_LOG_ASSERT("carl.interval", map.count(m[i].first) > (size_t)0, "Every variable is expected to be in the map.");
		result *= carl::pow(map.at(m[i].first), m[i].second);
        if( result.isZero() )
            return result;
	}
	return result;
}

template<typename Coeff, typename Numeric, EnableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(t.coeff());
	if (t.monomial())
		result *= IntervalEvaluation::evaluate( *t.monomial(), map );
	return result;
}

template<typename Coeff, typename Numeric, DisableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(t.coeff());
	if (t.monomial())
		result *= IntervalEvaluation::evaluate( *t.monomial(), map );
	return result;
}

template<typename Coeff, typename Policy, typename Ordering, typename Numeric>
inline Interval<Numeric> IntervalEvaluation::evaluate(const MultivariatePolynomial<Coeff, Policy, Ordering>& p, const std::map<Variable, Interval<Numeric>>& map)
{
	CARL_LOG_FUNC("carl.core.monomial", p << ", " << map);
	if(p.isZero()) {
		return Interval<Numeric>(0);
	} else {
		Interval<Numeric> result(evaluate(p[0], map)); 
		for (unsigned i = 1; i < p.nrTerms(); ++i) {
            if( result.isInfinite() )
                return result;
			result += evaluate(p[i], map);
		}
		return result;
	}
}

template<typename P, typename Numeric>
inline Interval<Numeric> IntervalEvaluation::evaluate(const FactorizedPolynomial<P>& p, const std::map<Variable, Interval<Numeric>>& map)
{
    if( !existsFactorization( p ) )
        return Interval<Numeric>( p.coefficient() );
    if( p.factorizedTrivially() )
    {
        return evaluate( p.polynomial(), map ) * Interval<Numeric>( p.coefficient() );
    }
    else
    {
        Interval<Numeric> result( p.coefficient() );
        for( const auto& factor : p.factorization() )
        {
            Interval<Numeric> factorEvaluated = evaluate( factor.first, map );
            if( factorEvaluated.isZero() )
                return factorEvaluated;
            result *= factorEvaluated.pow( factor.second );
        }
        return std::move( result );
    }
}

template<typename Numeric, typename Coeff, EnableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map) {
	CARL_LOG_FUNC("carl.core.monomial", p << ", " << map);
	assert(map.count(p.mainVar()) > 0);
	Interval<Numeric> res = Interval<Numeric>::emptyInterval();
	const Interval<Numeric> varValue = map.at(p.mainVar());
	Interval<Numeric> exp(1);
	for (unsigned i = 0; i < p.degree(); i++) {
		res += p.coefficients()[i] * exp;
        if( res.isInfinite() )
            return res;
		exp = varValue.pow(i+1);
	}
	return res;
}

template<typename Numeric, typename Coeff, DisableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map) {
	CARL_LOG_FUNC("carl.core.monomial", p << ", " << map);
	assert(map.count(p.mainVar()) > 0);
	Interval<Numeric> res = Interval<Numeric>(carl::constant_zero<Numeric>().get());
	const Interval<Numeric>& varValue = map.at(p.mainVar());
	Interval<Numeric> exp(1);
	for (uint i = 0; i <= p.degree(); i++) {
		res += IntervalEvaluation::evaluate(p.coefficients()[i], map) * exp;
        if( res.isInfinite() )
            return res;
		exp = carl::pow(varValue, i+1);
	}
	return res;
}


template<typename PolynomialType, typename Number, class strategy>
inline Interval<Number> IntervalEvaluation::evaluate(const MultivariateHorner<PolynomialType, strategy>& mvH, const std::map<Variable, Interval<Number>>& map)
{
	#ifdef DEBUG_HORNER
		std::cout << __func__ << "   " << mvH << std::endl;
	#endif

	Interval<Number> result(1);
	CARL_LOG_FUNC("carl.core.monomial", mvH << ", " << map);

	if (mvH.getVariable() != Variable::NO_VARIABLE){
		assert(map.count(mvH.getVariable()) > 0);
		Interval<Number> res = Interval<Number>::emptyInterval();
		const Interval<Number> varValue = map.find(mvH.getVariable())->second;

		

		//Case 1: no further Horner schemes in mvH
		if (!mvH.getDependent() && !mvH.getIndependent())
		{
			result = ( varValue.pow(mvH.getExponent()) * Interval<Number> (mvH.getDepConstant()) ) + Interval<Number> (mvH.getIndepConstant());
			return result;
		}
		//Case 2: dependent part contains a Horner Scheme
		else if (mvH.getDependent() && !mvH.getIndependent())
		{
			result = varValue.pow(mvH.getExponent()) * evaluate(*mvH.getDependent(), map) + Interval<Number> (mvH.getIndepConstant());
			return result;
		}
		//Case 3: independent part contains a Horner Scheme
		else if (!mvH.getDependent() && mvH.getIndependent())
		{
			result = varValue.pow(mvH.getExponent()) * Interval<Number> (mvH.getDepConstant()) +  evaluate(*mvH.getIndependent(), map);
			return result;
		}
		//Case 4: both independent part and dependent part 
		else if (mvH.getDependent()  && mvH.getIndependent())
		{
			result = varValue.pow(mvH.getExponent()) * evaluate(*mvH.getDependent(), map) + evaluate(*mvH.getIndependent(), map);
			return result;
		}
	}
	else
	{
		result = Interval<Number> (mvH.getIndepConstant());
	}
	return result;
}

} //Namespace carl

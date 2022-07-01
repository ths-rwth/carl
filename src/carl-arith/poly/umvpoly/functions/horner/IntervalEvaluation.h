#pragma once
#include <carl-arith/interval/Interval.h>
#include <carl-arith/interval/power.h>

#include "../IntervalEvaluation.h"

// #include "MultivariateHorner.h"

namespace carl {

template<typename PolynomialType, class strategy  >
class MultivariateHorner; 

template<typename PolynomialType, typename Number, class strategy>
inline Interval<Number> evaluate(const MultivariateHorner<PolynomialType, strategy>& mvH, const std::map<Variable, Interval<Number>>& map)
{
	Interval<Number> result(1);
	CARL_LOG_FUNC("carl.core.intervalevaluation", mvH << ", " << map);

	if (mvH.getVariable() != Variable::NO_VARIABLE){
		assert(map.count(mvH.getVariable()) > 0);
		Interval<Number> res = Interval<Number>::empty_interval();
		const Interval<Number> varValue = map.find(mvH.getVariable())->second;

		//Case 1: no further Horner schemes in mvH
		if (!mvH.getDependent() && !mvH.getIndependent())
		{
			result = ( pow(varValue, mvH.getExponent()) * Interval<Number> (mvH.getDepConstant()) ) + Interval<Number> (mvH.getIndepConstant());
			return result;
		}
		//Case 2: dependent part contains a Horner Scheme
		else if (mvH.getDependent() && !mvH.getIndependent())
		{
			result = pow(varValue, mvH.getExponent()) * evaluate(*mvH.getDependent(), map) + Interval<Number> (mvH.getIndepConstant());
			return result;
		}
		//Case 3: independent part contains a Horner Scheme
		else if (!mvH.getDependent() && mvH.getIndependent())
		{
			result = pow(varValue, mvH.getExponent()) * Interval<Number> (mvH.getDepConstant()) +  evaluate(*mvH.getIndependent(), map);
			return result;
		}
		//Case 4: both independent part and dependent part 
		else if (mvH.getDependent()  && mvH.getIndependent())
		{
			result = pow(varValue, mvH.getExponent()) * evaluate(*mvH.getDependent(), map) + evaluate(*mvH.getIndependent(), map);
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

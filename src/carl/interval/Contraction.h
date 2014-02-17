/* 
 * File:   Contraction.h
 * Author: stefan
 *
 * Created on August 30, 2013, 4:55 PM
 */

#include "DoubleInterval.h"


#pragma once

namespace carl {

    template <template<typename> class Operator, typename Polynomial>
    class Contraction : private Operator<Polynomial> {

    private:
        const Polynomial mConstraint; // Todo: Should be a reference.
        std::map<Variable, Polynomial> mDerivatives;
    public:

        Contraction(const Polynomial& constraint) : Operator<Polynomial>(),
        mConstraint(constraint) {

        }

        bool operator()(const DoubleInterval::evaldoubleintervalmap& intervals, Variable::Arg variable, DoubleInterval& resA, DoubleInterval& resB) {
            typename std::map<Variable, Polynomial>::const_iterator it = mDerivatives.find(variable);
            if( it == mDerivatives.end() )
            {
                it = mDerivatives.emplace(variable, mConstraint.derivative(variable)).first;
            }
            return Operator<Polynomial>::contract(intervals, variable, mConstraint, (*it).second, resA, resB);
        }
    };

    template<typename Polynomial>
    class SimpleNewton {
    public:
        
        bool contract(const DoubleInterval::evaldoubleintervalmap& intervals, Variable::Arg variable, const Polynomial& constraint, const Polynomial& derivative, DoubleInterval& resA, DoubleInterval& resB) {
            double center = intervals.at(variable).midpoint();
            DoubleInterval centerInterval = DoubleInterval(center);

            // Create map for replacement of variables by intervals and replacement of center by point interval
            typename DoubleInterval::evaldoubleintervalmap substitutedIntervalMap = intervals;
            substitutedIntervalMap[variable] = centerInterval;

            // Create Newton Operator
            DoubleInterval numerator = IntervalEvaluation::evaluate(constraint, substitutedIntervalMap);
            DoubleInterval denominator = IntervalEvaluation::evaluate(derivative, intervals);
            DoubleInterval result1, result2;

            bool split = numerator.div_ext(result1, result2, denominator);
            if (split) {
                result1 = result1.inverse();
                result2 = result2.inverse();

                if(result1.isLessOrEqual(result2)) {
					resA = centerInterval.add(result1);
					resB = centerInterval.add(result2);
				}
				else
				{
					resA = centerInterval.add(result2);
					resB = centerInterval.add(result1);
                }
                return true;
            } else {
                result1 = result1.inverse();
                resA = centerInterval.add(result1);
                return false;
            }
        }
    };

    typedef Contraction<SimpleNewton, MultivariatePolynomial<cln::cl_RA>> SimpleNewtonContraction;

}
/* 
 * File:   Contraction.h
 * Author: stefan
 *
 * Created on August 30, 2013, 4:55 PM
 */

#include "Interval.h"
#include "../core/Sign.h"


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

        bool operator()(const Interval<double>::evalintervalmap& intervals, Variable::Arg variable, Interval<double>& resA, Interval<double>& resB) {
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
        
        bool contract(const Interval<double>::evalintervalmap& intervals, Variable::Arg variable, const Polynomial& constraint, const Polynomial& derivative, Interval<double>& resA, Interval<double>& resB) 
        {
            double center = intervals.at(variable).center();
            Interval<double> centerInterval = Interval<double>(center);
            
            // Create map for replacement of variables by intervals and replacement of center by point interval
            typename Interval<double>::evalintervalmap substitutedIntervalMap = intervals;
            substitutedIntervalMap[variable] = centerInterval;

            // Create Newton Operator
            Interval<double> numerator = IntervalEvaluation::evaluate(constraint, substitutedIntervalMap);
            Interval<double> denominator = IntervalEvaluation::evaluate(derivative, intervals);
            Interval<double> result1, result2;
            
            bool split = numerator.div_ext(denominator, result1, result2);
            if (split) {
                
                if(result1 <= result2) {
                    resB = centerInterval.sub(result1);
                    resA = centerInterval.sub(result2);
                }
                else
                {
                    resA = centerInterval.sub(result2);
                    resB = centerInterval.sub(result1);
                }
                return true;
            } else {
                resA = centerInterval.sub(result1);
                return false;
            }
        }
    };

    typedef Contraction<SimpleNewton, MultivariatePolynomial<cln::cl_RA>> SimpleNewtonContraction;

}
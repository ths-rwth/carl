/* 
 * File:   Contraction.h
 * Author: stefan
 *
 * Created on August 30, 2013, 4:55 PM
 */

#include "Interval.h"
#include "../core/Sign.h"
#include "IntervalEvaluation.h"


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
            
//			std::cout << __func__ << ": centerInterval: " << centerInterval << std::endl;
			
            // Create map for replacement of variables by intervals and replacement of center by point interval
            typename Interval<double>::evalintervalmap substitutedIntervalMap = intervals;
            substitutedIntervalMap[variable] = centerInterval;

            // Create Newton Operator
            Interval<double> numerator = IntervalEvaluation::evaluate(constraint, substitutedIntervalMap);
            Interval<double> denominator = IntervalEvaluation::evaluate(derivative, intervals);
            Interval<double> result1, result2;
			
//			std::cout << __func__ << ": numerator: " << numerator << ", denominator: " << denominator << std::endl;
            
            bool split = numerator.div_ext(denominator, result1, result2);
            if (split) {
//				std::cout << __func__ << ": caused split: " << result1 << " and " << result2 << std::endl;
                if(result1 >= result2) {
                    resA = intervals.at(variable).intersect(centerInterval.sub(result1));
                    resB = intervals.at(variable).intersect(centerInterval.sub(result2));
                    if( resB.isEmpty() )
                    {
                        return false;
                    }
                    if( resA.isEmpty() ) // resB is not empty at this state
                    {
                        resA = resB;
                        return false;
                    }
                }
                else
                {
                    resA = intervals.at(variable).intersect(centerInterval.sub(result2));
                    resB = intervals.at(variable).intersect(centerInterval.sub(result1));
                    if( resB.isEmpty() )
                    {
                        return false;
                    }
                    if( resA.isEmpty() ) // resB is not empty at this state
                    {
                        resA = resB;
                        return false;
                    }
                }
                return true;
            } else {
//				std::cout << __func__ << ": result: " << result1 << std::endl;
                resA = intervals.at(variable).intersect(centerInterval.sub(result1));
                return false;
            }
        }
    };

    typedef Contraction<SimpleNewton, MultivariatePolynomial<cln::cl_RA>> SimpleNewtonContraction;

}
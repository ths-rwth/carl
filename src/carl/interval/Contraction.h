/* 
 * File:   Contraction.h
 * Author: stefan
 *
 * Created on August 30, 2013, 4:55 PM
 */

#include "DoubleInterval.h"


#pragma once

namespace carl {

    template <typename Polynomial, typename Operator>
    class Contraction : private Operator {
        using Polynomial::derivative(Variable);
    private:
        const Polynomial& mConstraint;
        std::map<Variable, Polynomial> mDerivatives;
    public:

        Contraction(const Polynomial& constraint) : Operator(),
        mConstraint(constraint) {

        }

        bool operator()(const DoubleInterval::evaldoubleintervalmap& intervals, Variable::Arg variable, DoubleInterval& resA, DoubleInterval& resB) {
            std::map<Variable, Polynomial>::const_iterator it = mDerivatives.find(variable);
            if( it == mDerivatives.end() )
            {
                it = mDerivatives.emplace(variable, mConstraint.derivative(variable));
            }
            return contract(intervals, variable, mConstraint, (*it).second, resA, resB);
        }
    };

    template<typename Polynomial>
    class SimpleNewton {
    public:
        
        bool contract(const DoubleInterval::evaldoubleintervalmap& intervals, Variable::Arg variable, const Polynomial& constraint, const Polynomial& derivative, DoubleInterval& resA, DoubleInterval& resB) {
            using DoubleInterval::evaldoubleintervalmap;
            double center = intervals.at(variable).midpoint();
            DoubleInterval centerInterval = DoubleInterval(center);

            // Create map for replacement of variables by intervals and replacement of center by point interval
            evaldoubleintervalmap substitutedIntervalMap = intervals;
            substitutedIntervalMap[variable] = centerInterval;

            // Create Newton Operator
            //        DoubleInterval numerator   = GiNaCRA::DoubleInterval::evaluate( _constraint.polynomial(), substitutedIntervalMap );
            DoubleInterval numerator = IntervalEvaluation::evaluate(constraint, substitutedIntervalMap, true);
            DoubleInterval denominator = IntervalEvaluation::evaluate(derivative, intervals, true);
            DoubleInterval result1, result2;

            bool split = numerator.div_ext(result1, result2, denominator);
            if (split) {
                result1 = result1.minus();
                //            resultA = centerInterval.add( result1 );
                result2 = result2.minus();
                //            resultB = centerInterval.add( result2 );

                switch (result1.isLessOrEqual(result2)) {
                    case true:
                        resA = centerInterval.add(result1);
                        resB = centerInterval.add(result2);
                        break;
                    case false:
                        resA = centerInterval.add(result2);
                        resB = centerInterval.add(result1);
                }
                return true;
            } else {
                result1 = result1.minus();
                resA = centerInterval.add(result1);
                return false;
            }
        }
    };

    typedef Contraction<SimpleNewton> SimpleNewtonContraction;

}
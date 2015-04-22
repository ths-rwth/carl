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

//#define CONTRACTION_DEBUG

namespace carl {
    
    template<typename Polynomial>
    class VarSolutionFormula
    {
        private:
            /// Stores n, if the nth root has to be taken of mNumerator/mDenominator
            unsigned mRoot;
            /// Stores the numerator
            Polynomial mNumerator;
            /// Stores the denominator, which is one, if mDenominator == nullptr
            std::shared_ptr<const typename Polynomial::MonomType> mDenominator;
            
        public:
            VarSolutionFormula() = delete;
            
            /**
             * Constructs the solution formula for the given variable x in the equation p = 0, where p is the given polynomial.
             * The polynomial p must have one of the following forms: 
             *      1.) ax+h, with a being a rational number and h a linear polynomial not containing x and not having a constant part
             *      2.) x^i*m-y, with i being a positive integer, m being a monomial not contains x and y being a variable different from x
             * @param p The polynomial containing the given variable to construct a solution formula for.
             * @param x The variable to construct a solution formula for.
             */
            VarSolutionFormula(const Polynomial& p, Variable::Arg x):
                mRoot(1),
                mNumerator(),
                mDenominator(nullptr)
            {
                assert(p.has(x));
                assert(!p.hasConstantTerm());
                assert(p.isLinear() || (p.nrTerms() == 2 && carl::isOne(p.begin()->coeff()) && carl::isOne(p.rbegin()->coeff()) 
                                        && ((p.begin()->has(x) && !p.rbegin()->has(x) && p.rbegin()->isLinear()) 
                                            || (p.rbegin()->has(x) && !p.begin()->has(x) && p.begin()->isLinear()))));
                // Construct the solution formula for x in p = 0
                // @todo fill out
            }
            
            /**
             * Evaluates this solution formula for the given mapping of the variables occurring in the solution formula to double intervals.
             * @param intervals The mapping of the variables occurring in the solution formula to double intervals
             * @param resA The first interval of the result.
             * @param resB The second interval of the result.
             * @return true, if the second interval is not empty. (the first interval must then be also nonempty)
             */
            bool evaluate(const Interval<double>::evalintervalmap& intervals, Interval<double>& resA, Interval<double>& resB) const
            {
                // @todo fill out
                return false;
            }
    };

    template <template<typename> class Operator, typename Polynomial>
    class Contraction : private Operator<Polynomial> {

    private:
        const Polynomial mConstraint; // Todo: Should be a reference.
        std::map<Variable, Polynomial> mDerivatives;
        std::map<Variable, VarSolutionFormula<Polynomial>> mVarSolutionFormulas;
    public:

        Contraction(const Polynomial& constraint) : Operator<Polynomial>(),
        mConstraint(constraint),
        mDerivatives(),
        mVarSolutionFormulas() {}

        bool operator()(const Interval<double>::evalintervalmap& intervals, Variable::Arg variable, Interval<double>& resA, Interval<double>& resB, bool useNiceCenter = false, bool withPropagation = false) {
            typename std::map<Variable, Polynomial>::const_iterator it = mDerivatives.find(variable);
            if( it == mDerivatives.end() )
            {
                it = mDerivatives.emplace(variable, mConstraint.derivative(variable)).first;
            }
            bool splitOccurredInContraction = Operator<Polynomial>::contract(intervals, variable, mConstraint, (*it).second, resA, resB, useNiceCenter);
            if( withPropagation )
            {
                typename std::map<Variable, VarSolutionFormula<Polynomial>>::const_iterator itB = mVarSolutionFormulas.find(variable);
                if( itB == mVarSolutionFormulas.end() )
                {
                    itB = mVarSolutionFormulas.emplace(variable, std::move(VarSolutionFormula<Polynomial>(mConstraint,variable))).first;
                }
                if (withPropagation) {
                    Interval<double> resultPropagation1, resultPropagation2;
                    bool splitOccurredInEvaluation = itB->second.evaluate( intervals, resultPropagation1, resultPropagation2 );
                    if( splitOccurredInContraction && splitOccurredInEvaluation )
                    {
                        // @todo here we could get three intervals, e.g., resA = [0,2], resB = [3,7], resultPropagation1 = [1,4] and resultPropagation2 = [6,7] results in [1,2],[3,4],[6,7]
                        // we could merge two of them, e.g., those with the least gap between them, in given example we would then get [1,4],[6,7]
                    }
                    else if( splitOccurredInContraction )
                    {
                        assert( resultPropagation2.isEmpty() );
                        resA = resA.intersect( resultPropagation1 );
                        resB = resB.intersect( resultPropagation1 );
                    }
                    else if( splitOccurredInEvaluation )
                    {
                        assert( resB.isEmpty() );
                        resA = resA.intersect( resultPropagation1 );
                        resB = resA.intersect( resultPropagation2 );
                    }
                    else // no split occurred
                    {
                        assert( resultPropagation2.isEmpty() );
                        assert( resB.isEmpty() );
                        resA = resA.intersect( resultPropagation1 );
                    }
                    if( resA.isEmpty() || resB.isEmpty() )
                    {
                        resA = resB;
                        resB = Interval<double>::emptyInterval();
                        return false;
                    }
                    return true;
                }
            }
            return splitOccurredInContraction;
        }
    };

    template<typename Polynomial>
    class SimpleNewton {
    public:
        
        bool contract(const Interval<double>::evalintervalmap& intervals, Variable::Arg variable, const Polynomial& constraint, const Polynomial& derivative, Interval<double>& resA, Interval<double>& resB, bool useNiceCenter = false) 
        {
            bool splitOccurred = false;
            double center = useNiceCenter ? intervals.at(variable).sample() : intervals.at(variable).center();
            Interval<double> centerInterval = Interval<double>(center);
            
			#ifdef CONTRACTION_DEBUG
            std::cout << "variable = " << variable << std::endl;
            std::cout << "constraint = " << constraint << std::endl;
            std::cout << "derivative = " << derivative << std::endl;
			std::cout << __func__ << ": centerInterval: " << centerInterval << std::endl;
			#endif
			
            // Create map for replacement of variables by intervals and replacement of center by point interval
            typename Interval<double>::evalintervalmap substitutedIntervalMap = intervals;
            substitutedIntervalMap[variable] = centerInterval;

            // Create Newton Operator
            Interval<double> numerator = IntervalEvaluation::evaluate(constraint, substitutedIntervalMap);
            Interval<double> denominator = IntervalEvaluation::evaluate(derivative, intervals);
            Interval<double> result1, result2;
			
			#ifdef CONTRACTION_DEBUG
			std::cout << __func__ << ": numerator: " << numerator << ", denominator: " << denominator << std::endl;
			#endif
            
            bool split = numerator.div_ext(denominator, result1, result2);
            if (split) {
				#ifdef CONTRACTION_DEBUG
				std::cout << __func__ << ": caused split: " << result1 << " and " << result2 << std::endl;
				#endif
                splitOccurred = true;
                if(result1 >= result2) {
                    resA = intervals.at(variable).intersect(centerInterval.sub(result1));
                    resB = intervals.at(variable).intersect(centerInterval.sub(result2));
					if (variable.getType() == VariableType::VT_INT) {
						resA = resA.integralPart();
						resB = resB.integralPart();
					}
					#ifdef CONTRACTION_DEBUG
					std::cout << __func__ << ": result after intersection: " << resA << " and " << resB << std::endl;
					#endif
                    if( resB.isEmpty() )
                    {
                        splitOccurred = false;
                    }
                    else if( resA.isEmpty() ) // resB is not empty at this state
                    {
                        resA = resB;
                        resB = Interval<double>::emptyInterval();
                        splitOccurred = false;
                    }
                }
                else
                {
                    resA = intervals.at(variable).intersect(centerInterval.sub(result2));
                    resB = intervals.at(variable).intersect(centerInterval.sub(result1));
					if (variable.getType() == VariableType::VT_INT) {
						resA = resA.integralPart();
						resB = resB.integralPart();
					}
					#ifdef CONTRACTION_DEBUG
					std::cout << __func__ << ": result after intersection: " << resA << " and " << resB << std::endl;
					#endif
                    if( resB.isEmpty() )
                    {
                        splitOccurred = false;
                    }
                    else if( resA.isEmpty() ) // resB is not empty at this state
                    {
                        resA = resB;
                        resB = Interval<double>::emptyInterval();
                        splitOccurred = false;
                    }
                }
            } else {
				#ifdef CONTRACTION_DEBUG
				std::cout << __func__ << ": result: " << result1 << std::endl;
				#endif
                resA = intervals.at(variable).intersect(centerInterval.sub(result1));
				if (variable.getType() == VariableType::VT_INT) {
					resA = resA.integralPart();
				}
				#ifdef CONTRACTION_DEBUG
				std::cout << __func__ << ": result after intersection: " << resA << std::endl;
				#endif
            }
            return splitOccurred;
        }
    };

    typedef Contraction<SimpleNewton, Polynomial> SimpleNewtonContraction;

}
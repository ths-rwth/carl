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
//                std::cout << "[p] " << p << "[x]" << x << std::endl;
                //std::cout << "[nrTerms] " << p.nrTerms() << std::endl;
                //std::cout << "[isOne] " << carl::isOne(p.begin()->coeff()) << std::endl;
                //std::cout << "[isOne] " << carl::isOne(p.rbegin()->coeff()) << std::endl;
                //std::cout << "[begLin] " << p.begin()->isLinear() << std::endl;
                //std::cout << "[endLin] " << p.rbegin()->isLinear() << std::endl;

                assert(p.has(x));
                assert(!p.hasConstantTerm());
                assert(p.isLinear() || (p.nrTerms() == 2 && (carl::isOne(p.begin()->coeff()) || carl::isOne(p.rbegin()->coeff()))
                                        && ((p.begin()->has(x) && !p.rbegin()->has(x)) 
                                        || (p.rbegin()->has(x) && !p.begin()->has(x)))));
                
                // Construct the solution formula for x in p = 0
                typename Polynomial::TermsType::const_iterator yIter;
                typename Polynomial::TermsType::const_iterator xIter;

                //std::cout << "[propagation]:";
                // Case 1.):
                if (p.isLinear())
                {
                    //std::cout << "[Case 1]" << std::endl;
                    Polynomial (t1);
                    yIter = p.begin();
                    
                    for (const auto& t: p) {
                        assert(t.monomial() != nullptr);
                        if (t.has(x)) {
                            
                            mNumerator = p / t.coeff();
                            mNumerator -= x;
                            mNumerator *= (-1);
//                            std::cout << "[mNumerator]" << mNumerator << std::endl;
                            break;
                        }
                    }                    
                }

                // Case 2.)
                else
                {   
                    assert(p.nrTerms() == 2);

                    if (p.begin()->has(x))
                    {
                        xIter = p.begin();   
                        yIter = xIter;
                        ++yIter;
                    } 
                    else
                    {
                        yIter = p.begin();   
                        xIter = yIter;
                        ++xIter;
                    }

                    assert(xIter->monomial() != nullptr);
                    assert(!xIter->isLinear() || xIter->coeff() == (-1));
                    //std::cout << "[Case 2]" << std::endl;
                    if (xIter->isLinear()) 
                    {
                        mNumerator = Polynomial ( *yIter );
                    }
                    else
                    {
                        mRoot = xIter->monomial()->exponentOfVariable(x);
//                        std::cout << "[mRoot]" << mRoot << std::endl;                  
                        mDenominator = xIter->monomial()->dropVariable(x);
//                        std::cout << "[mDenominator]" << mDenominator << std::endl;
                        mNumerator = -Polynomial ( *yIter );
                    }
//                    std::cout << "[mNumerator]" << mNumerator << std::endl;                    
                }
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
                // evaluate monomial
                //std::cout << "[PreAll] resA: " << resA << "resB: " << resB << std::endl;
                Interval<double> numerator = IntervalEvaluation::evaluate(mNumerator, intervals);
                //std::cout << "[numerator] " << numerator << std::endl;
                if (mDenominator == nullptr)
                {
                    resA = std::move(numerator.root((int) mRoot));
                    //std::cout << "[postMove] " << resA << std::endl;
                    resB = Interval<double>::emptyInterval();
                    return false;
                }
                
                Interval<double> denominator = IntervalEvaluation::evaluate(*mDenominator, intervals);        
                Interval<double> result1, result2;

                // divide:
                //std::cout << "[PreDiv] numerator: " << numerator << "denominator: " << denominator << std::endl;
                bool split = numerator.div_ext(denominator, result1, result2);

                // extract root:
                assert(mRoot <= std::numeric_limits<int>::max());

                //std::cout << "[PreRoot] Res1: " << result1 << "Res2: " << result2 << std::endl;
                resA = result1.root((int) mRoot);
                if (split)
                {
                    resB = result2.root((int) mRoot);
                }

                //std::cout << "[PreRoot] resA: " << resA << "resB: " << resB << std::endl;
                if (split && !resB.isEmpty()) return true;
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
//            std::cout << "contraction of " << variable << " with " << intervals << " in " << mConstraint << std::endl;
            bool splitOccurredInContraction = Operator<Polynomial>::contract(intervals, variable, mConstraint, (*it).second, resA, resB, useNiceCenter);
//            std::cout << "  after contraction: " << resA;
//            if( splitOccurredInContraction ) std::cout << " and " << resB;
//            std::cout << std::endl;
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
//                    std::cout << "  after propagation: " << resultPropagation1;
//                    if( splitOccurredInEvaluation ) std::cout << " and " << resultPropagation2;
//                    std::cout << std::endl;
                    //std::cout << "[splitOccurred in Eval] "<< splitOccurredInEvaluation << " [splitOccurredInContraction] " << splitOccurredInContraction << std::endl;
                    if( splitOccurredInContraction && splitOccurredInEvaluation )
                    {
                        // @todo here we could get three intervals, e.g., resA = [0,2], resB = [3,7], resultPropagation1 = [1,4] and resultPropagation2 = [6,7] results in [1,2],[3,4],[6,7]
                        // we could merge two of them, e.g., those with the least gap between them, in given example we would then get [1,4],[6,7]
                        //std::cout << "[res1][1] " << resultPropagation1 << "[res2][1] " << resultPropagation2 << std::endl;

                        std::vector<Interval<double>> resultingIntervals;
                        Interval<double> tempInterval;
                        
                        //intersecting resulting intervals and adding them to a vector in case they are non-empty
                        tempInterval = resA.intersect( resultPropagation1 );
                        if (!tempInterval.isEmpty()) resultingIntervals.push_back(tempInterval);
                        //std::cout << "[resA][1] " << resA << "[resB][1] " << resB << std::endl;
            
                        tempInterval = resA.intersect( resultPropagation2 );
                        if (!tempInterval.isEmpty()) resultingIntervals.push_back(tempInterval);
                        //std::cout << "[resA][2] " << resA << "[resB][2] " << resB << std::endl;

                        tempInterval = resB.intersect( resultPropagation1 );
                        if (!tempInterval.isEmpty()) resultingIntervals.push_back(tempInterval);
                        //std::cout << "[resA][3] " << resA << "[resB][3] " << resB << std::endl;
                        
                        tempInterval = resB.intersect( resultPropagation2 );
                        if (!tempInterval.isEmpty()) resultingIntervals.push_back(tempInterval);
                        //std::cout << "[resA][4] " << resA << "[resB][4] " << resB << std::endl;

                        //std::cout << "[resultingIntervals size] " << resultingIntervals.size() << std::endl;
                        assert (resultingIntervals.size() < 4);

                        //checking how many intervals were generated
                        if (resultingIntervals.size() >= 1)
                        {
                            resA = resultingIntervals[0];
                        }

                        if (resultingIntervals.size() == 2)
                        {
                            resB = resultingIntervals[1];
                        }

                        if (resultingIntervals.size() == 3)
                        {
                            size_t indexA = 0;
                            size_t indexB = 0;
                            double bestDistance = resultingIntervals[0].distance(resultingIntervals[1]);
                            double currentDistance = bestDistance;

                            for (size_t i = 0; i < 3; i++)
                            {
                                for (size_t j = 0; j < 3; j++)
                                {
                                    if(j != i)
                                    {
                                        currentDistance = resultingIntervals[i].distance(resultingIntervals[j]);
                                        if (currentDistance <= bestDistance)   
                                        {
                                            bestDistance = currentDistance;
                                            indexA = i;
                                            indexB = j;
                                            
                                        }
                                    }
                                }
                            }
                            Interval<double> mergeCandidateA = resultingIntervals[indexA];
                            Interval<double> mergeCandidateB = resultingIntervals[indexB];

                            // merge the two Intervals which are closest to each other
                            resA = mergeCandidateA.convexHull(mergeCandidateB);

                            //finding the non-merged interval
                            resB = resultingIntervals[(indexA+indexB-1)%3];    
                            //std::cout << "[resA][5] " << resA << "[resB][5] " << resB << std::endl;                    
                        }
                    }
                    else if( splitOccurredInContraction )
                    {
                        assert( resultPropagation2.isEmpty() );
                        resA = resA.intersect( resultPropagation1 );
                        resB = resB.intersect( resultPropagation1 );
                        //std::cout << "[resA][5] " << resA << "[resB][5] " << resB << std::endl;

                    }
                    else if( splitOccurredInEvaluation )
                    {
                        assert( resB.isEmpty() );
                        resA = resA.intersect( resultPropagation1 );
                        resB = resA.intersect( resultPropagation2 );
                        //std::cout << "[resA][5] " << resA << "[resB][5] " << resB << std::endl;
                    }
                    else // no split occurred
                    {
                        assert( resultPropagation2.isEmpty() );
                        assert( resB.isEmpty() );

                        resA = resA.intersect( resultPropagation1 );
                        //std::cout << "[resA][5] " << resA << "[resB][5] " << resB << std::endl;
                    }
                    if( resA.isEmpty() && !resB.isEmpty() )
                    {
                        resA = resB;
                        resB = Interval<double>::emptyInterval();
                        //std::cout << "[resA][6] " << resA << "[resB][6] " << resB << std::endl;
//                        std::cout << "  after both: " << resA << std::endl;
                        return false;
                    }
//                    std::cout << "  after both: " << resA;
//                    if( !resB.isEmpty() ) std::cout << " and " << resB;
//                    std::cout << std::endl;
                    return !resB.isEmpty();
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

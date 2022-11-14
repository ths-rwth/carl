/* 
 * File:   Contraction.h
 * Author: stefan
 *
 * Created on August 30, 2013, 4:55 PM
 */

#pragma once
#include <carl-arith/interval/Interval.h>
#include <carl-arith/interval/SetTheory.h>
#include <carl-arith/core/Sign.h>
#include <carl-arith/poly/umvpoly/functions/horner/MultivariateHorner.h>
#include <carl-arith/poly/umvpoly/functions/IntervalEvaluation.h>
#include <algorithm>

//#define CONTRACTION_DEBUG
//#define USE_HORNER

namespace carl {
    
    template<typename Polynomial>
    class VarSolutionFormula
    {
        private:
            /// The variable, for which to solve.
            Variable mVar;
            /// Stores n, if the nth root has to be taken of mNumerator/mDenominator
            uint mRoot;
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
             *      2.) x^i*m-y, with i being a positive integer, m being a monomial not containing x and y being a variable different from x
             * @param p The polynomial containing the given variable to construct a solution formula for.
             * @param x The variable to construct a solution formula for.
             */

            VarSolutionFormula(const Polynomial& p, Variable::Arg x):
                mVar(x),
                mRoot(1),
                mNumerator(),
                mDenominator(nullptr)
            {
        
                #ifdef CONTRACTION_DEBUG
                std::cout << __func__ << ": [Polynome]: " << p << " [#Terms]: " << p.nr_terms() << std::endl;     
                #endif


                assert(p.has(x));
                assert(!p.has_constant_term());
                assert(p.is_linear() || (p.nr_terms() == 2 && (carl::is_one(p.begin()->coeff()) || carl::is_one(p.rbegin()->coeff()))
                                        && ((p.begin()->has(x) && !p.rbegin()->has(x)) 
                                        || (p.rbegin()->has(x) && !p.begin()->has(x)))));
                
                // Construct the solution formula for x in p = 0

                #ifdef CONTRACTION_DEBUG
                std::cout << __func__ << ": Propagating... " << std::endl;     
                #endif

                // Case 1.):
                if (p.is_linear())
                {
                    #ifdef CONTRACTION_DEBUG
                    std::cout << __func__ << ": Case 1. (linear)... " << std::endl;     
                    #endif
                    for (const auto& t: p) {
                        assert(t.monomial() != nullptr);
                        if (t.has(x)) {
                            
                            mNumerator = p / t.coeff();
                            mNumerator -= x;
                            mNumerator *= (-1);
                            
                            #ifdef CONTRACTION_DEBUG
                            std::cout << __func__ << ": Setting mNumerator:" << mNumerator << std::endl;     
                            #endif

                            return;
                        }
                    }                    
                }

                // Case 2.)
                else
                {   
                    #ifdef CONTRACTION_DEBUG
                    std::cout << __func__ << ": Case 2. (non-linear)... " << std::endl;     
                    #endif

                    assert(p.nr_terms() == 2);
                    typename Polynomial::TermsType::const_iterator yIter;
                    typename Polynomial::TermsType::const_iterator xIter;
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
                    assert(!xIter->is_linear() || xIter->coeff() == (-1));
                    //std::cout << "[Case 2]" << std::endl;
                    if (xIter->is_linear()) 
                    {
                        mNumerator = Polynomial ( *yIter );
                    }
                    else
                    {
                        mRoot = xIter->monomial()->exponent_of_variable(x);
                        #ifdef CONTRACTION_DEBUG
                        std::cout << __func__ << ": Setting mRoot:" << mRoot << std::endl;     
                        #endif
              
                        mDenominator = xIter->monomial()->drop_variable(x);
                        #ifdef CONTRACTION_DEBUG
                        std::cout << __func__ << ": Setting mDenominator:" << mDenominator << std::endl;     
                        #endif
                        mNumerator = -Polynomial ( *yIter );
                    }
                    #ifdef CONTRACTION_DEBUG
                    std::cout << __func__ << ": Setting mNumerator:" << mNumerator << std::endl;     
                    #endif                         
                }
            }
            
            void addRoot( const Interval<double>& _interv, const Interval<double>& _varInterval, std::vector<Interval<double>>& _result ) const
            {
                Interval<double> tmp = _interv.root((int) mRoot);
				CARL_LOG_DEBUG("carl.contraction", mRoot << "th root of " << _interv << " = " << tmp);
                if( mRoot % 2 == 0 )
                {
                    Interval<double> rootA, rootB;
                    if( carl::set_union(tmp, -tmp, rootA, rootB ) )
                    {   
						CARL_LOG_DEBUG("carl.contraction", tmp << " and " << -tmp << " = " << rootA << " and " << rootB);
                        if (mVar.type() == VariableType::VT_INT) {
                            rootA = rootA.integral_part();
                        }
                        rootA = set_intersection(rootA, _varInterval);
						CARL_LOG_DEBUG("carl.contraction", "first intersected with " << _varInterval << " = " << rootA);
                        if( !rootA.is_empty() )
                            _result.push_back(std::move(rootA));
                        if (mVar.type() == VariableType::VT_INT) {
                            rootB = rootB.integral_part();
                        }
                        rootB = set_intersection(rootB, _varInterval);
						CARL_LOG_DEBUG("carl.contraction", "second intersected with " << _varInterval << " = " << rootB);
                        if( !rootB.is_empty() )
                            _result.push_back(std::move(rootB));
                    }
                    else
                    {
						CARL_LOG_DEBUG("carl.contraction", tmp << " and " << -tmp << " = " << rootA << " and " << rootB);
                        if (mVar.type() == VariableType::VT_INT) {
                            rootA = rootA.integral_part();
                        }
                        rootA = set_intersection(rootA, _varInterval);
                        if( !rootA.is_empty() )
                            _result.push_back(std::move(rootA));
                    }
                }
                else
                {
                    if (mVar.type() == VariableType::VT_INT) {
                        tmp = tmp.integral_part();
                    }
                    _result.push_back( std::move(tmp) );
                }
            }
            
            /**
             * Evaluates this solution formula for the given mapping of the variables occurring in the solution formula to double intervals.
             * @param intervals The mapping of the variables occurring in the solution formula to double intervals
             * @param resA The first interval of the result.
             * @param resB The second interval of the result.
             * @return true, if the second interval is not empty. (the first interval must then be also nonempty)
             */
            std::vector<Interval<double>> evaluate(const Interval<double>::evalintervalmap& intervals) const
            {
                // evaluate monomial
                std::vector<Interval<double>> result;
                assert( intervals.find(mVar) != intervals.end() );
                const Interval<double>& varInterval = intervals.at(mVar);
                Interval<double> numerator = carl::evaluate(mNumerator, intervals);
				CARL_LOG_DEBUG("carl.contraction", mNumerator << " -> " << numerator);
                if (mDenominator == nullptr)
                {
                    addRoot( numerator, varInterval, result );
					CARL_LOG_DEBUG("carl.contraction", "no denominator -> " << result);
                    return result;
                }
                Interval<double> denominator = carl::evaluate(*mDenominator, intervals);  
				CARL_LOG_DEBUG("carl.contraction", *mDenominator << " -> " << denominator);
                Interval<double> result1, result2;
                // divide:
                bool split = numerator.div_ext(denominator, result1, result2);
				CARL_LOG_DEBUG("carl.contraction", numerator << " / " << denominator << " -> " << result1 << " and " << result2);
                // extract root:
                assert(mRoot <= std::numeric_limits<int>::max());
                if( split )
                {
                    // TODO: division returns two intervals, which might be united to one interval
                    Interval<double> tmpA, tmpB;
                    if( carl::set_union(result1, result2, tmpA, tmpB ) )
                    {
                        addRoot( tmpA, varInterval, result );
                        addRoot( tmpB, varInterval, result );
                    }
                    else
                    {
                        addRoot( tmpA, varInterval, result );
                    }
                }
                else
                {
                    addRoot( result1, varInterval, result );
                }
				CARL_LOG_DEBUG("carl.contraction", "Finally: " << result);
                return result;
            }
    };

    template <template<typename> class Operator, typename Polynomial>
    class Contraction : private Operator<Polynomial> { 

    private:
        Polynomial mConstraint; // Todo: Should be a reference.
        Polynomial* mpOriginal;
        #ifdef USE_HORNER
        MultivariateHorner<Polynomial, strategy> mHornerForm;
        std::map<Variable, MultivariateHorner<Polynomial,strategy>> mDerivatives;
        #else
        std::map<Variable, Polynomial> mDerivatives;
        #endif
        std::map<Variable, VarSolutionFormula<Polynomial>> mVarSolutionFormulas;
        std::map<Polynomial, MultivariateHorner<Polynomial,strategy>> mHornerSchemes;

    public:
        Contraction() = delete;
        Contraction(const Polynomial& constraint):
			Operator<Polynomial>(),
            mConstraint(constraint),
            mpOriginal(nullptr),
            #ifdef USE_HORNER
            mHornerForm(constraint),
            #endif
            mDerivatives(),
            mVarSolutionFormulas(),
			mHornerSchemes()
        {}

        Contraction(const Polynomial& constraint, const Polynomial& _original ):
			Operator<Polynomial>(),
            mConstraint(constraint),
            mpOriginal (_original.is_linear() ? nullptr : new Polynomial(_original)),
            #ifdef USE_HORNER
            mHornerForm( mpOriginal == nullptr ? constraint :  _original ),
            #endif
            mDerivatives(),
            mVarSolutionFormulas() ,
			mHornerSchemes()
        {}
        Contraction(const Contraction&) = delete;
        
        Contraction(Contraction&& _contraction ):
			Operator<Polynomial>(),
            mConstraint(std::move(_contraction.mConstraint)),
            mpOriginal(_contraction.mpOriginal),
            #ifdef USE_HORNER
            mHornerForm(std::move(_contraction.mHornerForm)),
            #endif
            mDerivatives(std::move(_contraction.mDerivatives)),
            mVarSolutionFormulas(std::move(_contraction.mVarSolutionFormulas)),
			mHornerSchemes(std::move(_contraction.mHornerSchemes))
        {
            _contraction.mpOriginal = nullptr;
        }
        
        Contraction& operator=(const Contraction&) = delete;
        Contraction& operator=(Contraction&&) = delete;

        ~Contraction()
        {
            if( mpOriginal != nullptr )
                delete mpOriginal;
        }
        
        const Polynomial& polynomial() const
        {
            return mpOriginal == nullptr ? mConstraint : *mpOriginal;
        }

        bool operator()(const Interval<double>::evalintervalmap& intervals, Variable::Arg variable, Interval<double>& resA, Interval<double>& resB, bool useNiceCenter = false, bool usePropagation = false)
        {
            bool splitOccurredInContraction = false;
            if( !usePropagation || mpOriginal == nullptr || !mConstraint.is_linear() )
            {
                #ifdef USE_HORNER
                typename std::map<Variable, MultivariateHorner<Polynomial,strategy>>::const_iterator it = mDerivatives.find(variable);
                #else
                typename std::map<Variable, Polynomial>::const_iterator it = mDerivatives.find(variable);
                #endif

                if( it == mDerivatives.end() )
                {
                    #ifdef USE_HORNER
                    //Deriviate and convert to Horner
                    if( mpOriginal == nullptr )
                        it = mDerivatives.emplace(variable, std::move(MultivariateHorner<Polynomial, strategy>( mConstraint.derivative(variable)))).first;
                    else
                        it = mDerivatives.emplace(variable, std::move(MultivariateHorner<Polynomial, strategy>( mpOriginal->derivative(variable)))).first;
                    #else
                    if( mpOriginal == nullptr )
                        it = mDerivatives.emplace(variable, derivative(mConstraint, variable)).first;
                    else
                        it = mDerivatives.emplace(variable, derivative(*mpOriginal, variable)).first;
                    #endif
                }

                #ifdef CONTRACTION_DEBUG
                std::cout << __func__ << ": contraction of " << variable << " with " << intervals << " in " << mConstraint << " mpOriginal: " << mpOriginal << std::endl;
                #endif

                #ifdef USE_HORNER
                splitOccurredInContraction = Operator<Polynomial>::contract(intervals, variable, mHornerForm, (*it).second, resA, resB, useNiceCenter);
                #else
                splitOccurredInContraction = Operator<Polynomial>::contract(intervals, variable, (mpOriginal == nullptr ? mConstraint : *mpOriginal), (*it).second, resA, resB, useNiceCenter);
                #endif
            }
            else
            {
				CARL_LOG_DEBUG("carl.contraction", "Trivial case...");
                resA = intervals.at(variable);
                resB = Interval<double>::empty_interval();
            }

            CARL_LOG_DEBUG("carl.contraction", "  after propagation: " << resA << " / " << resB);
			CARL_LOG_DEBUG("carl.contraction", "  Split? " << splitOccurredInContraction);

            if( usePropagation )
            {
                typename std::map<Variable, VarSolutionFormula<Polynomial>>::const_iterator const_iterator_VarSolutionFormula = mVarSolutionFormulas.find(variable);
                if( const_iterator_VarSolutionFormula == mVarSolutionFormulas.end() )
                {
                    const_iterator_VarSolutionFormula = mVarSolutionFormulas.emplace(variable, std::move(VarSolutionFormula<Polynomial>(mConstraint,variable))).first;
                }
                
                // calculate result of propagation
                std::vector<Interval<double>> resultPropagation = const_iterator_VarSolutionFormula->second.evaluate( intervals );
                
                #ifdef CONTRACTION_DEBUG
                std::cout << "  propagation result: " << resultPropagation << std::endl;
                #endif

                if( resultPropagation.empty() )
                {
                    resA = Interval<double>::empty_interval();
                    resB = Interval<double>::empty_interval();
					CARL_LOG_DEBUG("carl.contraction", "  after propagation: " << resA << " / " << resB);
                    return false;
                }

                // intersect with result of contraction
                std::vector<Interval<double>> resultingIntervals;
                if( splitOccurredInContraction )
                {   
                    Interval<double> tmp;
                    for( const auto& i : resultPropagation )
                    {
                        tmp = carl::set_intersection(i, resA);
						CARL_LOG_DEBUG("carl.contraction", "  intersection(" << i << ", " << resA << ") = " << tmp);
                        if( !tmp.is_empty() )
                            resultingIntervals.push_back(tmp);
                        tmp = carl::set_intersection(i, resB);
						CARL_LOG_DEBUG("carl.contraction", "  intersection(" << i << ", " << resB << ") = " << tmp);
                        if( !tmp.is_empty() )
                            resultingIntervals.push_back(tmp);
                    }
                }

                else 
                {
                    Interval<double> tmp;
                    for( const auto& i : resultPropagation )
                    {
                        tmp = carl::set_intersection(i, resA);
						CARL_LOG_DEBUG("carl.contraction", "  intersection(" << i << ", " << resA << ") = " << tmp);
                        if( !tmp.is_empty() )
                            resultingIntervals.push_back(tmp);
                    }
                }
                if( resultingIntervals.empty() )
                {
                    resA = Interval<double>::empty_interval();
                    resB = Interval<double>::empty_interval();
                    CARL_LOG_DEBUG("carl.contraction", "  after propagation: " << resA << " / " << resB);
                    return false;
                }
                if( resultingIntervals.size() == 1 )
                {
                    resA = resultingIntervals[0];
                    resB = Interval<double>::empty_interval();
                    CARL_LOG_DEBUG("carl.contraction", "  after propagation: " << resA << " / " << resB);
                    return false;
                }
                if( resultingIntervals.size() == 2 )
                {
                    if( resultingIntervals[0] < resultingIntervals[1] )
                    {
                        resA = resultingIntervals[0];
                        resB = resultingIntervals[1];
                    }
                    else
                    {
                        assert(resultingIntervals[1] < resultingIntervals[0]);
                        resA = resultingIntervals[1];
                        resB = resultingIntervals[0];
                    }
                    CARL_LOG_DEBUG("carl.contraction", "  after propagation: " << resA << " / " << resB);
                    return true;
                }
                else
                {
                    std::sort( resultPropagation.begin(), resultPropagation.end(), 
                              [](const Interval<double>& i,const Interval<double> j) 
                              { if(i<j){return true;} else { assert(j<i); return false; } }
                             );
                    auto intervalBeforeBiggestGap = resultPropagation.begin();
                    auto iter = resultPropagation.begin();
                    ++iter;
                    double bestDistance = intervalBeforeBiggestGap->distance(*iter);
                    for( ; iter != resultPropagation.end(); ++iter )
                    {
                        auto jter = iter;
                        ++jter;
                        if( jter == resultPropagation.end() )
                        {
                            break;
                        }
                        else
                        {
                            double distance = iter->distance(*jter);
                            if( bestDistance < distance )
                            {
                                bestDistance = distance;
                                intervalBeforeBiggestGap = iter;
                            }
                        }
                    }
                    resA = *intervalBeforeBiggestGap;
                    for( iter = resultPropagation.begin(); iter != intervalBeforeBiggestGap; ++iter )
                    {
                        resA = resA.convex_hull( *iter );
                    }
                    ++iter;
                    resB = *iter;
                    for( ; iter != resultPropagation.end(); ++iter )
                    {
                        resB = resB.convex_hull( *iter );
                    }
                    #ifdef CONTRACTION_DEBUG
                    std::cout << "  after propagation: " << resA; if( !resB.is_empty() ) { std::cout << " and " << resB; } std::cout << std::endl;                            
                    #endif
                    return true;
                }              
            }
            return splitOccurredInContraction;
        }
    };

    template<typename Polynomial>
    class SimpleNewton {
    public:
        
        template <typename evalType>
        bool contract(const Interval<double>::evalintervalmap& intervals, 
            Variable::Arg variable, 
            const evalType& constraint, 
            const evalType& derivative, 
            Interval<double>& resA, 
            Interval<double>& resB, 
            bool useNiceCenter = false) 
        {
            bool splitOccurred = false;
            
            double center = useNiceCenter ? carl::sample(intervals.at(variable)) : carl::center(intervals.at(variable));
            if( center == std::numeric_limits<double>::infinity() || center == -std::numeric_limits<double>::infinity() )
            {
                resA = intervals.at(variable);
                return false;
            }
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

            Interval<double> numerator (0);
            Interval<double> denominator(0);
            
     
            
            // Create Newton Operator
            numerator =   carl::evaluate(constraint, substitutedIntervalMap);
            denominator = carl::evaluate(derivative, intervals);


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
                    resA = carl::set_intersection(intervals.at(variable), centerInterval.sub(result1));
                    resB = carl::set_intersection(intervals.at(variable), centerInterval.sub(result2));
                }
                else
                {
                    resA = carl::set_intersection(intervals.at(variable), centerInterval.sub(result2));
                    resB = carl::set_intersection(intervals.at(variable), centerInterval.sub(result1));
                }
                if (variable.type() == VariableType::VT_INT) {
                    resA = resA.integral_part();
                    resB = resB.integral_part();
                }
                #ifdef CONTRACTION_DEBUG
                std::cout << __func__ << ": result after intersection: " << resA << " and " << resB << std::endl;
                #endif
                if( resB.is_empty() )
                {
                    splitOccurred = false;
                }
                else if( resA.is_empty() ) // resB is not empty at this state
                {
                    resA = resB;
                    resB = Interval<double>::empty_interval();
                    splitOccurred = false;
                }
                else
                {
                    Interval<double> tmpA, tmpB;
                    if (!carl::set_union(resA, resB, tmpA, tmpB)) {
                        resA = std::move(tmpA);
                        resB = Interval<double>::empty_interval();
                        splitOccurred = false;
                    }
                }
            } else {
				#ifdef CONTRACTION_DEBUG
				std::cout << __func__ << ": result: " << result1 << std::endl;
				std::cout << __func__ << ": center: " << centerInterval << std::endl;
				std::cout << __func__ << ": center sub: " << centerInterval.sub(result1) << std::endl;
				std::cout << __func__ << ": intersecting " << intervals.at(variable) << " and " << centerInterval.sub(result1) << std::endl;
				#endif
                resA = carl::set_intersection(intervals.at(variable), centerInterval.sub(result1));
				if (variable.type() == VariableType::VT_INT) {
					resA = resA.integral_part();
				}
				#ifdef CONTRACTION_DEBUG
				std::cout << __func__ << ": result after intersection: " << resA << std::endl;
				#endif
            }
            return splitOccurred;
        }
    };

    //typedef Contraction<SimpleNewton, Polynomial> SimpleNewtonContraction;

}

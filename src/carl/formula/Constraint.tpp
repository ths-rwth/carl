/**
 * Constraint.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2010-04-26
 * @version 2013-10-30
 */

#include "Constraint.h"
#ifdef USE_GINAC
#include "../converter/OldGinacConverter.h"
#endif
#include "ConstraintPool.h"

using namespace std;

namespace carl
{
    template<typename Pol, EnableIf<needs_cache<Pol>>>
    Pol makePolynomial( typename Pol::PolyType&& _poly )
    {
        return Pol( std::move(_poly), constraintPool<Pol>().pPolynomialCache() );
    }

    template<typename Pol, EnableIf<needs_cache<Pol>>>
    Pol makePolynomial( carl::Variable::Arg _var )
    {
        return Pol( std::move(typename Pol::PolyType(_var)), constraintPool<Pol>().pPolynomialCache() );
    }
    
    template<typename Pol>
    ConstraintContent<Pol>::ConstraintContent():
        mID( 0 ),
        mHash( CONSTRAINT_HASH( Pol( typename Pol::NumberType( 0 ) ), Relation::EQ, Pol ) ),
        mUsages( 0 ),
        mRelation( Relation::EQ ),
        mLhs( typename Pol::NumberType( 0 ) ),
        mFactorization(),
        mVariables(),
        mVarInfoMap(),
        mLhsDefinitess( Definiteness::NON )
    {}

    template<typename Pol>
    ConstraintContent<Pol>::ConstraintContent( Pol&& _lhs, Relation _rel, std::size_t _id ):
        mID( _id ),
        mHash( CONSTRAINT_HASH( _lhs, _rel, Pol ) ),
        mUsages( 0 ),
        mRelation( _rel ),
        mLhs( std::move(_lhs) ),
        mFactorization(),
        mVariables(),
        mVarInfoMap(),
        mLhsDefinitess( Definiteness::NON )
    {
        
        initLazy(); // TODO: this must unfortunately be done here and basically collects the variables. The reason is that without the variables, we cannot do the integer normalization.
        if( hasIntegerValuedVariable() && !hasRealValuedVariable() )
        {
            if( mRelation == Relation::LESS )
            {
                mLhs += carl::constant_one<typename Pol::CoeffType>::get();
                mRelation = Relation::LEQ;
                mHash = CONSTRAINT_HASH( mLhs, mRelation, Pol );
            }
            if( mRelation == Relation::GREATER )
            {
                mLhs -= carl::constant_one<typename Pol::CoeffType>::get();
                mRelation = Relation::GEQ;
                mHash = CONSTRAINT_HASH( mLhs, mRelation, Pol );
            }
        }
        mLhsDefinitess = mLhs.definiteness( FULL_EFFORT_FOR_DEFINITENESS_CHECK );
    }

    template<typename Pol>
    ConstraintContent<Pol>::~ConstraintContent()
    {}

    template<typename Pol>
    unsigned ConstraintContent<Pol>::isConsistent() const
    {
        if( mLhs.isConstant() )
            return carl::evaluate( mLhs.constantPart(), mRelation ) ? 1 : 0;
        else
        {
            switch( mRelation )
            {
                case Relation::EQ:
                {
                    if( mLhsDefinitess == Definiteness::POSITIVE || mLhsDefinitess == Definiteness::NEGATIVE ) return 0;
                    break;
                }
                case Relation::NEQ:
                {
                    if( mLhsDefinitess == Definiteness::POSITIVE || mLhsDefinitess == Definiteness::NEGATIVE ) return 1;
                    break;
                }
                case Relation::LESS:
                {
                    if( mLhsDefinitess == Definiteness::NEGATIVE ) return 1;
                    if( mLhsDefinitess >= Definiteness::POSITIVE_SEMI ) return 0;
                    break;
                }
                case Relation::GREATER:
                {
                    if( mLhsDefinitess == Definiteness::POSITIVE ) return 1;
                    if( mLhsDefinitess <= Definiteness::NEGATIVE_SEMI ) return 0;
                    break;
                }
                case Relation::LEQ:
                {
                    if( mLhsDefinitess <= Definiteness::NEGATIVE_SEMI ) return 1;
                    if( mLhsDefinitess == Definiteness::POSITIVE ) return 0;
                    break;
                }
                case Relation::GEQ:
                {
                    if( mLhsDefinitess >= Definiteness::POSITIVE_SEMI ) return 1;
                    if( mLhsDefinitess == Definiteness::NEGATIVE ) return 0;
                    break;
                }
                default:
                {
                    cout << "Error in isConsistent: unexpected relation symbol." << endl;
                    return false;
                }
            }
            return 2;
        }
    }
    
    template<typename Pol>
    ConstraintContent<Pol>* ConstraintContent<Pol>::simplify() const
    {
        typedef typename Pol::PolyType PolyT;
        Relation rel = mRelation;
        if( (mLhsDefinitess == Definiteness::POSITIVE_SEMI && rel == Relation::LEQ) || (mLhsDefinitess == Definiteness::NEGATIVE_SEMI && rel == Relation::GEQ) )
            rel = Relation::EQ;
        // Left-hand side is a non-linear univariate monomial
        if( mVariables.size() == 1 && !mLhs.isLinear() && mLhs.nrTerms() == 1 )
        {
            switch( rel )
            {
                case Relation::EQ:
                    return new ConstraintContent( *mVariables.begin(), rel );
                case Relation::NEQ:
                    return new ConstraintContent( *mVariables.begin(), rel );
                case Relation::LEQ:
                    if( mLhsDefinitess == Definiteness::NEGATIVE_SEMI )
                        return new ConstraintContent( PolyT( typename Pol::NumberType( -1 ) ) * PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel );
                    else
                        return new ConstraintContent( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel );
                case Relation::GEQ:
                    if( mLhsDefinitess == Definiteness::POSITIVE_SEMI )
                        return new ConstraintContent( PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel );
                    else
                        return new ConstraintContent( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel );
                case Relation::LESS:
                    if( mLhsDefinitess == Definiteness::NEGATIVE_SEMI )
                        return new ConstraintContent( *mVariables.begin(), Relation::NEQ );
                    else
                    {
                        if( mLhsDefinitess == Definiteness::POSITIVE_SEMI )
                            return new ConstraintContent( PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel );
                        else
                            return new ConstraintContent( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel );
                    }
                case Relation::GREATER:
                    if( mLhsDefinitess == Definiteness::POSITIVE_SEMI )
                        return new ConstraintContent( *mVariables.begin(), Relation::NEQ );
                    else
                    {
                        if( mLhsDefinitess == Definiteness::NEGATIVE_SEMI )
                            return new ConstraintContent( PolyT( typename Pol::NumberType( -1 ) ) * PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel ); 
                        else
                            return new ConstraintContent( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel ); 
                    }
                default:
                    assert( false );
            }
        }
        else if( hasIntegerValuedVariable() && !hasRealValuedVariable() && !mLhs.isConstant() )
        {
            typename Pol::NumberType constPart = mLhs.constantPart();
            if( constPart != typename Pol::NumberType( 0 ) )
            {
                // Find the gcd of the coefficients of the non-constant terms.
                typename Pol::NumberType g = carl::abs( mLhs.coprimeFactorWithoutConstant() );
                assert( g != typename Pol::NumberType( 0 ) );
                if( carl::mod( carl::getNum( constPart ), carl::getDenom( g ) ) != 0 )
                {
                    switch( mRelation )
                    {
                        case Relation::EQ:
                            return new ConstraintContent( Pol( typename Pol::NumberType( 0 ) ), Relation::LESS );
                        case Relation::NEQ:
                            return new ConstraintContent( Pol( typename Pol::NumberType( 0 ) ), Relation::EQ );
                        case Relation::LEQ:
                        {
                            Pol newLhs = ((mLhs - constPart) * g);
                            newLhs += carl::floor( (constPart * g) ) + typename Pol::NumberType( 1 );
                            return new ConstraintContent( newLhs, Relation::LEQ );
                        }
                        case Relation::GEQ:
                        {
                            Pol newLhs = ((mLhs - constPart) * g);
                            newLhs += carl::floor( (constPart * g) );
                            return new ConstraintContent( newLhs, Relation::GEQ );
                        }
                        case Relation::LESS:
                        {
                            Pol newLhs = ((mLhs - constPart) * g);
                            newLhs += carl::floor( (constPart * g) ) + typename Pol::NumberType( 1 );
                            return new ConstraintContent( newLhs, Relation::LEQ );
                        }
                        case Relation::GREATER:
                        {
                            Pol newLhs = ((mLhs - constPart) * g);
                            newLhs += carl::floor( (constPart * g) );
                            return new ConstraintContent( newLhs, Relation::GEQ );
                        }
                        default:
                            assert( false );
                    }
                }
            }
        }
        return nullptr;
    }

    template<typename Pol>
    void ConstraintContent<Pol>::initLazy()
    {
        mLhs.gatherVariables( mVariables );
    }

    template<typename Pol>
    void ConstraintContent<Pol>::initEager()
    {
        initVariableInformations();
    }
    
    template<typename Pol>
    void ConstraintContent<Pol>::initFactorization() const 
    {
        #ifdef CONSTRAINT_WITH_FACTORIZATION
        #ifdef USE_GINAC
        if( !mLhs.isLinear() && mLhs.nrTerms() <= MAX_NUMBER_OF_MONOMIALS_FOR_FACTORIZATION && mVariables.size() <= MAX_DIMENSION_FOR_FACTORIZATION
            && maxDegree() <= MAX_DEGREE_FOR_FACTORIZATION && maxDegree() >= MIN_DEGREE_FOR_FACTORIZATION )
        {
            mFactorization = carl::factor( mLhs );
        } else {
			mFactorization.emplace( mLhs, 1 );
		}
        #else
        mFactorization.emplace( mLhs, 1 );
        #endif
        #else
        mFactorization.emplace( mLhs, 1 );
        #endif
    }

    template<typename Pol>
    bool ConstraintContent<Pol>::operator==( const ConstraintContent& _constraint ) const
    {
        if( mID == 0 || _constraint.mID == 0 )
        {
            return mRelation == _constraint.mRelation && mLhs == _constraint.mLhs;
        }
        return mID == _constraint.mID;
    }

    template<typename Pol>
    string ConstraintContent<Pol>::toString( unsigned _unequalSwitch, bool _infix, bool _friendlyVarNames ) const
    {
        string result = "";
        if( _infix )
            result = mLhs.toString( true, _friendlyVarNames );
        else
            result += "(";
        switch( mRelation )
        {
            case Relation::EQ:
                result += "=";
                break;
            case Relation::NEQ:
                if( _infix )
                {
                    if( _unequalSwitch == 1 )
                        result += "<>";
                    else if( _unequalSwitch == 2 )
                        result += "/=";
                    else // standard case
                        result += "!=";
                }
                else
                {
                    if( _unequalSwitch == 0 ) // standard case
                        result += "!=";
                    else if( _unequalSwitch == 1 )
                    {
                        string lhsString = mLhs.toString( false, _friendlyVarNames );
                        return "(or (< " + lhsString + " 0) (> " + lhsString + " 0))";
                    }
                    else
                    {
                        string lhsString = mLhs.toString( false, _friendlyVarNames );
                        return "(not (= " + lhsString + " 0))";
                    }
                }
                break;
            case Relation::LESS:
                result += "<";
                break;
            case Relation::GREATER:
                result += ">";
                break;
            case Relation::LEQ:
                result += "<=";
                break;
            case Relation::GEQ:
                result += ">=";
                break;
            default:
                result += "~";
        }
        result += (_infix ? "0" : (" " + mLhs.toString( false, _friendlyVarNames ) + " 0)"));
        return result;
    }
    
    template<typename Pol>
    Constraint<Pol>::Constraint( const ConstraintContent<Pol>* _content ):
        mpContent( _content )
    {
        ConstraintPool<Pol>::getInstance().reg( mpContent );
    }
    
    template<typename Pol>
    Constraint<Pol>::Constraint( bool _valid ):
        Constraint( ConstraintPool<Pol>::getInstance().create( _valid ) )
    {}
    
    template<typename Pol>
    Constraint<Pol>::Constraint( carl::Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound ):
        Constraint( ConstraintPool<Pol>::getInstance().create( _var, _rel, _bound ) )
    {}
            
    template<typename Pol>
    Constraint<Pol>::Constraint( const Pol& _lhs, Relation _rel ):
        Constraint( ConstraintPool<Pol>::getInstance().create( _lhs, _rel ) )
    {}

    template<typename Pol>
    template<typename P, EnableIf<needs_cache<P>>>
    Constraint<Pol>::Constraint( const typename P::PolyType& _lhs, Relation _rel ):
        Constraint( ConstraintPool<Pol>::getInstance().create( _lhs, _rel ) )
    {}
    
    template<typename Pol>
    Constraint<Pol>::Constraint( const Constraint<Pol>& _constraint ):
        mpContent( _constraint.mpContent )
    {
        ConstraintPool<Pol>::getInstance().reg( mpContent );
    }
    
    template<typename Pol>
    Constraint<Pol>::Constraint( Constraint<Pol>&& _constraint ) noexcept:
        mpContent( _constraint.mpContent )
    {
        _constraint.mpContent = nullptr;
    }
            
    template<typename Pol>
    Constraint<Pol>::~Constraint()
    {
        if( mpContent != nullptr )
        {
            ConstraintPool<Pol>::getInstance().free( mpContent );
        }
    }
    
    template<typename Pol>
    Constraint<Pol>& Constraint<Pol>::operator=( const Constraint<Pol>& _constraint )
    {
        ConstraintPool<Pol>::getInstance().reg( _constraint.mpContent );
        if( mpContent != nullptr ) 
            ConstraintPool<Pol>::getInstance().free( mpContent );
        mpContent = _constraint.mpContent;
        return *this;
    }
    
    template<typename Pol>
    Constraint<Pol>& Constraint<Pol>::operator=( Constraint<Pol>&& _constraint ) noexcept
    {
        if( mpContent != nullptr ) 
            ConstraintPool<Pol>::getInstance().free( mpContent );
        mpContent = _constraint.mpContent;
        _constraint.mpContent = nullptr;
        return *this;
    }
    
    template<typename Pol>
    unsigned Constraint<Pol>::satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const
    {
//        std::cout << "Is  " << this->toString( 0, true, true ) << std::endl;
//        std::cout << "satisfied by  " << std::endl;
//        for( auto iter = _assignment.begin(); iter != _assignment.end(); ++iter )
//            std::cout << iter->first << " in " << iter->second << std::endl;
        unsigned result = 2;
        Pol tmp = mpContent->mLhs.substitute( _assignment );
        if( tmp.isConstant() )
        {
            result = carl::evaluate( (tmp.isZero() ? typename Pol::NumberType( 0 ) : tmp.trailingTerm().coeff()), relation() ) ? 1 : 0;
        }
//        std::cout << "result is " << result << std::endl;
//        std::cout << std::endl;
        return result;
    }
    
    template<typename Pol>
    unsigned Constraint<Pol>::consistentWith( const EvaluationMap<Interval<double>>& _solutionInterval ) const
    {
        if( variables().empty() )
            return carl::evaluate( constantPart(), relation() ) ? 1 : 0;
        else
        {
            auto varIter = variables().begin();
            auto varIntervalIter = _solutionInterval.begin();
            while( varIter != variables().end() && varIntervalIter != _solutionInterval.end() )
            {
                if( *varIter < varIntervalIter->first )
                {
                    return 2;
                }
                else if( *varIter > varIntervalIter->first )
                {
                    ++varIntervalIter;
                }
                else
                {
                    ++varIter;
                    ++varIntervalIter;
                }
            }
            if( varIter != variables().end() )
                return 2;
            Interval<double> solutionSpace = IntervalEvaluation::evaluate( lhs(), _solutionInterval );
            if( solutionSpace.isEmpty() )
                return 2;
            switch( relation() )
            {
                case Relation::EQ:
                {
                    if( solutionSpace.isZero() )
                        return 1;
                    else if( !solutionSpace.contains( 0 ) )
                        return 0;
                    break;
                }
                case Relation::NEQ:
                {
                    if( !solutionSpace.contains( 0 ) )
                        return 1;
                    break;
                }
                case Relation::LESS:
                {
                    if( solutionSpace.upperBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.upper() < 0 )
                            return 1;
                        else if( solutionSpace.upper() == 0 && solutionSpace.upperBoundType() == BoundType::STRICT )
                            return 1;
                    }
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY && solutionSpace.lower() >= 0 )
                        return 0;
                    break;
                }
                case Relation::GREATER:
                {
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.lower() > 0 )
                            return 1;
                        else if( solutionSpace.lower() == 0 && solutionSpace.lowerBoundType() == BoundType::STRICT )
                            return 1;
                    }
                    if( solutionSpace.upperBoundType() != BoundType::INFTY && solutionSpace.upper() <= 0 )
                        return 0;
                    break;
                }
                case Relation::LEQ:
                {
                    if( solutionSpace.upperBoundType() != BoundType::INFTY && solutionSpace.upper() <= 0)
                        return 1;
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.lower() > 0 )
                            return 0;
                        else if( solutionSpace.lower() == 0 && solutionSpace.lowerBoundType() == BoundType::STRICT )
                            return 0;
                    }
                    break;
                }
                case Relation::GEQ:
                {
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY && solutionSpace.lower() >= 0 )
                        return 1;
                    if( solutionSpace.upperBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.upper() < 0 )
                            return 0;
                        else if( solutionSpace.upper() == 0 && solutionSpace.upperBoundType() == BoundType::STRICT )
                            return 0;
                    }
                    break;
                }
                default:
                {
                    cout << "Error in isConsistent: unexpected relation symbol." << endl;
                    return 0;
                }
            }
            return 2;
        }
    }
    
    template<typename Pol>
    unsigned Constraint<Pol>::consistentWith( const EvaluationMap<Interval<double>>& _solutionInterval, Relation& _stricterRelation ) const
    {
        _stricterRelation = relation();
        if( variables().empty() )
            return carl::evaluate( constantPart(), relation() ) ? 1 : 0;
        else
        {
            auto varIter = variables().begin();
            auto varIntervalIter = _solutionInterval.begin();
            while( varIter != variables().end() && varIntervalIter != _solutionInterval.end() )
            {
                if( *varIter < varIntervalIter->first )
                {
                    return 2;
                }
                else if( *varIter > varIntervalIter->first )
                {
                    ++varIntervalIter;
                }
                else
                {
                    ++varIter;
                    ++varIntervalIter;
                }
            }
            if( varIter != variables().end() )
                return 2;
            Interval<double> solutionSpace = IntervalEvaluation::evaluate( lhs(), _solutionInterval );
            if( solutionSpace.isEmpty() )
                return 2;
            switch( relation() )
            {
                case Relation::EQ:
                {
                    if( solutionSpace.isZero() )
                        return 1;
                    else if( !solutionSpace.contains( 0 ) )
                        return 0;
                    break;
                }
                case Relation::NEQ:
                {
                    if( !solutionSpace.contains( 0 ) )
                        return 1;
                    if( solutionSpace.upperBoundType() == BoundType::WEAK && solutionSpace.upper() == 0 )
                    {
                        _stricterRelation = Relation::LESS;
                    }
                    else if( solutionSpace.lowerBoundType() == BoundType::WEAK && solutionSpace.lower() == 0 )
                    {
                        _stricterRelation = Relation::GREATER;
                    }
                    break;
                }
                case Relation::LESS:
                {
                    if( solutionSpace.upperBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.upper() < 0 )
                            return 1;
                        else if( solutionSpace.upper() == 0 && solutionSpace.upperBoundType() == BoundType::STRICT )
                            return 1;
                    }
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY && solutionSpace.lower() >= 0 )
                        return 0;
                    break;
                }
                case Relation::GREATER:
                {
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.lower() > 0 )
                            return 1;
                        else if( solutionSpace.lower() == 0 && solutionSpace.lowerBoundType() == BoundType::STRICT )
                            return 1;
                    }
                    if( solutionSpace.upperBoundType() != BoundType::INFTY && solutionSpace.upper() <= 0 )
                        return 0;
                    break;
                }
                case Relation::LEQ:
                {
                    if( solutionSpace.upperBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.upper() <= 0)
                        {
                            return 1;
                        }
                    }
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.lower() > 0 )
                        {
                            return 0;
                        }
                        else if( solutionSpace.lower() == 0 )
                        {
                            if( solutionSpace.lowerBoundType() == BoundType::STRICT )
                            {
                                return 0;
                            }
                            else
                            {
                                _stricterRelation = Relation::EQ;
                            }
                        }
                    }
                    break;
                }
                case Relation::GEQ:
                {
                    if( solutionSpace.lowerBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.lower() >= 0 )
                            return 1;
                    }
                    if( solutionSpace.upperBoundType() != BoundType::INFTY )
                    {
                        if( solutionSpace.upper() < 0 )
                            return 0;
                        else if( solutionSpace.upper() == 0 )
                        {
                            if( solutionSpace.upperBoundType() == BoundType::STRICT )
                                return 0;
                            else
                                _stricterRelation = Relation::EQ;
                        }
                    }
                    break;
                }
                default:
                {
                    cout << "Error in isConsistent: unexpected relation symbol." << endl;
                    return 0;
                }
            }
            return 2;
        }
    }

	template<typename Pol>
	unsigned Constraint<Pol>::evaluate(const EvaluationMap<Interval<typename carl::UnderlyingNumberType<Pol>::type>>& _assignment) const {
		// 0 = False, 1 = Null, 2 = Maybe, 3 = True
		Interval<typename carl::UnderlyingNumberType<Pol>::type> res = IntervalEvaluation::evaluate(lhs(), _assignment);
		switch (relation()) {
			case Relation::EQ: {
				if (res.isZero()) return 3;
				else if (res.contains(0)) return 2;
				else return 0;
			}
			case Relation::NEQ: {
				if (res.isZero()) return 0;
				else if (res.contains(0)) return 2;
				else return 3;
			}
			case Relation::LESS: {
				if (res.isNegative()) return 3;
				else if (res.isSemiPositive()) return 0;
				else return 2;
			}
			case Relation::GREATER: {
				if (res.isPositive()) return 3;
				else if (res.isSemiNegative()) return 0;
				else return 2;
			}
			case Relation::LEQ: {
				if (res.isSemiNegative()) return 3;
				else if (res.isPositive()) return 0;
				else if (res.isSemiPositive()) return 1;
				else return 2;
			}
			case Relation::GEQ: {
				if (res.isSemiPositive()) return 3;
				else if (res.isNegative()) return 0;
				else if (res.isSemiNegative()) return 1;
				else return 2;
			}
		}
		assert(false);
		return 1;
	}
    
    template<typename Pol>
    bool Constraint<Pol>::hasFinitelyManySolutionsIn( const Variable& _var ) const
    {
        if( variables().find( _var ) == variables().end() )
            return true;
        if( relation() == Relation::EQ )
        {
            if( variables().size() == 1 )
                return true;
            //TODO: else, if not too expensive (construct constraints being the side conditions)
        }
        return false;
    }

    template<typename Pol>
    Pol Constraint<Pol>::coefficient( const Variable& _var, uint _degree ) const
    {
        VARINFOMAP_LOCK_GUARD
        auto varInfo = mpContent->mVarInfoMap.find( _var );
        assert( varInfo != mpContent->mVarInfoMap.end() );
        if( !varInfo->second.hasCoeff() )
        {
            varInfo->second = lhs().template getVarInfo<true>( _var );
        }
        auto d = varInfo->second.coeffs().find( _degree );
        return d != varInfo->second.coeffs().end() ? d->second : Pol( typename Pol::NumberType( 0 ) );
    }

    template<typename Pol>
    bool Constraint<Pol>::getSubstitution( Variable& _substitutionVariable, Pol& _substitutionTerm, bool _negated, const Variable& _exclude ) const
    {
        if( (!_negated && relation() != Relation::EQ) || (_negated && relation() != Relation::NEQ) )
            return false;
        VARINFOMAP_LOCK_GUARD
        for( typename map<Variable, VarInfo<Pol>>::iterator varInfoPair = mpContent->mVarInfoMap.begin(); varInfoPair != mpContent->mVarInfoMap.end(); ++varInfoPair )
        {
			if (varInfoPair->first == _exclude) continue;
            if( varInfoPair->second.maxDegree() == 1 )
            {
                if( !varInfoPair->second.hasCoeff() )
                {
                    varInfoPair->second = lhs().template getVarInfo<true>( varInfoPair->first );
                }
                auto d = varInfoPair->second.coeffs().find( 1 );
                assert( d != varInfoPair->second.coeffs().end() );
                if( d->second.isConstant() && (varInfoPair->first.getType() != carl::VariableType::VT_INT || carl::isOne(carl::abs( d->second.constantPart() ))) )
                {
                    _substitutionVariable = varInfoPair->first;
                    _substitutionTerm = makePolynomial<Pol>( _substitutionVariable ) * d->second - lhs();
                    _substitutionTerm /= d->second.constantPart();
                    return true;
                }
            }
        }
        return false;
    }
	
	template<typename Pol>
    bool Constraint<Pol>::getAssignment(Variable& _substitutionVariable, typename Pol::NumberType& _substitutionValue) const {
		if (relation() != Relation::EQ) return false;
		if (lhs().nrTerms() > 2) return false;
		if (lhs().nrTerms() == 2 && !lhs().trailingTerm().isConstant()) return false;
		if (!lhs().lterm().isSingleVariable()) return false;
		_substitutionVariable = lhs().lterm().getSingleVariable();
		_substitutionValue = -lhs().constantPart() / lhs().lterm().coeff();
		return true;
	}
    
    template<typename Pol>
    ostream& operator<<( ostream& _out, const Constraint<Pol>& _constraint )
    {
        return (_out << _constraint.toString());
    }

    template<typename Pol>
    void Constraint<Pol>::printProperties( ostream& _out ) const
    {
        _out << "Properties:" << endl;
        _out << "   Definitess:              ";
        switch( mpContent->mLhsDefinitess )
        {
            case Definiteness::NON:
                _out << "NON" << endl;
                break;
            case Definiteness::POSITIVE:
                _out << "POSITIVE" << endl;
                break;
            case Definiteness::POSITIVE_SEMI:
                _out << "POSITIVE_SEMI" << endl;
                break;
            case Definiteness::NEGATIVE:
                _out << "NEGATIVE" << endl;
                break;
            case Definiteness::NEGATIVE_SEMI:
                _out << "NEGATIVE_SEMI" << endl;
                break;
            default:
                _out << "UNDEFINED" << endl;
                break;
        }
        _out << "   The number of monomials: " << lhs().nrTerms() << endl;
        _out << "   The maximal degree:      " << (lhs().isZero() ? 0 : lhs().totalDegree()) << endl;
        _out << "   The constant part:       " << constantPart() << endl;
        _out << "   Variables:" << endl;
        for( auto vi = mpContent->mVarInfoMap.begin(); vi != mpContent->mVarInfoMap.end(); ++vi )
        {
            _out << "        " << vi->first << " has " << vi->second.occurence() << " occurences." << endl;
            _out << "        " << vi->first << " has the maximal degree of " << vi->second.maxDegree() << "." << endl;
            _out << "        " << vi->first << " has the minimal degree of " << vi->second.minDegree() << "." << endl;
        }
    }
}    // namespace carl

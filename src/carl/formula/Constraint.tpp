/**
 * Constraint.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2010-04-26
 * @version 2013-10-30
 */

#include "Constraint.h"
#ifdef COMPARE_WITH_GINAC
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
    Pol makePolynomial( const carl::Variable::Arg _var )
    {
        return Pol( std::move(typename Pol::PolyType(_var)), constraintPool<Pol>().pPolynomialCache() );
    }
    
    template<typename Pol>
    Constraint<Pol>::Constraint():
        mID( 0 ),
        mHash( CONSTRAINT_HASH( Pol( typename Pol::NumberType( 0 ) ), Relation::EQ, Pol ) ), 
        mRelation( Relation::EQ ),
        mLhs( typename Pol::NumberType( 0 ) ),
        mFactorization(),
        mVariables(),
        mVarInfoMap(),
        mLhsDefinitess( Definiteness::NON )
    {}

    template<typename Pol>
    Constraint<Pol>::Constraint( Pol&& _lhs, Relation _rel, unsigned _id ):
        mID( _id ),
        mHash( CONSTRAINT_HASH( _lhs, _rel, Pol ) ),
        mRelation( _rel ),
        mLhs( std::move(_lhs) ),
        mFactorization(),
        mVariables(),
        mVarInfoMap(),
        mLhsDefinitess( Definiteness::NON )
    {
        mLhs.gatherVariables( mVariables );
        
        if( hasIntegerValuedVariable() && !hasRealValuedVariable() )
        {
            if( relation() == Relation::LESS )
            {
                mLhs += typename Pol::NumberType( 1 );
                mRelation = Relation::LEQ;
                mHash = CONSTRAINT_HASH( mLhs, mRelation, Pol );
            }
            if( relation() == Relation::GREATER )
            {
                mLhs -= typename Pol::NumberType( 1 );
                mRelation = Relation::GEQ;
                mHash = CONSTRAINT_HASH( mLhs, mRelation, Pol );
            }
        }
        initVariableInformations();
        mLhsDefinitess = mLhs.definiteness();
    }

    template<typename Pol>
    Constraint<Pol>::Constraint( const Constraint& _constraint, bool _rehash ):
        mID( _constraint.id() ),
        mHash( _rehash ? CONSTRAINT_HASH( _constraint.lhs(), _constraint.relation(), Pol ) : _constraint.getHash() ),
        mRelation( _constraint.relation() ),
        mLhs( _constraint.mLhs ),
        mFactorization( _constraint.mFactorization ),
        mVariables( _constraint.variables() ),
        mVarInfoMap( _constraint.mVarInfoMap ),
        mLhsDefinitess( _constraint.mLhsDefinitess )
    {}

    template<typename Pol>
    Constraint<Pol>::~Constraint()
    {}
    
    template<typename Pol>
    unsigned Constraint<Pol>::satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const
    {
//        std::cout << "Is  " << this->toString( 0, true, true ) << std::endl;
//        std::cout << "satisfied by  " << std::endl;
//        for( auto iter = _assignment.begin(); iter != _assignment.end(); ++iter )
//            std::cout << iter->first << " in " << iter->second << std::endl;
        unsigned result = 2;
        Pol tmp = mLhs.substitute( _assignment );
        if( tmp.isConstant() )
            result = evaluate( (tmp.isZero() ? typename Pol::NumberType( 0 ) : tmp.trailingTerm().coeff()), relation() ) ? 1 : 0;
//        std::cout << "result is " << result << std::endl;
//        std::cout << std::endl;
        return result;
    }

    template<typename Pol>
    unsigned Constraint<Pol>::isConsistent() const
    {
        if( variables().empty() )
            return evaluate( constantPart(), relation() ) ? 1 : 0;
        else
        {
            switch( relation() )
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
    unsigned Constraint<Pol>::consistentWith( const EvaluationMap<Interval<double>>& _solutionInterval ) const
    {
        if( variables().empty() )
            return evaluate( constantPart(), relation() ) ? 1 : 0;
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
            Interval<double> solutionSpace = IntervalEvaluation::evaluate( mLhs, _solutionInterval );
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
        _stricterRelation = mRelation;
        if( variables().empty() )
            return evaluate( constantPart(), relation() ) ? 1 : 0;
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
            Interval<double> solutionSpace = IntervalEvaluation::evaluate( mLhs, _solutionInterval );
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
		Interval<typename carl::UnderlyingNumberType<Pol>::type> res = IntervalEvaluation::evaluate(mLhs, _assignment);
		switch (mRelation) {
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
    Pol Constraint<Pol>::coefficient( const Variable& _var, unsigned _degree ) const
    {
        auto varInfo = mVarInfoMap.find( _var );
        assert( varInfo != mVarInfoMap.end() );
        if( !varInfo->second.hasCoeff() )
        {
            varInfo->second = mLhs.template getVarInfo<true>( _var );
        }
        auto d = varInfo->second.coeffs().find( _degree );
        return d != varInfo->second.coeffs().end() ? d->second : Pol( typename Pol::NumberType( 0 ) );
    }

    template<typename Pol>
    bool Constraint<Pol>::getSubstitution( Variable& _substitutionVariable, Pol& _substitutionTerm ) const
    {
        if( mRelation != Relation::EQ )
            return false;
        for( typename map<Variable, VarInfo<Pol>>::iterator varInfoPair = mVarInfoMap.begin(); varInfoPair != mVarInfoMap.end(); ++varInfoPair )
        {
            if( varInfoPair->second.maxDegree() == 1 )
            {
                if( !varInfoPair->second.hasCoeff() )
                {
                    varInfoPair->second = mLhs.template getVarInfo<true>( varInfoPair->first );
                }
                auto d = varInfoPair->second.coeffs().find( 1 );
                assert( d != varInfoPair->second.coeffs().end() );
                _substitutionVariable = varInfoPair->first;
                _substitutionTerm = makePolynomial<Pol>( _substitutionVariable ) * d->second - mLhs;
                _substitutionTerm /= d->second.constantPart();
                return true;
            }
        }
        return false;
    }
    
    template<typename Pol>
    Constraint<Pol>* Constraint<Pol>::simplify() const
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
                    return new Constraint( *mVariables.begin(), rel );
                case Relation::NEQ:
                    return new Constraint( *mVariables.begin(), rel );
                case Relation::LEQ:
                    if( mLhsDefinitess == Definiteness::NEGATIVE_SEMI )
                        return new Constraint( PolyT( typename Pol::NumberType( -1 ) ) * PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel );
                    else
                        return new Constraint( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel );
                case Relation::GEQ:
                    if( mLhsDefinitess == Definiteness::POSITIVE_SEMI )
                        return new Constraint( PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel );
                    else
                        return new Constraint( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel );
                case Relation::LESS:
                    if( mLhsDefinitess == Definiteness::NEGATIVE_SEMI )
                        return new Constraint( *mVariables.begin(), Relation::NEQ );
                    else
                    {
                        if( mLhsDefinitess == Definiteness::POSITIVE_SEMI )
                            return new Constraint( PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel );
                        else
                            return new Constraint( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel );
                    }
                case Relation::GREATER:
                    if( mLhsDefinitess == Definiteness::POSITIVE_SEMI )
                        return new Constraint( *mVariables.begin(), Relation::NEQ );
                    else
                    {
                        if( mLhsDefinitess == Definiteness::NEGATIVE_SEMI )
                            return new Constraint( PolyT( typename Pol::NumberType( -1 ) ) * PolyT( *mVariables.begin() ) * PolyT( *mVariables.begin() ), rel ); 
                        else
                            return new Constraint( (mLhs.trailingTerm().coeff() > 0 ? PolyT( typename Pol::NumberType( 1 ) ) : PolyT( typename Pol::NumberType( -1 ) ) ) * PolyT( *mVariables.begin() ), rel ); 
                    }
                default:
                    assert( false );
            }
        }
        else if( hasIntegerValuedVariable() && !hasRealValuedVariable() && !lhs().isConstant() )
        {
            typename Pol::NumberType constPart = lhs().constantPart();
            if( constPart != typename Pol::NumberType( 0 ) )
            {
                // Find the gcd of the coefficients of the non-constant terms.
                typename Pol::NumberType g = lhs().coprimeFactorWithoutConstant();
                assert( g > typename Pol::NumberType( 0 ) );
                if( carl::mod( carl::getNum( constPart ), carl::getDenom( g ) ) != 0 )
                {
                    switch( relation() )
                    {
                        case Relation::EQ:
                            return new Constraint( Pol( typename Pol::NumberType( 0 ) ), Relation::LESS );
                        case Relation::NEQ:
                            return new Constraint( Pol( typename Pol::NumberType( 0 ) ), Relation::EQ );
                        case Relation::LEQ:
                        {
                            Pol newLhs = ((lhs() - constPart) * (typename Pol::NumberType( 1 ) / g));
                            newLhs += carl::floor( (constPart / g) ) + typename Pol::NumberType( 1 );
                            return new Constraint( newLhs, Relation::LEQ );
                        }
                        case Relation::GEQ:
                        {
                            Pol newLhs = ((lhs() - constPart) * (typename Pol::NumberType( 1 ) / g));
                            newLhs += carl::floor( (constPart / g) );
                            return new Constraint( newLhs, Relation::GEQ );
                        }
                        case Relation::LESS:
                        {
                            Pol newLhs = ((lhs() - constPart) * (typename Pol::NumberType( 1 ) / g));
                            newLhs += carl::floor( (constPart / g) ) + typename Pol::NumberType( 1 );
                            return new Constraint( newLhs, Relation::LEQ );
                        }
                        case Relation::GREATER:
                        {
                            Pol newLhs = ((lhs() - constPart) * (typename Pol::NumberType( 1 ) / g));
                            newLhs += carl::floor( (constPart / g) );
                            return new Constraint( newLhs, Relation::GEQ );
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
    void Constraint<Pol>::init()
    {
        if( hasIntegerValuedVariable() && !hasRealValuedVariable() )
        {
            if( relation() == Relation::LESS )
            {
                mLhs += carl::constant_one<typename Pol::CoeffType>::get();
                mRelation = Relation::LEQ;
                mHash = CONSTRAINT_HASH( mLhs, mRelation, Pol );
            }
            if( relation() == Relation::GREATER )
            {
                mLhs -= carl::constant_one<typename Pol::CoeffType>::get();
                mRelation = Relation::GEQ;
                mHash = CONSTRAINT_HASH( mLhs, mRelation, Pol );
            }
        }
        initVariableInformations();
        mLhsDefinitess = mLhs.definiteness();
    }
    
    template<typename Pol>
    void Constraint<Pol>::initFactorization() const 
    {
        #ifdef CONSTRAINT_WITH_FACTORIZATION
        #ifdef COMPARE_WITH_GINAC
        if( lhs().nrTerms() <= MAX_NUMBER_OF_MONOMIALS_FOR_FACTORIZATION && mVariables.size() <= MAX_DIMENSION_FOR_FACTORIZATION
            && maxDegree() <= MAX_DEGREE_FOR_FACTORIZATION && maxDegree() >= MIN_DEGREE_FOR_FACTORIZATION )
        {
            mFactorization = ginacFactorization( mLhs );
        } else {
			mFactorization.insert( pair<Pol, unsigned>( mLhs, 1 ) );
		}
        #else
        mFactorization.insert( pair<Pol, unsigned>( mLhs, 1 ) );
        #endif
        #else
        mFactorization.insert( pair<Pol, unsigned>( mLhs, 1 ) );
        #endif
    }

    template<typename Pol>
    bool Constraint<Pol>::operator<( const Constraint& _constraint ) const
    {
        assert( mID > 0 && _constraint.id() > 0 );
        return mID < _constraint.id();
    }

    template<typename Pol>
    bool Constraint<Pol>::operator==( const Constraint& _constraint ) const
    {
        if( mID == 0 || _constraint.id() == 0 )
        {
            return relation() == _constraint.relation() && lhs() == _constraint.lhs();
        }
        return mID == _constraint.id();
    }

    template<typename Pol>
    ostream& operator<<( ostream& _out, const Constraint<Pol>& _constraint )
    {
        _out << _constraint.toString();
        return _out;
    }

    template<typename Pol>
    string Constraint<Pol>::toString( unsigned _unequalSwitch, bool _infix, bool _friendlyVarNames ) const
    {
        string result = "";
        if( _infix )
            result = mLhs.toString( true, _friendlyVarNames );
        else
            result += "(";
        switch( relation() )
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
    void Constraint<Pol>::printProperties( ostream& _out ) const
    {
        _out << "Properties:" << endl;
        _out << "   Definitess:              ";
        switch( mLhsDefinitess )
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
        _out << "   The number of monomials: " << mLhs.nrTerms() << endl;
        _out << "   The maximal degree:      " << (mLhs.isZero() ? 0 : mLhs.totalDegree()) << endl;
        _out << "   The constant part:       " << constantPart() << endl;
        _out << "   Variables:" << endl;
        for( auto vi = mVarInfoMap.begin(); vi != mVarInfoMap.end(); ++vi )
        {
            _out << "        " << vi->first << " has " << vi->second.occurence() << " occurences." << endl;
            _out << "        " << vi->first << " has the maximal degree of " << vi->second.maxDegree() << "." << endl;
            _out << "        " << vi->first << " has the minimal degree of " << vi->second.minDegree() << "." << endl;
        }
    }
    
    template<typename Pol>
    std::string Constraint<Pol>::relationToString( const Relation _rel )
    {
        switch( _rel )
        {
            case Relation::EQ:
                return "=";
            case Relation::NEQ:
                return "!=";
            case Relation::LEQ:
                return "<=";
            case Relation::GEQ:
                return ">=";
            case Relation::LESS:
                return "<";
            case Relation::GREATER:
                return ">";
            default:
                return "~";
        }
    }
    
    template<typename Pol>
    bool Constraint<Pol>::evaluate( const typename Pol::NumberType& _value, Relation _relation )
    {
        switch( _relation )
        {
            case Relation::EQ:
            {
                if( _value == 0 ) return true;
                else return false;
            }
            case Relation::NEQ:
            {
                if( _value == 0 ) return false;
                else return true;
            }
            case Relation::LESS:
            {
                if( _value < 0 ) return true;
                else return false;
            }
            case Relation::GREATER:
            {
                if( _value > 0 ) return true;
                else return false;
            }
            case Relation::LEQ:
            {
                if( _value <= 0 ) return true;
                else return false;
            }
            case Relation::GEQ:
            {
                if( _value >= 0 ) return true;
                else return false;
            }
            default:
            {
                cout << "Error in isConsistent: unexpected relation symbol." << endl;
                return false;
            }
        }
    }
    
    template<typename Pol>
    Relation Constraint<Pol>::invertRelation( const Relation _rel )
    {
        switch( _rel )
        {
            case Relation::EQ:
                return Relation::NEQ;
            case Relation::NEQ:
                return Relation::EQ;
            case Relation::LEQ:
                return Relation::GREATER;
            case Relation::GEQ:
                return Relation::LESS;
            case Relation::LESS:
                return Relation::GEQ;
            case Relation::GREATER:
                return Relation::LEQ;
            default:
                assert( false );
                return Relation::EQ;
        }
    }
    
    template<typename Pol>
    Relation Constraint<Pol>::turnAroundRelation( const Relation _rel )
    {
        switch( _rel )
        {
            case Relation::EQ:
                return Relation::EQ;
            case Relation::NEQ:
                return Relation::NEQ;
            case Relation::LEQ:
                return Relation::GEQ;
            case Relation::GEQ:
                return Relation::LEQ;
            case Relation::LESS:
                return Relation::GREATER;
            case Relation::GREATER:
                return Relation::LESS;
            default:
                assert( false );
                return Relation::EQ;
        }
    }
    
    static const signed A_IFF_B = 2;
    static const signed A_IMPLIES_B = 1;
    static const signed B_IMPLIES_A = -1;
    static const signed NOT__A_AND_B = -2;
    static const signed A_AND_B__IFF_C = -3;
    static const signed A_XOR_B = -4;

    template<typename Pol>
    signed Constraint<Pol>::compare( const Constraint* _constraintA, const Constraint* _constraintB )
    {
        /*
         * Check whether it holds that 
         * 
         *                      _constraintA  =  a_1*m_1+...+a_k*m_k + c ~ 0
         * and 
         *                      _constraintB  =  b_1*m_1+...+b_k*m_k + d ~ 0, 
         * 
         * where a_1,..., a_k, b_1,..., b_k, c, d are rational coefficients, 
         *       m_1,..., m_k are non-constant monomials and 
         *       exists a rational g such that 
         * 
         *                   a_i = g * b_i for all 1<=i<=k 
         *              or   b_i = g * a_i for all 1<=i<=k 
         */
        typename Pol::NumberType one_divided_by_a = _constraintA->lhs().coprimeFactorWithoutConstant();
        typename Pol::NumberType one_divided_by_b = _constraintB->lhs().coprimeFactorWithoutConstant();
        typename Pol::NumberType c = _constraintA->lhs().constantPart();
        typename Pol::NumberType d = _constraintB->lhs().constantPart();
        assert( carl::isOne(carl::getNum(carl::abs(one_divided_by_a))) && carl::isOne(carl::getNum(carl::abs(one_divided_by_b))) );
        Pol tmpA = (_constraintA->lhs() - c) * one_divided_by_a;
        Pol tmpB = (_constraintB->lhs() - d) * one_divided_by_b;
//        std::cout << "tmpA = " << tmpA << std::endl;
//        std::cout << "tmpB = " << tmpB << std::endl;
        if( tmpA != tmpB ) return 0;
        bool termACoeffGreater = false;
        bool signsDiffer = one_divided_by_a < carl::constant_zero<typename Pol::NumberType>::get() != one_divided_by_b < carl::constant_zero<typename Pol::NumberType>::get();
        typename Pol::NumberType g;
        if( carl::getDenom(one_divided_by_a) > carl::getDenom(one_divided_by_b) )
        {
            g = carl::getDenom(one_divided_by_a)/carl::getDenom(one_divided_by_b);
            if( signsDiffer )
                g = -g;
            termACoeffGreater = true;
            d *= g;
        }
        else
        {
            g = carl::getDenom(one_divided_by_b)/carl::getDenom(one_divided_by_a);
            if( signsDiffer )
                g = -g;
            c *= g;
        }
        // Apply the multiplication by a negative g to the according relation symbol, which
        // has to be turned around then.
        Relation relA = _constraintA->relation();
        Relation relB = _constraintB->relation();
        if( g < 0 )
        {
            if( termACoeffGreater )
            {
                switch( relB )
                {
                    case Relation::LEQ:
                        relB = Relation::GEQ;
                        break;
                    case Relation::GEQ:
                        relB = Relation::LEQ;
                        break;
                    case Relation::LESS:
                        relB = Relation::GREATER;
                        break;
                    case Relation::GREATER:
                        relB = Relation::LESS;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch( relA )
                {
                    case Relation::LEQ:
                        relA = Relation::GEQ;
                        break;
                    case Relation::GEQ:
                        relA = Relation::LEQ;
                        break;
                    case Relation::LESS:
                        relA = Relation::GREATER;
                        break;
                    case Relation::GREATER:
                        relA = Relation::LESS;
                        break;
                    default:
                        break;
                }   
            }
        }
//        std::cout << "c = " << c << std::endl;
//        std::cout << "d = " << d << std::endl;
//        std::cout << "g = " << g << std::endl;
//        std::cout << "relA = " << relA << std::endl;
//        std::cout << "relB = " << relB << std::endl;
        // Compare the adapted constant parts.
        switch( relB )
        {
            case Relation::EQ:
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d=0
                        if( c == d ) return A_IFF_B; 
                        else return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d=0
                        if( c == d ) return A_XOR_B;
                        else return B_IMPLIES_A;
                    case Relation::LESS: // p+c<0  and  p+d=0
                        if( c < d ) return B_IMPLIES_A;
                        else return NOT__A_AND_B;
                    case Relation::GREATER: // p+c>0  and  p+d=0
                        if( c > d ) return B_IMPLIES_A;
                        else return NOT__A_AND_B;
                    case Relation::LEQ: // p+c<=0  and  p+d=0
                        if( c <= d ) return B_IMPLIES_A;
                        else return NOT__A_AND_B;
                    case Relation::GEQ: // p+c>=0  and  p+d=0
                        if( c >= d ) return B_IMPLIES_A;
                        else return NOT__A_AND_B;
                    default:
                        return false;
                }
            case Relation::NEQ:
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d!=0
                        if( c == d ) return A_XOR_B;
                        else return A_IMPLIES_B;
                    case Relation::NEQ: // p+c!=0  and  p+d!=0
                        if( c == d ) return A_IFF_B;
                        else return 0;
                    case Relation::LESS: // p+c<0  and  p+d!=0
                        if( c >= d ) return A_IMPLIES_B;
                        else return 0;
                    case Relation::GREATER: // p+c>0  and  p+d!=0
                        if( c <= d ) return A_IMPLIES_B;
                        else return 0;
                    case Relation::LEQ: // p+c<=0  and  p+d!=0
                        if( c > d ) return A_IMPLIES_B;
                        else if( c == d ) return A_AND_B__IFF_C;
                        else return 0;
                    case Relation::GEQ: // p+c>=0  and  p+d!=0
                        if( c < d ) return A_IMPLIES_B;
                        else if( c == d ) return A_AND_B__IFF_C;
                        else return 0;
                    default:
                        return 0;
                }
            case Relation::LESS:
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d<0
                        if( c > d ) return A_IMPLIES_B;
                        else return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d<0
                        if( c <= d ) return B_IMPLIES_A;
                        else return 0;
                    case Relation::LESS: // p+c<0  and  p+d<0
                        if( c == d ) return A_IFF_B;
                        else if( c < d ) return B_IMPLIES_A;
                        else return A_IMPLIES_B;
                    case Relation::GREATER: // p+c>0  and  p+d<0
                        if( c <= d ) return NOT__A_AND_B;
                        else return 0;
                    case Relation::LEQ: // p+c<=0  and  p+d<0
                        if( c > d ) return A_IMPLIES_B;
                        else return B_IMPLIES_A;
                    case Relation::GEQ: // p+c>=0  and  p+d<0
                        if( c < d ) return NOT__A_AND_B;
                        else if( c == d ) return A_XOR_B;
                        else return 0;
                    default:
                        return 0;
                }
            case Relation::GREATER:
            {
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d>0
                        if( c < d ) return A_IMPLIES_B;
                        else return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d>0
                        if( c >= d ) return B_IMPLIES_A;
                        else return 0;
                    case Relation::LESS: // p+c<0  and  p+d>0
                        if( c >= d ) return NOT__A_AND_B;
                        else return 0;
                    case Relation::GREATER: // p+c>0  and  p+d>0
                        if( c == d ) return A_IFF_B;
                        else if( c > d ) return B_IMPLIES_A;
                        else return A_IMPLIES_B;
                    case Relation::LEQ: // p+c<=0  and  p+d>0
                        if( c > d ) return NOT__A_AND_B;
                        else if( c == d ) return A_XOR_B;
                        else return 0;
                    case Relation::GEQ: // p+c>=0  and  p+d>0
                        if( c > d ) return B_IMPLIES_A;
                        else return A_IMPLIES_B;
                    default:
                        return 0;
                }
            }
            case Relation::LEQ:
            {
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d<=0
                        if( c >= d ) return A_IMPLIES_B;
                        else return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d<=0
                        if( c < d ) return B_IMPLIES_A;
                        else if( c == d ) return A_AND_B__IFF_C;
                        else return 0;
                    case Relation::LESS: // p+c<0  and  p+d<=0
                        if( c < d ) return B_IMPLIES_A;
                        else return A_IMPLIES_B;
                    case Relation::GREATER: // p+c>0  and  p+d<=0
                        if( c < d ) return NOT__A_AND_B;
                        else if( c == d ) return A_XOR_B;
                        else return 0;
                    case Relation::LEQ: // p+c<=0  and  p+d<=0
                        if( c == d ) return A_IFF_B;
                        else if( c < d ) return B_IMPLIES_A;
                        else return A_IMPLIES_B;
                    case Relation::GEQ: // p+c>=0  and  p+d<=0
                        if( c < d ) return NOT__A_AND_B;
                        else if( c == d ) return A_AND_B__IFF_C;
                        else return 0;
                    default:
                        return 0;
                }
            }
            case Relation::GEQ:
            {
                switch( relA )
                {
                    case Relation::EQ: // p+c=0  and  p+d>=0
                        if( c <= d ) return A_IMPLIES_B;
                        else return NOT__A_AND_B;
                    case Relation::NEQ: // p+c!=0  and  p+d>=0
                        if( c > d ) return B_IMPLIES_A;
                        else if( c == d ) return A_AND_B__IFF_C;
                        else return 0;
                    case Relation::LESS: // p+c<0  and  p+d>=0
                        if( c > d ) return NOT__A_AND_B;
                        else if( c == d ) return A_XOR_B;
                        else return 0;
                    case Relation::GREATER: // p+c>0  and  p+d>=0
                        if( c < d ) return B_IMPLIES_A;
                        else return A_IMPLIES_B;
                    case Relation::LEQ: // p+c<=0  and  p+d>=0
                        if( c > d ) return NOT__A_AND_B;
                        else if( c == d ) return A_AND_B__IFF_C;
                        else return 0;
                    case Relation::GEQ: // p+c>=0  and  p+d>=0
                        if( c == d ) return A_IFF_B;
                        else if( c < d ) return A_IMPLIES_B;
                        else return B_IMPLIES_A;
                    default:
                        return 0;
                }
            }
            default:
                return 0;
        }
    }
}    // namespace carl


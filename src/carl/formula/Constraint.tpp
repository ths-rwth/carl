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
#include "../core/polynomialfunctions/Definiteness.h"
#include "../core/polynomialfunctions/Factorization.h"
#include "../core/polynomialfunctions/Substitution.h"


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
    ConstraintContent<Pol>::ConstraintContent( std::size_t _id, Pol&& _lhs, Relation _rel, carl::carlVariables&& _vars, carl::Definiteness _definiteness, unsigned _consistent ):
        mID( _id ),
        mRelation( _rel ),
        mLhs( std::move(_lhs) ),
        mVariables (std::move(_vars)),
        mLhsDefiniteness(_definiteness),
        mConsistency(_consistent),
        mHash( CONSTRAINT_HASH( mLhs, mRelation, Pol ) )
    {
		CARL_LOG_DEBUG("carl.formula.constraint", "Created " << *this);
        initVariableInformations();
    }
    
    template<typename Pol>
    void ConstraintContent<Pol>::initFactorization() 
    {
		mFactorization = carl::factorization(mLhs);
    }
    
    template<typename Pol>
    Constraint<Pol>::Constraint( std::shared_ptr<ConstraintContent<Pol>> _content ):
        mpContent( _content )
    {
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
    {
		CARL_LOG_DEBUG("carl.formula.constraint", _lhs << " " << _rel << " 0  ->  " << *this);
	}

    template<typename Pol>
    template<typename P, EnableIf<needs_cache<P>>>
    Constraint<Pol>::Constraint( const typename P::PolyType& _lhs, Relation _rel ):
        Constraint( ConstraintPool<Pol>::getInstance().create( _lhs, _rel ) )
    {}
    
    template<typename Pol>
    Constraint<Pol>::Constraint( const Constraint<Pol>& _constraint ):
        mpContent( _constraint.mpContent )
    {
    }
    
    template<typename Pol>
    Constraint<Pol>::Constraint( Constraint<Pol>&& _constraint ) noexcept:
        mpContent( _constraint.mpContent )
    {
        _constraint.mpContent.reset();
    }

    template<typename Pol>
    Constraint<Pol>& Constraint<Pol>::operator=( const Constraint<Pol>& _constraint )
    {
        mpContent = _constraint.mpContent;
        return *this;
    }
    
    template<typename Pol>
    Constraint<Pol>& Constraint<Pol>::operator=( Constraint<Pol>&& _constraint ) noexcept
    {
        mpContent = _constraint.mpContent;
        _constraint.mpContent.reset();
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
        Pol tmp = carl::substitute(mpContent->mLhs, _assignment);
        if( tmp.isConstant() )
        {
            result = carl::evaluate( (isZero(tmp) ? typename Pol::NumberType( 0 ) : tmp.trailingTerm().coeff()), relation() ) ? 1 : 0;
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
			auto vars = variables().underlyingVariables();
            auto varIter = vars.begin();
            auto varIntervalIter = _solutionInterval.begin();
            while( varIter != vars.end() && varIntervalIter != _solutionInterval.end() )
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
            if( varIter != vars.end() )
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
                    std::cout << "Error in isConsistent: unexpected relation symbol." << std::endl;
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
			auto vars = variables().underlyingVariables();
            auto varIter = vars.begin();
            auto varIntervalIter = _solutionInterval.begin();
            while( varIter != vars.end() && varIntervalIter != _solutionInterval.end() )
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
            if( varIter != vars.end() )
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
                    std::cout << "Error in isConsistent: unexpected relation symbol." << std::endl;
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
        if( variables().has( _var ))
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
        for( typename std::map<Variable, VarInfo<Pol>>::iterator varInfoPair = mpContent->mVarInfoMap.begin(); varInfoPair != mpContent->mVarInfoMap.end(); ++varInfoPair )
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
                if( d->second.isConstant() && (varInfoPair->first.type() != carl::VariableType::VT_INT || carl::isOne(carl::abs( d->second.constantPart() ))) )
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
		if (lhs().nrTerms() == 0) return false;
		if (!lhs().lterm().isSingleVariable()) return false;
		if (lhs().nrTerms() == 1) {
			_substitutionVariable = lhs().lterm().getSingleVariable();
			_substitutionValue = 0;
			return true;
		}
		assert(lhs().nrTerms() == 2);
		if (!lhs().trailingTerm().isConstant()) return false;
		_substitutionVariable = lhs().lterm().getSingleVariable();
		_substitutionValue = -lhs().trailingTerm().coeff() / lhs().lterm().coeff();
		return true;
	}

	template<typename Pol>
	bool Constraint<Pol>::isPseudoBoolean() const {
		return !variables().boolean().empty();
	}

    template<typename Pol>
    void Constraint<Pol>::printProperties( std::ostream& _out ) const
    {
        _out << "Properties:" << std::endl;
        _out << "   Definitess:              ";
        switch( mpContent->mLhsDefiniteness )
        {
            case Definiteness::NON:
                _out << "NON" << std::endl;
                break;
            case Definiteness::POSITIVE:
                _out << "POSITIVE" << std::endl;
                break;
            case Definiteness::POSITIVE_SEMI:
                _out << "POSITIVE_SEMI" << std::endl;
                break;
            case Definiteness::NEGATIVE:
                _out << "NEGATIVE" << std::endl;
                break;
            case Definiteness::NEGATIVE_SEMI:
                _out << "NEGATIVE_SEMI" << std::endl;
                break;
            default:
                _out << "UNDEFINED" << std::endl;
                break;
        }
        _out << "   The number of monomials: " << lhs().nrTerms() << std::endl;
        _out << "   The maximal degree:      " << (carl::isZero(lhs()) ? 0 : lhs().totalDegree()) << std::endl;
        _out << "   The constant part:       " << constantPart() << std::endl;
        _out << "   Variables:" << std::endl;
        for( auto vi = mpContent->mVarInfoMap.begin(); vi != mpContent->mVarInfoMap.end(); ++vi )
        {
            _out << "        " << vi->first << " has " << vi->second.occurence() << " occurences." << std::endl;
            _out << "        " << vi->first << " has the maximal degree of " << vi->second.maxDegree() << "." << std::endl;
            _out << "        " << vi->first << " has the minimal degree of " << vi->second.minDegree() << "." << std::endl;
        }
    }
}    // namespace carl

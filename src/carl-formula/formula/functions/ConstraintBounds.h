#pragma once

namespace carl {

/// A map from formula pointers to a map of rationals to a pair of a constraint relation and a formula pointer. (internally used)
template<typename Pol>
using ConstraintBounds = FastMap<Pol, std::map<typename Pol::NumberType, std::pair<Relation,Formula<Pol>>>> ;

//    #define CONSTRAINT_BOUND_DEBUG

/**
 * Adds the bound to the bounds of the polynomial specified by this constraint. E.g., if the constraint is p+b~0, where p is a sum
 * of terms, being a rational (actually integer) coefficient times a non-trivial (!=1) monomial( product of variables to the power
 * of an exponent), b is a rational and ~ is any constraint relation. Furthermore, the leading coefficient of p is 1. Then we add
 * the bound -b to the bounds of p (means that p ~ -b) stored in the given constraint bounds.
 * @param _constraintBounds An object collecting bounds of polynomials.
 * @param _constraint The constraint to find a bound for a polynomial for.
 * @param _inConjunction true, if the constraint is part of a conjunction.
 *                       false, if the constraint is part of a disjunction.
 * @return Formula<Pol>( FALSE ), if the yet determined bounds imply that the conjunction (_inConjunction == true) or disjunction
 *                (_inConjunction == false) of which we got the given constraint is invalid resp. valid;
 *         false, the added constraint.
 */
template<typename Pol>
Formula<Pol> addConstraintBound( ConstraintBounds<Pol>& _constraintBounds, const Formula<Pol>& _constraint, bool _inConjunction )
{
    #ifdef CONSTRAINT_BOUND_DEBUG
    std::cout << "add from a " << (_inConjunction ? "conjunction" : "disjunction") << " to " << &_constraintBounds << ":   " << _constraint << std::endl;
    #endif
    bool negated = _constraint.type() == FormulaType::NOT;
    assert( _constraint.type() == FormulaType::CONSTRAINT || (negated && _constraint.subformula().type() == FormulaType::CONSTRAINT ) );
    const Constraint<Pol>& constraint = negated ? _constraint.subformula().constraint() : _constraint.constraint();
    assert( constraint.isConsistent() == 2 );
    typename Pol::NumberType boundValue;
    Relation relation = negated ? carl::inverse( constraint.relation() ) : constraint.relation();
    const Pol& lhs = constraint.lhs();
    Pol poly;
    bool multipliedByMinusOne = lhs.lterm().coeff() < typename Pol::NumberType( 0 );
    if( multipliedByMinusOne )
    {
        boundValue = constraint.lhs().constantPart();
        relation = carl::turn_around( relation );
        poly = Pol( -lhs + boundValue );
    }
    else
    {
        boundValue = -constraint.lhs().constantPart();
        poly = Pol( lhs + boundValue );
    }
    typename Pol::NumberType cf( poly.coprimeFactor() );
    assert( cf > 0 );
    boundValue *= cf;
    poly *= cf;
    #ifdef CONSTRAINT_BOUND_DEBUG
    std::cout << "try to add the bound  " << relation << boundValue << "  for the polynomial  " << poly << std::endl;
    #endif
    auto resA = _constraintBounds.insert( std::make_pair( std::move(poly), std::move( std::map<typename Pol::NumberType, std::pair<Relation, Formula<Pol>>>() ) ) );
    auto resB = resA.first->second.insert( std::make_pair( boundValue, std::make_pair( relation, _constraint ) ) );
    if( resB.second || resB.first->second.first == relation )
        return resB.first->second.second;
    switch( relation )
    {
        case Relation::EQ:
            if( _inConjunction )
            {
                if( resB.first->second.first == Relation::LEQ || resB.first->second.first == Relation::GEQ )
                {
                    resB.first->second.first = Relation::EQ;
                    resB.first->second.second = _constraint;
                    return resB.first->second.second;
                }
                else
                    return Formula<Pol>( FormulaType::FALSE );
            }
            else
            {
                switch( resB.first->second.first )
                {
                    case Relation::LEQ:
                        return resB.first->second.second;
                    case Relation::GEQ:
                        return resB.first->second.second;
                    case Relation::LESS:
                        resB.first->second.first = Relation::LEQ;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GEQ : Relation::LEQ );
                        return resB.first->second.second;
                    case Relation::GREATER:
                        resB.first->second.first = Relation::GEQ;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LEQ : Relation::GEQ );
                        return resB.first->second.second;
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        return Formula<Pol>( FormulaType::FALSE );
                }
            }
        case Relation::LEQ:
            if( _inConjunction )
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        return resB.first->second.second;
                    case Relation::GEQ:
                        resB.first->second.first = Relation::EQ;
                        resB.first->second.second = Formula<Pol>( lhs, Relation::EQ );
                        return resB.first->second.second;
                    case Relation::LESS:
                        return resB.first->second.second;
                    case Relation::GREATER:
                        return Formula<Pol>( FormulaType::FALSE );
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        resB.first->second.first = Relation::LESS;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS );
                        return resB.first->second.second;
                }
            }
            else
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        resB.first->second.first = Relation::LEQ;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                    case Relation::GEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::LESS:
                        resB.first->second.first = Relation::LEQ;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                    case Relation::GREATER:
                        return Formula<Pol>( FormulaType::FALSE );
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        return Formula<Pol>( FormulaType::FALSE );
                }
            }
        case Relation::GEQ:
            if( _inConjunction )
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        return resB.first->second.second;
                    case Relation::LEQ:
                        resB.first->second.first = Relation::EQ;
                        resB.first->second.second = Formula<Pol>( lhs, Relation::EQ );
                        return resB.first->second.second;
                    case Relation::LESS:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::GREATER:
                        return resB.first->second.second;
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        resB.first->second.first = Relation::GREATER;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER );
                        return resB.first->second.second;
                }
            }
            else
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        resB.first->second.first = Relation::GEQ;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                    case Relation::LEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::LESS:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::GREATER:
                        resB.first->second.first = Relation::GEQ;
                        resB.first->second.second = _constraint;
                        return Formula<Pol>( FormulaType::FALSE );
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        return Formula<Pol>( FormulaType::FALSE );
                }
            }
        case Relation::LESS:
            if( _inConjunction )
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::LEQ:
                        resB.first->second.first = Relation::LESS;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                    case Relation::GEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::GREATER:
                        return Formula<Pol>( FormulaType::FALSE );
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        resB.first->second.first = Relation::LESS;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                }
            }
            else
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        resB.first->second.first = Relation::LEQ;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GEQ : Relation::LEQ );
                        return resB.first->second.second;
                    case Relation::LEQ:
                        return resB.first->second.second;
                    case Relation::GEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::GREATER:
                        resB.first->second.first = Relation::NEQ;
                        resB.first->second.second = Formula<Pol>( lhs, Relation::NEQ );
                        return resB.first->second.second;
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        return resB.first->second.second;
                }
            }
        case Relation::GREATER:
            if( _inConjunction )
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::LEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::GEQ:
                        resB.first->second.first = Relation::GREATER;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                    case Relation::LESS:
                        return Formula<Pol>( FormulaType::FALSE );
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        resB.first->second.first = Relation::GREATER;
                        resB.first->second.second = _constraint;
                        return resB.first->second.second;
                }
            }
            else
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        resB.first->second.first = Relation::GEQ;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LEQ : Relation::GEQ );
                        return resB.first->second.second;
                    case Relation::LEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::GEQ:
                        return resB.first->second.second;
                    case Relation::LESS:
                        resB.first->second.first = Relation::NEQ;
                        resB.first->second.second = Formula<Pol>( lhs, Relation::NEQ );
                        return resB.first->second.second;
                    default:
                        assert( resB.first->second.first == Relation::NEQ );
                        return resB.first->second.second;
                }
            }
        default:
            assert( relation == Relation::NEQ );
            if( _inConjunction )
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::LEQ:
                        resB.first->second.first = Relation::LESS;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS );
                        return resB.first->second.second;
                    case Relation::GEQ:
                        resB.first->second.first = Relation::GREATER;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER );
                        return resB.first->second.second;
                    case Relation::LESS:
                        resB.first->second.first = Relation::LESS;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::GREATER : Relation::LESS );
                        return resB.first->second.second;
                    default:
                        assert( resB.first->second.first == Relation::GREATER );
                        resB.first->second.first = Relation::GREATER;
                        resB.first->second.second = Formula<Pol>( lhs, multipliedByMinusOne ? Relation::LESS : Relation::GREATER );
                        return resB.first->second.second;
                }
            }
            else
            {
                switch( resB.first->second.first )
                {
                    case Relation::EQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::LEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::GEQ:
                        return Formula<Pol>( FormulaType::FALSE );
                    case Relation::LESS:
                        return resB.first->second.second;
                    default:
                        assert( resB.first->second.first == Relation::GREATER );
                        return resB.first->second.second;
                }
            }
    }
}

/**
 * Stores for every polynomial for which we determined bounds for given constraints a minimal set of constraints
 * representing these bounds into the given set of sub-formulas of a conjunction (_inConjunction == true) or disjunction
 * (_inConjunction == false) to construct.
 * @param _constraintBounds An object collecting bounds of polynomials.
 * @param _intoAsts A set of sub-formulas of a conjunction (_inConjunction == true) or disjunction (_inConjunction == false) to construct.
 * @param _inConjunction true, if constraints representing the polynomial's bounds are going to be part of a conjunction.
 *                       false, if constraints representing the polynomial's bounds are going to be part of a disjunction.
 * @return true, if the yet added bounds imply that the conjunction (_inConjunction == true) or disjunction
 *                (_inConjunction == false) to which the bounds are added is invalid resp. valid;
 *         false, otherwise.
 */
template<typename Pol>
bool swapConstraintBounds( ConstraintBounds<Pol>& _constraintBounds, Formulas<Pol>& _intoFormulas, bool _inConjunction )
{
    #ifdef CONSTRAINT_BOUND_DEBUG
    std::cout << "swap from " << &_constraintBounds << " to a " << (_inConjunction ? "conjunction" : "disjunction") << std::endl;
    #endif
    while( !_constraintBounds.empty() )
    {
        #ifdef CONSTRAINT_BOUND_DEBUG
        std::cout << "for the bounds of  " << _constraintBounds.begin()->first << std::endl;
        #endif
        const std::map<typename Pol::NumberType, std::pair<Relation, Formula<Pol>>>& bounds = _constraintBounds.begin()->second;
        assert( !bounds.empty() );
        if( bounds.size() == 1 )
        {
            _intoFormulas.push_back( bounds.begin()->second.second );
            #ifdef CONSTRAINT_BOUND_DEBUG
            std::cout << "   just add the only bound" << std::endl;
            #endif
        }
        else
        {
            auto mostSignificantLowerBound = bounds.end();
            auto mostSignificantUpperBound = bounds.end();
            auto moreSignificantCase = bounds.end();
            Formulas<Pol> lessSignificantCases;
            auto iter = bounds.begin();
            for( ; iter != bounds.end(); ++iter )
            {
                #ifdef CONSTRAINT_BOUND_DEBUG
                std::cout << "   bound is  " << iter->second.first << iter->first << std::endl;
                #endif
                if( (_inConjunction && iter->second.first == Relation::NEQ)
                    || (!_inConjunction && iter->second.first == Relation::EQ) )
                {
                    if( moreSignificantCase == bounds.end() )
                    {
                        if( (_inConjunction && mostSignificantUpperBound == bounds.end())
                            || (!_inConjunction && mostSignificantLowerBound == bounds.end()) )
                        {
                            if( (_inConjunction && mostSignificantLowerBound != bounds.end())
                                || (!_inConjunction && mostSignificantUpperBound != bounds.end()) )
                            {
                                #ifdef CONSTRAINT_BOUND_DEBUG
                                std::cout << "      case: " << __LINE__ << std::endl;
                                #endif
                                _intoFormulas.push_back( iter->second.second );
                            }
                            else
                            {
                                #ifdef CONSTRAINT_BOUND_DEBUG
                                std::cout << "      case: " << __LINE__ << std::endl;
                                #endif
                                lessSignificantCases.push_back( iter->second.second );
                            }
                        }
                    }
                }
                else if( (_inConjunction && (iter->second.first == Relation::GEQ || iter->second.first == Relation::GREATER)) // found a lower bound
                            || (!_inConjunction && (iter->second.first == Relation::LEQ || iter->second.first == Relation::LESS)) ) // found an upper bound
                {
                    if( (_inConjunction && mostSignificantUpperBound != bounds.end()) // found already an upper bound -> conjunction is invalid!
                        || (!_inConjunction && mostSignificantLowerBound != bounds.end()) // found already a lower bound -> disjunction is valid!
                        || moreSignificantCase != bounds.end() )
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        std::cout << "      case: " << __LINE__ << std::endl;
                        #endif
                        break;
                    }
                    else
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        std::cout << "      case: " << __LINE__ << std::endl;
                        #endif
                        if( _inConjunction ) // update the strongest upper bound
                            mostSignificantLowerBound = iter;
                        else // update the weakest upper bound
                            mostSignificantUpperBound = iter;
                        lessSignificantCases.clear();
                    }
                }
                else if( (_inConjunction && iter->second.first == Relation::EQ)
                        || (!_inConjunction && iter->second.first == Relation::NEQ) )
                {
                    // _inConjunction == true: found already another equality or an upper bound -> conjunction is invalid!
                    // _inConjunction == false: found already another bound with != as relation or a lower bound -> disjunction is valid!
                    if( moreSignificantCase != bounds.end() || (_inConjunction ? mostSignificantUpperBound : mostSignificantLowerBound) != bounds.end() )
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        std::cout << "      case: " << __LINE__ << std::endl;
                        #endif
                        break;
                    }
                    // _inConjunction == true: found first equality
                    // _inConjunction == false: found first bound with !=
                    else
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        std::cout << "      case: " << __LINE__ << std::endl;
                        #endif
                        moreSignificantCase = iter;
                    }
                }
                // _inConjunction == true: found an upper bound
                // _inConjunction == false: found a lower bound
                else
                {
                    #ifdef CONSTRAINT_BOUND_DEBUG
                    std::cout << "      case: " << __LINE__ << std::endl;
                    #endif
                    assert( !_inConjunction || iter->second.first == Relation::LEQ || iter->second.first == Relation::LESS );
                    assert( _inConjunction || iter->second.first == Relation::GEQ || iter->second.first == Relation::GREATER );
                    if( _inConjunction && mostSignificantUpperBound == bounds.end() ) // first upper bound found = strongest upper bound
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        std::cout << "      case: " << __LINE__ << std::endl;
                        #endif
                        mostSignificantUpperBound = iter;
                    }
                    else if( !_inConjunction && mostSignificantLowerBound == bounds.end() ) // first lower bound found = weakest lower bound
                    {
                        #ifdef CONSTRAINT_BOUND_DEBUG
                        std::cout << "      case: " << __LINE__ << std::endl;
                        #endif
                        mostSignificantLowerBound = iter;
                    }
                }
            }
            if( iter != bounds.end() )
                break;
            if( moreSignificantCase != bounds.end() )
            {
                _intoFormulas.push_back( moreSignificantCase->second.second );
            }
            else
            {
                #ifdef CONSTRAINT_BOUND_DEBUG
                if( !(_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                        || mostSignificantUpperBound->first > mostSignificantLowerBound->first)
                    || !( !_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                            || mostSignificantLowerBound->first > mostSignificantUpperBound->first ) )
                {
                    std::cout << "mostSignificantUpperBound:   " << mostSignificantUpperBound->first << "  [" << mostSignificantUpperBound->second.second << "]" << std::endl;
                    std::cout << "mostSignificantLowerBound:   " << mostSignificantLowerBound->first << "  [" << mostSignificantLowerBound->second.second << "]" << std::endl;
                }
                #endif
                assert( !_inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                        || mostSignificantUpperBound->first > mostSignificantLowerBound->first );
                assert( _inConjunction || mostSignificantUpperBound == bounds.end() || mostSignificantLowerBound == bounds.end()
                            || mostSignificantLowerBound->first > mostSignificantUpperBound->first );
                if( mostSignificantUpperBound != bounds.end() )
                    _intoFormulas.push_back( mostSignificantUpperBound->second.second );
                if( mostSignificantLowerBound != bounds.end() )
                    _intoFormulas.push_back( mostSignificantLowerBound->second.second );
                _intoFormulas.insert(_intoFormulas.end(), lessSignificantCases.begin(), lessSignificantCases.end() );
            }
        }
        _constraintBounds.erase( _constraintBounds.begin() );
    }
    if( _constraintBounds.empty() )
    {
        #ifdef CONSTRAINT_BOUND_DEBUG
        std::cout << std::endl;
        #endif
        return false;
    }
    else
    {
        _constraintBounds.clear();
        #ifdef CONSTRAINT_BOUND_DEBUG
        std::cout << "is " << (_inConjunction ? "invalid" : "valid") << std::endl << std::endl;
        #endif
        return true;
    }
}

}
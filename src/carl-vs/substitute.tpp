#include <cmath>
#include <limits>

#include <carl-arith/poly/umvpoly/functions/Derivative.h>
#include <carl-arith/poly/umvpoly/functions/SoSDecomposition.h>
#include <carl-arith/constraint/IntervalEvaluation.h>

#include <carl-arith/vs/Substitution.h>

//#define VS_DEBUG_SUBSTITUTION
const unsigned MAX_NUM_OF_TERMS = 512;


namespace carl::vs::detail 
{
    template <class combineType> 
    bool combine( const std::vector< std::vector< std::vector< combineType > > >& _toCombine, std::vector< std::vector< combineType > >& _combination )
    {
        // Initialize the current combination. If there is nothing to combine or an empty vector to combine with, return the empty vector.
        if( _toCombine.empty() ) return true;
        std::vector<typename std::vector< std::vector< combineType > >::const_iterator > combination 
            = std::vector<typename std::vector< std::vector< combineType > >::const_iterator >();
        unsigned estimatedResultSize = 1;
        for( auto iter = _toCombine.begin(); iter != _toCombine.end(); ++iter )
        {
            if( iter->empty() )
                return true;
            estimatedResultSize *= (unsigned)iter->size();
            if( estimatedResultSize > MAX_NUM_OF_COMBINATION_RESULT )
                return false;
            else 
                combination.push_back( iter->begin() );
        }
        // As long as no more combination for the last vector in first vector of vectors exists.
        bool lastCombinationReached = false;
        while( !lastCombinationReached )
        {
            // Create a new combination of vectors.
            _combination.push_back( std::vector< combineType >() );
            bool previousCounterIncreased = false;
            // For each vector in the vector of vectors, choose a vector. Finally we combine
            // those vectors by merging them to a new vector and add this to the result.
            auto currentVector = _toCombine.begin();
            for( auto combineElement = combination.begin(); combineElement != combination.end(); ++combineElement )
            {
                // Add the current vectors elements to the combination.
                _combination.back().insert( _combination.back().end(), (*combineElement)->begin(), (*combineElement)->end() );
                // Set the counter.
                if( !previousCounterIncreased )
                {
                    ++(*combineElement);
                    if( *combineElement != currentVector->end() )
                        previousCounterIncreased = true;
                    else
                    {
                        if( combineElement != --combination.end() ) 
                            *combineElement = currentVector->begin();
                        else 
                            lastCombinationReached = true;
                    }
                }
                ++currentVector;
            }
        }
        return true;
    }

    template<typename Poly>
    void simplify( CaseDistinction<Poly>& _toSimplify )
    {
        bool containsEmptyDisjunction = false;
        auto conj = _toSimplify.begin();
        while( conj != _toSimplify.end() )
        {
            bool conjInconsistent = false;
            auto cons = (*conj).begin();
            while( cons != (*conj).end() )
            {
                if( *cons == Constraint<Poly>( false ) )
                {
                    conjInconsistent = true;
                    break;
                }
                else if( *cons == Constraint<Poly>( true ) )
                    cons = (*conj).erase( cons );
                else
                    cons++;
            }
            bool conjEmpty = (*conj).empty();
            if( conjInconsistent || (containsEmptyDisjunction && conjEmpty) )
            {
                // Delete the conjunction.
                conj->clear();
                conj = _toSimplify.erase( conj );
            }
            else
                conj++;
            if( !containsEmptyDisjunction && conjEmpty )
                containsEmptyDisjunction = true;
        }
    }

    template<typename Poly>
    void simplify( CaseDistinction<Poly>& _toSimplify, Variables& _conflictingVars, const detail::EvalDoubleIntervalMap& _solutionSpace )
    {
        bool containsEmptyDisjunction = false;
        auto conj = _toSimplify.begin();
        while( conj != _toSimplify.end() )
        {
            bool conjInconsistent = false;
            auto cons = (*conj).begin();
            while( cons != (*conj).end() )
            {
                if( *cons == Constraint<Poly>( false ) )
                {
                    conjInconsistent = true;
                    break;
                }
                else if( *cons == Constraint<Poly>( true ) )
                    cons = (*conj).erase( cons );
                else
                {
                    unsigned conflictingWithSolutionSpace = consistent_with(cons->constr(), _solutionSpace );
                    
//                    std::cout << "Is  " << cons << std::endl;
//                    std::cout << std::endl;
//                    std::cout << "consistent with  " << std::endl;
//                    for( auto iter = _solutionSpace.begin(); iter != _solutionSpace.end(); ++iter )
//                        std::cout << iter->first << " in " << iter->second << std::endl;
//                    std::cout << "   ->  " << conflictingWithSolutionSpace << std::endl;
                    
                    if( conflictingWithSolutionSpace == 0 )
                    {
                        auto vars = variables(*cons);
                        _conflictingVars.insert( vars.begin(), vars.end() );
                        conjInconsistent = true;
                        break;
                    }
                    else if( conflictingWithSolutionSpace == 1 )
                        cons = (*conj).erase( cons );
                    else
                        ++cons;
                }
            }
            bool conjEmpty = (*conj).empty();
            if( conjInconsistent || (containsEmptyDisjunction && conjEmpty) )
            {
                // Delete the conjunction.
                conj->clear();
                conj = _toSimplify.erase( conj );
            }
            else
                ++conj;
            if( !containsEmptyDisjunction && conjEmpty )
                containsEmptyDisjunction = true;
        }
    }

    template<typename Poly>
    bool splitProducts( CaseDistinction<Poly>& _toSimplify, bool _onlyNeq )
    {
        std::map<const Constraint<Poly>, CaseDistinction<Poly>> result_cache;
        bool result = true;
        size_t toSimpSize = _toSimplify.size();
        for( size_t pos = 0; pos < toSimpSize; )
        {
            if( !_toSimplify.begin()->empty() )
            {
                CaseDistinction<Poly> temp;
                if( !splitProducts( _toSimplify[pos], temp, result_cache, _onlyNeq ) )
                    result = false;
                _toSimplify.erase( _toSimplify.begin() );
                _toSimplify.insert( _toSimplify.end(), temp.begin(), temp.end() );
                --toSimpSize;
            }
            else
                ++pos;
        }
        return result;
    }

    template<typename Poly>
    bool splitProducts( const ConstraintConjunction<Poly>& _toSimplify, CaseDistinction<Poly>& _result, std::map<const Constraint<Poly>, CaseDistinction<Poly>>& result_cache, bool _onlyNeq )
    {
        std::vector<CaseDistinction<Poly>> toCombine;
        for( auto constraint = _toSimplify.begin(); constraint != _toSimplify.end(); ++constraint )
        {
            auto i = result_cache.find(*constraint);
            if (i == result_cache.end()) {
                auto res = splitProducts(*constraint, _onlyNeq);
                i = result_cache.emplace(*constraint, res).first;
            }
            toCombine.push_back(i->second);
        }
        bool result = true;
        if( !combine( toCombine, _result ) )
            result = false;
        simplify( _result );
        return result;
    }

    template<typename Poly>
    CaseDistinction<Poly> splitProducts( const Constraint<Poly>& _constraint, bool _onlyNeq )
    {
        CaseDistinction<Poly> result;
        auto& factorization = _constraint.lhs_factorization();
        if( !carl::is_trivial(factorization) )
        {
            switch( _constraint.relation() )
            {
                case Relation::EQ:
                {
                    if( !_onlyNeq )
                    {
                        for( auto factor = factorization.begin(); factor != factorization.end(); ++factor )
                        {
                            result.emplace_back();
                            result.back().push_back( Constraint<Poly>( factor->first, Relation::EQ ) );
                        }
                    }
                    else
                    {
                        result.emplace_back();
                        result.back().push_back( _constraint );
                    }
                    simplify( result );
                    break;
                }
                case Relation::NEQ:
                {
                    result.emplace_back();
                    for( auto factor = factorization.begin(); factor != factorization.end(); ++factor )
                        result.back().push_back( Constraint<Poly>( factor->first, Relation::NEQ ) );
                    simplify( result );
                    break;
                }
                default:
                {
                    if( !_onlyNeq )
                    {
                        result = getSignCombinations( _constraint );
                        simplify( result );
                    }
                    else
                    {
                        result.emplace_back();
                        result.back().push_back( _constraint );
                    }
                }
                
            }
        }
        else
        {
            result.emplace_back();
            result.back().push_back( _constraint );
        }
        return result;
    }

    template<typename Poly>
    void splitSosDecompositions( CaseDistinction<Poly>& _toSimplify )
    {
        // TODO this needs to be reviewed and fixed
        // It seems that is is assumed that lcoeffNeg * constraint.lhs() is positive if
        // a SOS decomposition exists. However, we can only follow that it's non-negative
        // (in the univariate case), for the multivariate case more requirements need to be made
        // (therefor constraint.lhs().has_constant_term() ???).
        // This lead to wrong simplifications in very rare cases, for example
        // -100 + 140*z + -49*y^2 + -49*z^2 is wrongly simplified to true (see McsatVSBug).
        
        // Temporarily disabled (what can be followed in the multivariate case?).
        return;

        for( size_t i = 0; i < _toSimplify.size(); )
        {
            auto& cc = _toSimplify[i];
            bool foundNoInvalidConstraint = true;
            size_t pos = 0;
            while( foundNoInvalidConstraint && pos < cc.size() )
            {
                const Constraint<Poly>& constraint = cc[pos];
                std::vector<std::pair<typename Poly::NumberType,Poly>> sosDec;
                bool lcoeffNeg = carl::is_negative(constraint.lhs().lcoeff());
                if (lcoeffNeg)
                    sosDec = carl::sos_decomposition(-constraint.lhs());
                else
                    sosDec = carl::sos_decomposition(constraint.lhs());
                if( sosDec.size() > 1 )
                {
//                    std::cout << "Sum-of-squares decomposition of " << constraint.lhs() << " = " << sosDec << std::endl;
                    bool addSquares = true;
                    bool constraintValid = false;
                    switch( constraint.relation() )
                    {
                        case carl::Relation::EQ:
                        {
                            if( constraint.lhs().has_constant_term() )
                            {
                                foundNoInvalidConstraint = false;
                                addSquares = false;
                            }
                            break;
                        }
                        case carl::Relation::NEQ:
                        {
                            addSquares = false;
                            if( constraint.lhs().has_constant_term() )
                            {
                                constraintValid = true;
                            }
                            break;
                        }
                        case carl::Relation::LEQ:
                        {
                            if( lcoeffNeg )
                            {
                                addSquares = false;
                                constraintValid = true;
                            }
                            else if( constraint.lhs().has_constant_term() )
                            {
                                addSquares = false;
                                foundNoInvalidConstraint = false;
                            }
                            break;
                        }
                        case carl::Relation::LESS:
                        {
                            addSquares = false;
                            if( lcoeffNeg )
                            {
                                if( constraint.lhs().has_constant_term() )
                                    constraintValid = true;
                            }
                            else 
                                foundNoInvalidConstraint = false;
                            break;
                        }
                        case carl::Relation::GEQ:
                        {
                            if( !lcoeffNeg )
                            {
                                addSquares = false;
                                constraintValid = true;
                            }
                            else if( constraint.lhs().has_constant_term() )
                            {
                                addSquares = false;
                                foundNoInvalidConstraint = false;
                            }
                            break;
                        }
                        default:
                        {
                            assert( constraint.relation() == carl::Relation::GREATER );
                            addSquares = false;
                            if( lcoeffNeg )
                                foundNoInvalidConstraint = false;
                            else
                            {
                                if( constraint.lhs().has_constant_term() )
                                    constraintValid = true;
                            }
                        }
                    }
                    assert( !(!foundNoInvalidConstraint && constraintValid) );
                    assert( !(!foundNoInvalidConstraint && addSquares) );
                    if( constraintValid || addSquares )
                    {
                        cc[pos] = cc.back();
                        cc.pop_back();
                    }
                    else
                        ++pos;
                    if( addSquares )
                    {
                        for( auto it = sosDec.begin(); it != sosDec.end(); ++it )
                        {
                            cc.emplace_back( it->second, carl::Relation::EQ );
                        }
                    }   
                }
                else
                    ++pos;
            }
            if( foundNoInvalidConstraint )
                ++i;
            else
            {
                cc = _toSimplify.back();
                _toSimplify.pop_back();
            }
        }
    }

    template<typename Poly>
    CaseDistinction<Poly> getSignCombinations( const Constraint<Poly>& _constraint )
    {
        CaseDistinction<Poly> combinations;
        auto& factorization = _constraint.lhs_factorization();
        if( !carl::is_trivial(factorization) && factorization.size() <= MAX_PRODUCT_SPLIT_NUMBER )
        {
            assert( _constraint.relation() == Relation::GREATER || _constraint.relation() == Relation::LESS
                    || _constraint.relation() == Relation::GEQ || _constraint.relation() == Relation::LEQ );
            Relation relPos = Relation::GREATER;
            Relation relNeg = Relation::LESS;
            if( _constraint.relation() == Relation::GEQ || _constraint.relation() == Relation::LEQ )
            {
                relPos = Relation::GEQ;
                relNeg = Relation::LEQ;
            }
            bool positive = (_constraint.relation() == Relation::GEQ || _constraint.relation() == Relation::GREATER);
            ConstraintConjunction<Poly> positives;
            ConstraintConjunction<Poly> alwayspositives;
            ConstraintConjunction<Poly> negatives;
            ConstraintConjunction<Poly> alwaysnegatives;
            unsigned numOfAlwaysNegatives = 0;
            for( auto factor = factorization.begin(); factor != factorization.end(); ++factor )
            {
                Constraint<Poly> consPos = Constraint<Poly>( factor->first, relPos );
                unsigned posConsistent = consPos.is_consistent();
                if( posConsistent != 0 )
                    positives.push_back( consPos );
                Constraint<Poly> consNeg = Constraint<Poly>( factor->first, relNeg );
                unsigned negConsistent = consNeg.is_consistent();
                if( negConsistent == 0 )
                {
                    if( posConsistent == 0 )
                    {
                        combinations.emplace_back();
                        combinations.back().push_back( consNeg );
                        return combinations;
                    }
                    if( posConsistent != 1 )
                        alwayspositives.push_back( positives.back() );
                    positives.pop_back();
                }
                else
                {
                    if( posConsistent == 0 )
                    {
                        ++numOfAlwaysNegatives;
                        if( negConsistent != 1 ) 
                            alwaysnegatives.push_back( consNeg );
                    }
                    else negatives.push_back( consNeg );
                }
            }
            assert( positives.size() == negatives.size() );
            if( positives.size() > 0 )
            {
                std::vector< std::bitset<MAX_PRODUCT_SPLIT_NUMBER> > combSelector = std::vector< std::bitset<MAX_PRODUCT_SPLIT_NUMBER> >();
                if( fmod( numOfAlwaysNegatives, 2.0 ) != 0.0 )
                {
                    if( positive ) 
                        getOddBitStrings( positives.size(), combSelector );
                    else 
                        getEvenBitStrings( positives.size(), combSelector );
                }
                else
                {
                    if( positive ) 
                        getEvenBitStrings( positives.size(), combSelector );
                    else 
                        getOddBitStrings( positives.size(), combSelector );
                }
                for( auto comb = combSelector.begin(); comb != combSelector.end(); ++comb )
                {
                    combinations.emplace_back( alwaysnegatives );
                    combinations.back().insert( combinations.back().end(), alwayspositives.begin(), alwayspositives.end() );
                    for( unsigned pos = 0; pos < positives.size(); ++pos )
                    {
                        if( (*comb)[pos] ) 
                            combinations.back().push_back( negatives[pos] );
                        else 
                            combinations.back().push_back( positives[pos] );
                    }
                }
            }
            else
            {
                combinations.emplace_back( alwaysnegatives );
                combinations.back().insert( combinations.back().end(), alwayspositives.begin(), alwayspositives.end() );
            }
        }
        else
        {
            combinations.emplace_back();
            combinations.back().push_back( _constraint );
        }
        return combinations;
    }

    void getOddBitStrings( std::size_t _length, std::vector< std::bitset<MAX_PRODUCT_SPLIT_NUMBER> >& _strings )
    {
        assert( _length > 0 );
        if( _length == 1 )  _strings.push_back( std::bitset<MAX_PRODUCT_SPLIT_NUMBER>( 1 ) );
        else
        {
            std::size_t pos = _strings.size();
            getEvenBitStrings( _length - 1, _strings );
            for( ; pos < _strings.size(); ++pos )
            {
                _strings[pos] <<= 1;
                _strings[pos].flip(0);
            }
            getOddBitStrings( _length - 1, _strings );
            for( ; pos < _strings.size(); ++pos ) _strings[pos] <<= 1;
        }
    }

    void getEvenBitStrings( std::size_t _length, std::vector< std::bitset<MAX_PRODUCT_SPLIT_NUMBER> >& _strings )
    {
        assert( _length > 0 );
        if( _length == 1 ) _strings.push_back( std::bitset<MAX_PRODUCT_SPLIT_NUMBER>( 0 ) );
        else
        {
            std::size_t pos = _strings.size();
            getEvenBitStrings( _length - 1, _strings );
            for( ; pos < _strings.size(); ++pos ) _strings[pos] <<= 1;
            getOddBitStrings( _length - 1, _strings );
            for( ; pos < _strings.size(); ++pos )
            {
                _strings[pos] <<= 1;
                _strings[pos].flip(0);
            }
        }
    }

    template<typename Poly>
    void print( CaseDistinction<Poly>& _substitutionResults )
    {
        auto conj = _substitutionResults.begin();
        while( conj != _substitutionResults.end() )
        {
            if( conj != _substitutionResults.begin() )
                std::cout << " or (";
            else
                std::cout << "    (";
            auto cons = (*conj).begin();
            while( cons != (*conj).end() )
            {
                if( cons != (*conj).begin() )
                    std::cout << " and ";
                std::cout << *cons;
                cons++;
            }
            std::cout << ")" << std::endl;
            conj++;
        }
        std::cout << std::endl;
    }
    
    template<typename Poly>
    bool substitute( const Constraint<Poly>& _cons,
                     const Substitution<Poly>& _subs,
                     CaseDistinction<Poly>& _result,
                     bool _accordingPaper,
                     Variables& _conflictingVariables,
                     const detail::EvalDoubleIntervalMap& _solutionSpace)
    {
        #ifdef VS_DEBUG_SUBSTITUTION
        std::cout << "substitute: ( " << _cons << " )" << _subs << std::endl;
        #endif
        bool result = true;
        // Apply the substitution according to its type.
        switch( _subs.term().type() )
        {
            case TermType::NORMAL:
            {
                result = substituteNormal( _cons, _subs, _result, _accordingPaper, _conflictingVariables, _solutionSpace );
                break;
            }
            case TermType::PLUS_EPSILON:
            {
                result = substitutePlusEps( _cons, _subs, _result, _accordingPaper, _conflictingVariables, _solutionSpace );
                break;
            }
            case TermType::MINUS_INFINITY:
            {
                substituteInf( _cons, _subs, _result, _conflictingVariables, _solutionSpace );
                break;
            }
            case TermType::PLUS_INFINITY:
            {
                substituteInf( _cons, _subs, _result, _conflictingVariables, _solutionSpace );
                break;
            }
            default:
            {
                std::cout << "Error in substitute: unexpected type of substitution." << std::endl;
            }
        }
        #ifdef VS_DEBUG_SUBSTITUTION
        print( _result );
        #endif
        return result;
    }

    template<typename Poly>
    bool substituteNormal( const Constraint<Poly>& _cons,
                           const Substitution<Poly>& _subs,
                           CaseDistinction<Poly>& _result,
                           bool _accordingPaper,
                           Variables& _conflictingVariables,
                           const detail::EvalDoubleIntervalMap& _solutionSpace )
    {
        
        bool result = true;
        if( _cons.variables().has( _subs.variable() ) )
        {
            using Rational = typename Poly::NumberType;
            // Collect all necessary left hand sides to create the new conditions of all cases referring to the virtual substitution.
            if( carl::pow( Rational(Rational(_subs.term().sqrt_ex().constant_part().size()) + Rational(_subs.term().sqrt_ex().factor().size()) * Rational(_subs.term().sqrt_ex().radicand().size())), _cons.maxDegree( _subs.variable() )) > (MAX_NUM_OF_TERMS*MAX_NUM_OF_TERMS) )
            {
                return false;
            }
            carl::SqrtEx sub = carl::substitute( _cons.lhs(), _subs.variable(), _subs.term().sqrt_ex() );
            #ifdef VS_DEBUG_SUBSTITUTION
            std::cout << "Result of common substitution:" << sub << std::endl;
            #endif
            // The term then looks like:    q/s
            if( !sub.has_sqrt() )
            {
                // Create the new decision tuples.
                if( _cons.relation() == Relation::EQ || _cons.relation() == Relation::NEQ )
                {
                    // Add conjunction (sub.constant_part() = 0) to the substitution result.
                    _result.emplace_back();
                    _result.back().push_back( Constraint<Poly>( sub.constant_part(), _cons.relation() ) );
                }
                else
                {
                    if( !_subs.term().sqrt_ex().denominator().is_constant() )
                    {
                        // Add conjunction (sub.denominator()>0 and sub.constant_part() </>/<=/>= 0) to the substitution result.
                        _result.emplace_back();
                        _result.back().push_back( Constraint<Poly>( sub.denominator(), Relation::GREATER ) );
                        _result.back().push_back( Constraint<Poly>( sub.constant_part(), _cons.relation() ) );
                        // Add conjunction (sub.denominator()<0 and sub.constant_part() >/</>=/<= 0) to the substitution result.
                        Relation inverseRelation;
                        switch( _cons.relation() )
                        {
                            case Relation::LESS:
                                inverseRelation = Relation::GREATER;
                                break;
                            case Relation::GREATER:
                                inverseRelation = Relation::LESS;
                                break;
                            case Relation::LEQ:
                                inverseRelation = Relation::GEQ;
                                break;
                            case Relation::GEQ:
                                inverseRelation = Relation::LEQ;
                                break;
                            default:
                                assert( false );
                                inverseRelation = Relation::EQ;
                        }
                        _result.emplace_back();
                        _result.back().push_back( Constraint<Poly>( sub.denominator(), Relation::LESS ) );
                        _result.back().push_back( Constraint<Poly>( sub.constant_part(), inverseRelation ) );
                    }
                    else
                    {
                        // Add conjunction (f(-c/b)*b^k </>/<=/>= 0) to the substitution result.
                        _result.emplace_back();
                        _result.back().push_back( Constraint<Poly>( sub.constant_part(), _cons.relation() ) );
                    }
                }
            }
            // The term then looks like:   (q+r*sqrt(b^2-4ac))/s
            else
            {
                Poly s = Poly(1);
                if( !_subs.term().sqrt_ex().denominator().is_constant() )
                    s = sub.denominator();
                switch( _cons.relation() )
                {
                    case Relation::EQ:
                    {
                        result = substituteNormalSqrtEq( sub.radicand(), sub.constant_part(), sub.factor(), _result, _accordingPaper );
                        break;
                    }
                    case Relation::NEQ:
                    {
                        result = substituteNormalSqrtNeq( sub.radicand(), sub.constant_part(), sub.factor(), _result, _accordingPaper );
                        break;
                    }
                    case Relation::LESS:
                    {
                        result = substituteNormalSqrtLess( sub.radicand(), sub.constant_part(), sub.factor(), s, _result, _accordingPaper );
                        break;
                    }
                    case Relation::GREATER:
                    {
                        result = substituteNormalSqrtLess( sub.radicand(), sub.constant_part(), sub.factor(), -s, _result, _accordingPaper );
                        break;
                    }
                    case Relation::LEQ:
                    {
                        result = substituteNormalSqrtLeq( sub.radicand(), sub.constant_part(), sub.factor(), s, _result, _accordingPaper );
                        break;
                    }
                    case Relation::GEQ:
                    {
                        result = substituteNormalSqrtLeq( sub.radicand(), sub.constant_part(), sub.factor(), -s, _result, _accordingPaper );
                        break;
                    }
                    default:
                        std::cout << "Error in substituteNormal: Unexpected relation symbol" << std::endl;
                        assert( false );
                }
            }
        }
        else
        {
            _result.emplace_back();
            _result.back().push_back( _cons );
        }
        simplify( _result, _conflictingVariables, _solutionSpace );
        return result;
    }

    template<typename Poly>
    bool substituteNormalSqrtEq( const Poly& _radicand,
                                 const Poly& _q,
                                 const Poly& _r,
                                 CaseDistinction<Poly>& _result,
                                 bool _accordingPaper )
    {
        if( _q.size() > MAX_NUM_OF_TERMS || _r.size() > MAX_NUM_OF_TERMS || _radicand.size() > MAX_NUM_OF_TERMS )
            return false;
        Poly lhs = carl::pow(_q, 2) - carl::pow(_r, 2) * _radicand;
        if( _accordingPaper )
        {
            Poly qr = _q * _r;
            // Add conjunction (q*r<=0 and q^2-r^2*radicand=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( qr, Relation::LEQ ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::EQ ) );
        }
        else
        {
            // Add conjunction (q=0 and r=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::EQ ) );
            _result.back().push_back( Constraint<Poly>( _r, Relation::EQ ) );
            // Add conjunction (q=0 and radicand=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::EQ ) );
            _result.back().push_back( Constraint<Poly>( _radicand, Relation::EQ ) );
            // Add conjunction (q<0 and r>0 and q^2-r^2*radicand=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( _r, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::EQ ) );
            // Add conjunction (q>0 and r<0 and q^2-r^2*radicand=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( _r, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::EQ ) );
        }
        return true;
    }

    template<typename Poly>
    bool substituteNormalSqrtNeq( const Poly& _radicand,
                                  const Poly& _q,
                                  const Poly& _r,
                                  CaseDistinction<Poly>& _result,
                                  bool _accordingPaper )
    {
        if( _q.size() > MAX_NUM_OF_TERMS || _r.size() > MAX_NUM_OF_TERMS || _radicand.size() > MAX_NUM_OF_TERMS )
            return false;
        Poly lhs = carl::pow(_q, 2) - carl::pow(_r, 2) * _radicand;
        if( _accordingPaper )
        {
            Poly qr = _q * _r;
            // Add conjunction (q*r>0 and q^2-r^2*radicand!=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( qr, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::NEQ ) );
        }
        else
        {
            // Add conjunction (q>0 and r>0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( _r, Relation::GREATER ) );
            // Add conjunction (q<0 and r<0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( _r, Relation::LESS ) );
            // Add conjunction (q^2-r^2*radicand!=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( lhs, Relation::NEQ ) );
        }
        return true;
    }

    template<typename Poly>
    bool substituteNormalSqrtLess( const Poly& _radicand,
                                   const Poly& _q,
                                   const Poly& _r,
                                   const Poly& _s,
                                   CaseDistinction<Poly>& _result,
                                   bool _accordingPaper )
    {
        if( _q.size() > MAX_NUM_OF_TERMS || _r.size() > MAX_NUM_OF_TERMS || _radicand.size() > MAX_NUM_OF_TERMS )
            return false;
        Poly lhs = carl::pow(_q, 2) - carl::pow(_r, 2) * _radicand;
        if( _accordingPaper )
        {
            Poly qs = _q * _s;
            Poly rs = _r * _s;
            // Add conjunction (q*s<0 and q^2-r^2*radicand>0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( qs, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::GREATER ) );
            // Add conjunction (r*s<=0 and q*s<0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( rs, Relation::LEQ ) );
            _result.back().push_back( Constraint<Poly>( qs, Relation::LESS ) );
            // Add conjunction (r*s<=0 and q^2-r^2*radicand<0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( rs, Relation::LEQ ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::LESS ) );
        }
        else
        {
            // Add conjunction (q<0 and s>0 and q^2-r^2*radicand>0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::GREATER ) );
            // Add conjunction (q>0 and s<0 and q^2-r^2*radicand>0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::GREATER ) );
            // Add conjunction (r>0 and s<0 and q^2-r^2*radicand<0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _r, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::LESS ) );
            // Add conjunction (r<0 and s>0 and q^2-r^2*radicand<0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _r, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::LESS ) );
            // Add conjunction (r>=0 and q<0 and s>0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _r, Relation::GEQ ) );
            _result.back().push_back( Constraint<Poly>( _q, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::LESS ) );
            // Add conjunction (r<=0 and q>0 and s<0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _r, Relation::LEQ ) );
            _result.back().push_back( Constraint<Poly>( _q, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::GREATER ) );
        }
        return true;
    }

    template<typename Poly>
    bool substituteNormalSqrtLeq( const Poly& _radicand,
                                  const Poly& _q,
                                  const Poly& _r,
                                  const Poly& _s,
                                  CaseDistinction<Poly>& _result,
                                  bool _accordingPaper )
    {
        if( _q.size() > MAX_NUM_OF_TERMS || _r.size() > MAX_NUM_OF_TERMS || _radicand.size() > MAX_NUM_OF_TERMS )
            return false;
        Poly lhs = carl::pow(_q, 2) - carl::pow(_r, 2) * _radicand;
        if( _accordingPaper )
        {
            Poly qs = _q * _s;
            Poly rs = _r * _s;
            // Add conjunction (q*s<=0 and q^2-r^2*radicand>=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( qs, Relation::LEQ ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::GEQ ) );
            // Add conjunction (r*s<=0 and q^2-r^2*radicand<=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( rs, Relation::LEQ ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::LEQ ) );
        }
        else
        {
            // Add conjunction (q<0 and s>0 and q^2-r^2*radicand>=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::GEQ ) );
            // Add conjunction (q>0 and s<0 and q^2-r^2*radicand>=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _q, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::GEQ ) );
            // Add conjunction (r>0 and s<0 and q^2-r^2*radicand<=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _r, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::LEQ ) );
            // Add conjunction (r<0 and s>0 and q^2-r^2*radicand<=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _r, Relation::LESS ) );
            _result.back().push_back( Constraint<Poly>( _s, Relation::GREATER ) );
            _result.back().push_back( Constraint<Poly>( lhs, Relation::LEQ ) );
            // Add conjunction (r=0 and q=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _r, Relation::EQ ) );
            _result.back().push_back( Constraint<Poly>( _q, Relation::EQ ) );
            // Add conjunction (radicand=0 and q=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _radicand, Relation::EQ ) );
            _result.back().push_back( Constraint<Poly>( _q, Relation::EQ ) );
        }
        return true;
    }

    template<typename Poly>
    bool substitutePlusEps( const Constraint<Poly>& _cons,
                            const Substitution<Poly>& _subs,
                            CaseDistinction<Poly>& _result,
                            bool _accordingPaper,
                            Variables& _conflictingVariables,
                            const detail::EvalDoubleIntervalMap& _solutionSpace )
    {
        bool result = true;
        auto vars = variables(_cons);
        if( !vars.empty() )
        {
            if( vars.has( _subs.variable() ) )
            {
                switch( _cons.relation() )
                {
                    case Relation::EQ:
                    {
                        substituteTrivialCase( _cons, _subs, _result );
                        break;
                    }
                    case Relation::NEQ:
                    {
                        substituteNotTrivialCase( _cons, _subs, _result );
                        break;
                    }
                    case Relation::LESS:
                    {
                        result = substituteEpsGradients( _cons, _subs, Relation::LESS, _result, _accordingPaper, _conflictingVariables, _solutionSpace );
                        break;
                    }
                    case Relation::GREATER:
                    {
                        result = substituteEpsGradients( _cons, _subs, Relation::GREATER, _result, _accordingPaper, _conflictingVariables, _solutionSpace );
                        break;
                    }
                    case Relation::LEQ:
                    {
                        substituteTrivialCase( _cons, _subs, _result );
                        result = substituteEpsGradients( _cons, _subs, Relation::LESS, _result, _accordingPaper, _conflictingVariables, _solutionSpace );
                        break;
                    }
                    case Relation::GEQ:
                    {
                        substituteTrivialCase( _cons, _subs, _result );
                        result = substituteEpsGradients( _cons, _subs, Relation::GREATER, _result, _accordingPaper, _conflictingVariables, _solutionSpace );
                        break;
                    }
                    default:
                        assert( false );
                }
                simplify( _result, _conflictingVariables, _solutionSpace );
            }
            else
            {
                _result.emplace_back();
                _result.back().push_back( _cons );
            }
        }
        else
        {
            assert( false );
            std::cerr << "Warning in substitutePlusEps: The chosen constraint has no variable" << std::endl;
        }
        return result;
    }

    template<typename Poly>
    bool substituteEpsGradients( const Constraint<Poly>& _cons,
                                 const Substitution<Poly>& _subs,
                                 const Relation _relation,
                                 CaseDistinction<Poly>& _result,
                                 bool _accordingPaper,
                                 Variables& _conflictingVariables,
                                 const detail::EvalDoubleIntervalMap& _solutionSpace )
    {
        assert( _cons.variables().has( _subs.variable() ) );
        // Create a substitution formed by the given one without an addition of epsilon.
        auto term = Term<Poly>::normal(_subs.term().sqrt_ex());
        // Call the method substituteNormal with the constraint f(x)~0 and the substitution [x -> t],  where the parameter relation is ~.
        Constraint<Poly> firstCaseInequality = Constraint<Poly>( _cons.lhs(), _relation );
        if( !substituteNormal( firstCaseInequality, {_subs.variable(), term}, _result, _accordingPaper, _conflictingVariables, _solutionSpace ) )
            return false;
        // Create a vector to store the results of each single substitution.
        std::vector<CaseDistinction<Poly>> substitutionResultsVector;
        /*
         * Let k be the maximum degree of x in f, then call for every 1<=i<=k substituteNormal with:
         *
         *      f^0(x)=0 and ... and f^i-1(x)=0 and f^i(x)~0     as constraints and
         *      [x -> t]                                         as substitution,
         *
         * where the relation is ~.
         */
        Poly deriv = Poly( _cons.lhs() );
        while( deriv.has( _subs.variable() ) )
        {
            // Change the relation symbol of the last added constraint to "=".
            Constraint<Poly> equation = Constraint<Poly>( deriv, Relation::EQ );
            // Form the derivate of the left hand side of the last added constraint.
            deriv = carl::derivative(deriv, _subs.variable(), 1);
            // Add the constraint f^i(x)~0.
            Constraint<Poly> inequality = Constraint<Poly>( deriv, _relation );
            // Apply the substitution (without epsilon) to the new constraints.
            substitutionResultsVector.emplace_back();
            if( !substituteNormal( equation, {_subs.variable(), term}, substitutionResultsVector.back(), _accordingPaper, _conflictingVariables, _solutionSpace ) )
                return false;
            substitutionResultsVector.emplace_back();
            if( !substituteNormal( inequality, {_subs.variable(), term}, substitutionResultsVector.back(), _accordingPaper, _conflictingVariables, _solutionSpace ) )
                return false;
            if( !combine( substitutionResultsVector, _result ) )
                return false;
            simplify( _result, _conflictingVariables, _solutionSpace );
            // Remove the last substitution result.
            substitutionResultsVector.pop_back();
        }
        return true;
    }

    template<typename Poly>
    void substituteInf( const Constraint<Poly>& _cons, const Substitution<Poly>& _subs, CaseDistinction<Poly>& _result, Variables& _conflictingVariables, const detail::EvalDoubleIntervalMap& _solutionSpace )
    {
        auto vars = variables(_cons);
        if( !vars.empty() )
        {
            if( vars.has( _subs.variable() ) )
            {
                if( _cons.relation() == Relation::EQ )
                    substituteTrivialCase( _cons, _subs, _result );
                else if( _cons.relation() == Relation::NEQ )
                    substituteNotTrivialCase( _cons, _subs, _result );
                else
                             
                    substituteInfLessGreater( _cons, _subs, _result );
                simplify( _result, _conflictingVariables, _solutionSpace );
            }
            else
            {
                _result.emplace_back();
                _result.back().push_back( _cons );
            }
        }
        else
            std::cout << "Warning in substituteInf: The chosen constraint has no variable" << std::endl;
    }

    template<typename Poly>
    void substituteInfLessGreater( const Constraint<Poly>& _cons, const Substitution<Poly>& _subs, CaseDistinction<Poly>& _result )
    {
        assert( _cons.relation() != Relation::EQ );
        assert( _cons.relation() != Relation::NEQ );
        // Determine the relation for the coefficients of the odd and even degrees.
        Relation oddRelationType  = Relation::GREATER;
        Relation evenRelationType = Relation::LESS;
        if( _subs.term().is_minus_infty())
        {
            if( _cons.relation() == Relation::GREATER || _cons.relation() == Relation::GEQ )
            {
                oddRelationType  = Relation::LESS;
                evenRelationType = Relation::GREATER;
            }
        }
        else
        {
            assert( _subs.term().is_plus_infty() );
            if( _cons.relation() == Relation::LESS || _cons.relation() == Relation::LEQ )
            {
                oddRelationType  = Relation::LESS;
                evenRelationType = Relation::GREATER;
            }
        }
        // Check all cases according to the substitution rules.
        carl::uint varDegree = _cons.maxDegree( _subs.variable() );
        assert( varDegree > 0 );
        for( carl::uint i = varDegree + 1; i > 0; --i )
        {
            // Add conjunction (a_n=0 and ... and a_i~0) to the substitution result.
            _result.emplace_back();
            for( carl::uint j = varDegree; j > i - 1; --j )
                _result.back().push_back( Constraint<Poly>( _cons.coefficient( _subs.variable(), j ), Relation::EQ ) );
            if( i > 1 )
            {
                if( fmod( i - 1, 2.0 ) != 0.0 )
                    _result.back().push_back( Constraint<Poly>( _cons.coefficient( _subs.variable(), i - 1 ), oddRelationType ) );
                else
                    _result.back().push_back( Constraint<Poly>( _cons.coefficient( _subs.variable(), i - 1 ), evenRelationType ) );
            }
            else
                _result.back().push_back( Constraint<Poly>( _cons.coefficient( _subs.variable(), i - 1 ), _cons.relation() ) );
        }
    }

    template<typename Poly>
    void substituteTrivialCase( const Constraint<Poly>& _cons, const Substitution<Poly>& _subs, CaseDistinction<Poly>& _result )
    {
        assert( _cons.relation() == Relation::EQ || _cons.relation() == Relation::LEQ || _cons.relation() == Relation::GEQ );
        carl::uint varDegree = _cons.maxDegree( _subs.variable() );
        // Check the cases (a_0=0 and ... and a_n=0)
        _result.emplace_back();
        for( carl::uint i = 0; i <= varDegree; ++i )
            _result.back().push_back( Constraint<Poly>( _cons.coefficient( _subs.variable(), i ), Relation::EQ ) );
    }

    template<typename Poly>
    void substituteNotTrivialCase( const Constraint<Poly>& _cons, const Substitution<Poly>& _subs, CaseDistinction<Poly>& _result )
    {
        assert( _cons.relation() == Relation::NEQ );
        carl::uint varDegree = _cons.maxDegree( _subs.variable() );
        for( carl::uint i = 0; i <= varDegree; ++i )
        {
            // Add conjunction (a_i!=0) to the substitution result.
            _result.emplace_back();
            _result.back().push_back( Constraint<Poly>( _cons.coefficient( _subs.variable(), i ), Relation::NEQ ) );
        }
    }
}    // end namspace vs
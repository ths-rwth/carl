/**
 * @file FormulaPool.tpp
 *
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @version 2013-10-21
 */

#include "FormulaPool.h"

namespace carl
{
    template<typename Pol>
    FormulaPool<Pol>::FormulaPool( unsigned _capacity ):
        Singleton<FormulaPool<Pol>>(),
        mIdAllocator( 3 ),
        mPoolBuckets(new typename underlying_set::bucket_type[mRehashPolicy.numBucketsFor(_capacity)]),
	    mPool(typename underlying_set::bucket_traits(mPoolBuckets.get(), mRehashPolicy.numBucketsFor(_capacity))),
        mTseitinVars(),
        mTseitinVarToFormula()
    {
		VariablePool::getInstance();
        mpTrue = new FormulaContent<Pol>( TRUE, 1 );
        mpFalse = new FormulaContent<Pol>( FALSE, 2 );
        mpTrue->mNegation = mpFalse;
     	mpFalse->mNegation = mpTrue;
        mPool.insert( *mpTrue );
        mPool.insert( *mpFalse );
        Formula<Pol>::init( *mpTrue );
        Formula<Pol>::init( *mpFalse );
        mpTrue->mUsages = 2; // avoids deleting it
        mpFalse->mUsages = 2; // avoids deleting it
    }
    
    template<typename Pol>
    FormulaPool<Pol>::~FormulaPool()
    {
        // assert( mPool.size() == 2 );
        mPool.clear();
        delete mpTrue;
        delete mpFalse;
    }
    
    template<typename Pol>
    const FormulaContent<Pol>* FormulaPool<Pol>::add( FormulaContent<Pol>&& _element )
    {
        assert( _element.mType != FormulaType::NOT );
        FORMULA_POOL_LOCK_GUARD

        typename underlying_set::insert_commit_data insert_data;
	    auto res = mPool.insert_check(_element, /*content_hash(), content_equal(),*/ insert_data);
        if( res.second ) // Formula has not yet been generated.
        {
            auto cont = new FormulaContent<Pol>(std::move(_element));
			// Add also the negation of the formula to the pool in order to ensure that it
            // has the next id and hence would occur next to the formula in a set of sub-formula,
            // which is sorted by the ids. 
            cont->mId = mIdAllocator;
            Formula<Pol>::init( *cont );
            ++mIdAllocator;
            mPool.insert_commit(*cont, insert_data);
		    check_rehash();

            auto negation = createNegatedContent(cont);
            cont->mNegation = negation;
            negation->mId = mIdAllocator; 
            negation->mNegation = cont;
            Formula<Pol>::init( *negation );
            ++mIdAllocator;
            assert(mPool.find(*negation) == mPool.end());
            //auto res2 = mPool.insert(*negation);
            //assert(res2.second);
			CARL_LOG_DEBUG("carl.formula", "Added " << cont << " / " << negation << " to pool");
            return cont;
        } else {
			CARL_LOG_TRACE("carl.formula", "Found " << static_cast<const void*>(&*res.first) << " in pool");
            return &*res.first;
		}
        
    }
    
    template<typename Pol>
    bool FormulaPool<Pol>::formulasInverse( const Formula<Pol>& _subformulaA, const Formula<Pol>& _subformulaB )
    {
		return _subformulaA.negated() == _subformulaB;
    }
    
    template<typename Pol>
    const FormulaContent<Pol>* FormulaPool<Pol>::createImplication(Formulas<Pol>&& _subformulas) {
        assert(_subformulas.size() >= 2);
        #ifdef SIMPLIFY_FORMULA
        // Conclusion
        if (_subformulas.back().mpContent == mpTrue)
            return create(TRUE);
        if (_subformulas.back().mpContent == mpFalse) {
            _subformulas.pop_back();
            for (auto& f: _subformulas) {
                f = Formula<Pol>(NOT, f);
            }
            return create(OR, std::move(_subformulas));
        }
        #endif
        Formula<Pol> conclusion = _subformulas.back();
        _subformulas.pop_back();
        #ifdef SIMPLIFY_FORMULA
        // Premises
        for (auto it = _subformulas.begin(); it != _subformulas.end(); ) {
            if (it->mpContent == mpFalse) return create(TRUE);
            if (it->mpContent == mpTrue) {
                auto jt = it;
                ++jt;
                if( jt != _subformulas.end() )
                    *it = _subformulas.back();
                _subformulas.pop_back();
            }
            else
                ++it;
        }
        #endif
        if (_subformulas.empty()) {
            return conclusion.mpContent;
        }
        Formula<Pol> premise(AND, std::move(_subformulas));
        return add(FormulaContent<Pol>(IMPLIES, {premise, conclusion}));
    }
    
    template<typename Pol>
    const FormulaContent<Pol>* FormulaPool<Pol>::createNAry(FormulaType _type, Formulas<Pol>&& _subformulas)
    {
        assert( _type == FormulaType::AND || _type == FormulaType::OR || _type == FormulaType::XOR || _type == FormulaType::IFF );
//        std::cout << __func__ << _type;
//        for( const auto& f : _subformulas )
//            std::cout << " " << f;
//        std::cout << std::endl;
        if( _subformulas.size() == 1 )
        {
			CARL_LOG_TRACE("carl.formula", "Only a single subformula, eliminating " << _type);
            return _subformulas[0].mpContent;
        }
        if( _type != FormulaType::IFF )
        {
            for( size_t pos = 0; pos < _subformulas.size(); )
            {
                if( _subformulas[pos].getType() == _type )
                {
                    // We have (op .. (op a1 .. an) b ..), so create (op .. a1 .. an b ..) instead.
                    // Note, that a1 to an are definitely created before b, as they were sub-formulas
                    // of it, hence, the ids of a1 to an are smaller than the one of b and therefore a1<b .. an<b.
                    // That means, that a1 .. an are inserted into the given set of sub formulas before the position of
                    // b (=iter).
                    // Note also that the operator of a1 to an cannot be oper, as they where also created with this pool.
                    Formula<Pol> tmp = _subformulas[pos];
                    _subformulas[pos] = _subformulas.back();
                    _subformulas.pop_back();
                    _subformulas.insert(_subformulas.end(), tmp.subformulas().begin(), tmp.subformulas().end() );
                }
                else
                    ++pos;
            }
        }
        std::sort( _subformulas.begin(), _subformulas.end() );
        std::vector<Formula<Pol>> subformulas;
        subformulas.reserve( _subformulas.size() );
        bool negateResult = false;
        size_t pos = 0;
        while( pos < _subformulas.size() && _subformulas[pos].isTrue() )
        {
            switch( _type )
            {
                case FormulaType::XOR:
                    negateResult = !negateResult;
                    break;
                case FormulaType::OR:
                    return trueFormula();
                default:
                    assert( _type == FormulaType::AND || _type == FormulaType::IFF );
            }
            ++pos;
        }
        while( pos < _subformulas.size() && _subformulas[pos].isFalse() )
        {
            switch( _type )
            {
                case FormulaType::IFF:
                    if( _subformulas[0].isTrue() )
                        return falseFormula();
                    negateResult = true;
                    break;
                case FormulaType::AND:
                    return falseFormula();
                default:
                    assert( _type == FormulaType::OR || _type == FormulaType::XOR );
            }
            ++pos;   
        }
        for( ; pos < _subformulas.size(); )
        {
            if( pos < _subformulas.size() - 1 )
            {
                if( _subformulas[pos] == _subformulas[pos+1] )
                {
                    size_t numOfEqualSubformulas = pos;
                    ++pos;
                    while( pos < _subformulas.size() - 1 && _subformulas[pos] == _subformulas[pos+1] )
                        ++pos;
                    if( _type == FormulaType::XOR && (pos + 1 - numOfEqualSubformulas) % 2 == 0 )
                        ++pos;
                }
                else if( formulasInverse( _subformulas[pos], _subformulas[pos+1] ) )
                {
                    // Check if the sub-formula at pos is the negation of the sub-formula at pos+1
                    // Note, that the negation of a formula would by construction always be right after the formula
                    // in a set of formulas whose comparison operator is based on the one of formulas This is due to
                    // them comparing just the ids and we construct the negation of a formula right after the formula
                    // itself and assign the next id to it.
                    switch( _type )
                    {
                        case FormulaType::AND:
                            return falseFormula();
                        case FormulaType::OR:
                            return trueFormula();
                        case FormulaType::IFF:
                            return falseFormula();
                        default:
                            assert( _type == FormulaType::XOR );
                            negateResult = !negateResult;
                    }
                    ++pos;
                    ++pos;
                }
                else
                {
                    subformulas.push_back( _subformulas[pos] );
                    ++pos;
                }
            }
            else
            {
                subformulas.push_back( _subformulas[pos] );
                ++pos;
            }
        }
        if( subformulas.empty() )
        {
            if( negateResult || _type == FormulaType::AND || _type == FormulaType::IFF )
                return trueFormula();
            return falseFormula();
        }
        const FormulaContent<Pol>* result;
        if( subformulas.size() == 1 )
        {
            if( _type == FormulaType::IFF && _subformulas[0] == *subformulas.begin() )
                return trueFormula();
            result = subformulas.begin()->mpContent;
        }
        else
        {
            result = add(FormulaContent<Pol>( _type, std::move( subformulas ) ) );
        }
        return negateResult ? result->mNegation : result;
    }
    
    template<typename Pol>
    const FormulaContent<Pol>* FormulaPool<Pol>::createITE(Formulas<Pol>&& _subformulas) {
        assert(_subformulas.size() == 3);
        #ifdef SIMPLIFY_FORMULA
        Formula<Pol>& condition = _subformulas[0];
        Formula<Pol>& thencase = _subformulas[1];
        Formula<Pol>& elsecase = _subformulas[2];
        
        if (condition.isTrue()) return thencase.mpContent;
        if (condition.isFalse()) return elsecase.mpContent;
        if (thencase == elsecase) return thencase.mpContent;
        
        if (condition.getType() == FormulaType::NOT) {
            _subformulas[0] = condition.subformula();
            std::swap(_subformulas[1], _subformulas[2]);
            return createITE(std::move(_subformulas));
        }
        if (condition == elsecase) elsecase = Formula<Pol>(falseFormula());
        if (condition.mpContent == elsecase.mpContent->mNegation) elsecase = Formula<Pol>(trueFormula());
        if (condition == thencase) thencase = Formula<Pol>(trueFormula());
        if (condition.mpContent == thencase.mpContent->mNegation) thencase = Formula<Pol>(falseFormula());
        
        if (thencase.isFalse()) {
            // (ite c false b) = (~c or false) and (c or b) = ~c and (c or b) = (~c and b)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(Formula<Pol>(FormulaType::NOT, condition));
            subFormulas.push_back(elsecase);
            return create(FormulaType::AND, std::move(subFormulas));
        }
        if (thencase.isTrue()) {
            // (ite c true b) = (~c or true) and (c or b) = (c or b)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(condition);
            subFormulas.push_back(elsecase);
            return create(FormulaType::OR, std::move(subFormulas));
        }
        if (elsecase.isFalse()) {
            // (ite c false b) = (~c or a) and (c or false) = (~c or a) and c = (c and a)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(condition);
            subFormulas.push_back(thencase);
            return create(FormulaType::AND, std::move(subFormulas));
        }
        if (elsecase.isTrue()) {
            // (ite c true b) = (~c or a) and (c or true) = (~c or a)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(Formula<Pol>(FormulaType::NOT, condition));
            subFormulas.push_back(thencase);
            return create(FormulaType::OR, std::move(subFormulas));
        }
        #endif
        return add(FormulaContent<Pol>(ITE, std::move(_subformulas)));
    }
    
}    // namespace carl

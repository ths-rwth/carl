/**
 * @file FormulaPool.tpp
 *
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @version 2013-10-21
 */

#include "FormulaPool.h"

using namespace std;

namespace carl
{
    template<typename Pol>
    FormulaPool<Pol>::FormulaPool( unsigned _capacity ):
        Singleton<FormulaPool<Pol>>(),
        mIdAllocator( 3 ),
        mpTrue( new FormulaContent<Pol>( TRUE, 1 ) ),
        mpFalse( new FormulaContent<Pol>( FALSE, 2 ) ),
        mPool()
    {
        ConstraintPool<Pol>::getInstance();
        mpTrue->mNegation = mpFalse;
     	mpFalse->mNegation = mpTrue;
        mPool.reserve( _capacity );
        mPool.insert( mpTrue );
        mPool.insert( mpFalse );
        Formula<Pol>::init( *mpTrue );
        Formula<Pol>::init( *mpFalse );
        mpTrue->mUsages = 2; // avoids deleting it
        mpFalse->mUsages = 2; // avoids deleting it
    }
    
    template<typename Pol>
    FormulaPool<Pol>::~FormulaPool()
    {
        mPool.clear();
        delete mpTrue;
        delete mpFalse;
    }
    
    template<typename Pol>
    std::pair<typename FastPointerSet<FormulaContent<Pol>>::iterator,bool> FormulaPool<Pol>::insert( FormulaContent<Pol>* _element )
    {
        auto iterBoolPair = mPool.insert( _element );
        if( !iterBoolPair.second ) // Formula has already been generated.
        {
            delete _element;
        }
        return iterBoolPair;
    }
    
    template<typename Pol>
    const FormulaContent<Pol>* FormulaPool<Pol>::add( FormulaContent<Pol>* _element )
    {
        assert( _element->mType != FormulaType::NOT );
        FORMULA_POOL_LOCK_GUARD
        auto iterBoolPair = this->insert( _element );
        if( iterBoolPair.second ) // Formula has not yet been generated.
        {
            // Add also the negation of the formula to the pool in order to ensure that it
            // has the next id and hence would occur next to the formula in a set of sub-formula,
            // which is sorted by the ids.
            _element->mId = mIdAllocator; 
            Formula<Pol>::init( *_element );
            ++mIdAllocator;
            FormulaContent<Pol>* negation = new FormulaContent<Pol>(NOT, std::move( Formula<Pol>( *iterBoolPair.first ) ) );
            _element->mNegation = negation;
            negation->mId = mIdAllocator; 
            negation->mNegation = _element;
            Formula<Pol>::init( *negation );
            ++mIdAllocator;
        }
        return *iterBoolPair.first;
    }
    
    template<typename Pol>
    bool FormulaPool<Pol>::formulasInverse( const Formula<Pol>& _subformulaA, const Formula<Pol>& _subformulaB )
    {
        if( _subformulaA.mpContent == mpTrue && _subformulaB.mpContent == mpFalse )
            return true;
        if( _subformulaB.getType() == FormulaType::NOT && _subformulaB.subformula() == _subformulaA )
            return true;
        return false;
    }
    
    template<typename Pol>
    const FormulaContent<Pol>* FormulaPool<Pol>::createImplication(Formulas<Pol>&& _subformulas) {
        assert(_subformulas.size() >= 2);
        #ifdef SIMPLIFY_FORMULA
        auto it = _subformulas.rbegin();
        // Conclusion
        if (it->mpContent == mpTrue) return create(TRUE);
        if (it->mpContent == mpFalse) {
            _subformulas.pop_back();
            for (auto& f: _subformulas) {
                f = Formula<Pol>(NOT, f);
            }
            return create(OR, std::move(_subformulas));
        }
        // Premises
        for (it++; it != _subformulas.rend(); ) {
            if (it->mpContent == mpFalse) return create(TRUE);
            if (it->mpContent == mpTrue) _subformulas.erase(it.base());
            it++;
        }
        #endif
        Formula<Pol> conclusion = _subformulas.back();
        _subformulas.pop_back();
        Formula<Pol> premise(AND, std::move(_subformulas));
        return add(new FormulaContent<Pol>(IMPLIES, {premise, conclusion}));
    }
    
    template<typename Pol>
    const FormulaContent<Pol>* FormulaPool<Pol>::createNAry(FormulaType _type, Formulas<Pol>&& _subformulas)
    {
        assert(_type == FormulaType::AND || _type == FormulaType::OR || _type == FormulaType::XOR || _type == FormulaType::IFF);
        //cout << "create new formula with type " << _type << endl;
        //for( auto f : _subformulas )
        //    cout << "\t" << f << endl;
        if (_subformulas.size() == 1) {
            return _subformulas[0].mpContent;
        }
        for( size_t pos = 0; pos < _subformulas.size(); )
        {
            if( _subformulas[pos].getType() == _type && (_type == FormulaType::AND || _type == FormulaType::OR) )
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
            {
                // Check if the sub-formula at iter is the negation of the sub-formula at iterB
                // Note, that the negation of a formula would by construction always be right after the formula
                // in a set of formulas whose comparison operator is based on the one of formulas This is due to
                // them comparing just the ids and we construct the negation of a formula right after the formula
                // itself and assign the next id to it.
                if( pos < _subformulas.size() - 1 && formulasInverse( _subformulas[pos], _subformulas[pos+1] ) )
                {
                    switch( _type )
                    {
                        case FormulaType::AND:
                        {
                            return falseFormula();
                        }
                        case FormulaType::OR:
                        {
                            return trueFormula();
                        }
                        case FormulaType::IFF:
                        {
                            return falseFormula();
                        }
                        case FormulaType::XOR:
                        {
                            _subformulas[pos] = Formula<Pol>( trueFormula() );
                            ++pos;
                            _subformulas[pos] = _subformulas.back();
                            _subformulas.pop_back();
                            break;
                        }
                        default:
                        {
                            assert( false );
                            break;
                        }
                    }
                }
                else
                {
                    ++pos;
                }
            }
        }
        std::sort(_subformulas.begin(), _subformulas.end());
        _subformulas.erase(std::unique(_subformulas.begin(), _subformulas.end()), _subformulas.end());
        if( _subformulas.empty() )
            return falseFormula();
        else
        {
            #ifdef SIMPLIFY_FORMULA
            if( _type == FormulaType::AND || _type == FormulaType::OR || _type == FormulaType::IFF )
            {
                auto iterToTrue = _subformulas.begin();
                auto iterToFalse = _subformulas.begin();
                if( iterToTrue->mpContent == mpTrue )
                {
                    ++iterToFalse;
                    if( iterToFalse != _subformulas.end() && iterToFalse->mpContent != mpFalse )
                        iterToFalse = _subformulas.end();
                }
                else
                {
                    iterToTrue = _subformulas.end();
                    if( iterToFalse->mpContent != mpFalse )
                        iterToFalse = _subformulas.end();
                }
                if( _type == FormulaType::AND )
                {
                    if( iterToFalse != _subformulas.end() ) return falseFormula();
                    if( iterToTrue != _subformulas.end() ) _subformulas.erase( iterToTrue );
                    else if( _subformulas.empty() ) return trueFormula();
                }
                else if( _type == FormulaType::OR )
                {
                    if( iterToTrue != _subformulas.end() ) return trueFormula();
                    if( iterToFalse != _subformulas.end() ) _subformulas.erase( iterToFalse );
                    else if( _subformulas.empty() ) return falseFormula();
                }
                else // _type == FormulaType::IFF
                {
                    if( iterToFalse != _subformulas.end() && iterToTrue != _subformulas.end() )
                    {
                        return falseFormula();
                    }
                    else if( iterToFalse != _subformulas.end() )
                    {
                        if( _subformulas.size() == 2 )
                        {
                            if( iterToFalse == _subformulas.begin() )
                                return (++iterToFalse)->mpContent->mNegation;
                            return _subformulas.begin()->mpContent->mNegation;
                        }
                    }
                    else if( iterToTrue != _subformulas.end() )
                    {
                        if( _subformulas.size() == 2 )
                        {
                            if( iterToTrue == _subformulas.begin() )
                                return (++iterToTrue)->mpContent;
                            return _subformulas.begin()->mpContent;
                        }
                    }
                }
            }
            #endif
            if( _subformulas.size() == 1 )
                return newFormulaWithOneSubformula( _type, *(_subformulas.begin()) );
        }
        return add( new FormulaContent<Pol>( _type, std::move( _subformulas ) ) );
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
            std::swap(_subformulas[1], _subformulas[2]);
            return createITE(std::move(_subformulas));
        }
        if (condition == elsecase) elsecase = falseFormula();
        if (condition == elsecase.mpContent->mNegation) elsecase = trueFormula();
        if (condition == thencase) thencase = trueFormula();
        if (condition == thencase.mpContent->mNegation) thencase = falseFormula();
        
        if (thencase.isFalse()) {
            // (ite c false b) = (~c or false) and (c or b) = ~c and (c or b) = (~c and b)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(Formula<Pol>(FormulaType::NOT, condition));
            subFormulas.push_back(elsecase);
            return add(new FormulaContent<Pol>(FormulaType::AND, std::move(subFormulas)));
        }
        if (thencase.isTrue()) {
            // (ite c false b) = (~c or true) and (c or b) = (c or b)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(condition);
            subFormulas.push_back(elsecase);
            return add(new FormulaContent<Pol>(FormulaType::OR, std::move(subFormulas)));
        }
        if (elsecase.isFalse()) {
            // (ite c false b) = (~c or a) and (c or false) = (~c or a) and c = (c and a)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(condition);
            subFormulas.push_back(thencase);
            return add(new FormulaContent<Pol>(FormulaType::AND, std::move(subFormulas)));
        }
        if (elsecase.isTrue()) {
            // (ite c false b) = (~c or a) and (c or true) = (~c or a)
            Formulas<Pol> subFormulas;
            subFormulas.push_back(Formula<Pol>(FormulaType::NOT, condition));
            subFormulas.push_back(thencase);
            return add(new FormulaContent<Pol>(FormulaType::OR, std::move(subFormulas)));
        }
        #endif
        return add(new FormulaContent<Pol>(ITE, std::move(_subformulas)));
    }
    
}    // namespace carl

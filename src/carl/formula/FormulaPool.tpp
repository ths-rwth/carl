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
        mpTrue( new Element( true, 1 ) ),
        mpFalse( new Element( false, 2 ) ),
        mPool()
    {
        mPool.reserve( _capacity );
        mPool.insert( mpTrue );
        mPool.insert( mpFalse );
        Formula<Pol>::init( *mpTrue );
        Formula<Pol>::init( *mpFalse );
    }
    
    template<typename Pol>
    FormulaPool<Pol>::~FormulaPool()
    {
        while( !mPool.empty() )
        {
            const Element* ele = *mPool.begin();
            mPool.erase( mPool.begin() );
            delete ele;
        }
    }
    
    template<typename Pol>
    std::pair<typename FastPointerSet<typename FormulaPool<Pol>::Element>::iterator,bool> FormulaPool<Pol>::insert( ElementSPtr _element, bool _elementNotInPool )
    {
        auto iterBoolPair = mPool.insert( _element );
        assert( _elementNotInPool <= iterBoolPair.second );
        if( _elementNotInPool || iterBoolPair.second ) // Formula has not yet been generated.
        {
            _element->mId = mIdAllocator; // id should be set here to avoid conflicts when multi-threading
            Formula<Pol>::init( *_element );
            ++mIdAllocator;
        }
        else
        {
            delete _element;
        }
        return iterBoolPair;
    }
    
    template<typename Pol>
    typename FormulaPool<Pol>::ConstElementSPtr FormulaPool<Pol>::add( ElementSPtr _element )
    {
        FORMULA_POOL_LOCK_GUARD
        auto iterBoolPair = insert( _element, false );
        if( iterBoolPair.second ) // Formula has not yet been generated.
        {
            // Add also the negation of the formula to the pool in order to ensure that it
            // has the next id and hence would occur next to the formula in a set of sub-formula,
            // which is sorted by the ids.
            insert( new Element( Formula<Pol>( *iterBoolPair.first ) ), true );
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
    typename FormulaPool<Pol>::ConstElementSPtr FormulaPool<Pol>::create( FormulaType _type, std::set<Formula<Pol>>&& _subformulas )
    {
        assert( _type == FormulaType::AND || _type == FormulaType::OR || _type == FormulaType::XOR || _type == FormulaType::IFF );
//        cout << "create new formula with type " << formulaTypeToString( _type ) << endl;
//        for( auto f : _subformulas )
//            cout << *f << endl;
        for( auto iter = _subformulas.begin(); iter != _subformulas.end(); )
        {
            if( iter->getType() == _type && (_type == FormulaType::AND || _type == FormulaType::OR) )
            {
                // We have (op .. (op a1 .. an) b ..), so create (op .. a1 .. an b ..) instead.
                // Note, that a1 to an are definitely created before b, as they were sub-formulas
                // of it, hence, the ids of a1 to an are smaller than the one of b and therefore a1<b .. an<b.
                // That means, that a1 .. an are inserted into the given set of sub formulas before the position of
                // b (=iter).
                // Note also that the operator of a1 to an cannot be oper, as they where also created with this pool.
                _subformulas.insert( iter->subformulas().begin(), iter->subformulas().end() );
                iter = _subformulas.erase( iter );
            }
            else
            {
                auto iterB = iter; 
                ++iter;
                // Check if the sub-formula at iter is the negation of the sub-formula at iterB
                // Note, that the negation of a formula would by construction always be right after the formula
                // in a set of formulas whose comparison operator is based on the one of formulas This is due to
                // them comparing just the ids and we construct the negation of a formula right after the formula
                // itself and assign the next id to it.
                if( iter != _subformulas.end() )
                {
                    if( formulasInverse( *iterB, *iter ) )
                    {
                        switch( _type )
                        {
                            case FormulaType::AND:
                            {
                                return mpFalse;
                            }
                            case FormulaType::OR:
                            {
                                return mpTrue;
                            }
                            case FormulaType::IFF:
                            {
                                return mpFalse;
                            }
                            case FormulaType::XOR:
                            {
                                _subformulas.erase( iterB );
                                iter = _subformulas.erase( iter );
                                _subformulas.insert( Formula<Pol>( mpTrue ) );
                                break;
                            }
                            default:
                            {
                                assert( false );
                                break;
                            }
                        }
                    }
                }
            }
        }
        if( _subformulas.empty() )
            return mpFalse;
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
                    if( iterToTrue != _subformulas.end() ) _subformulas.erase( iterToTrue );
                    if( iterToFalse != _subformulas.end() ) return mpFalse;
                    else if( _subformulas.empty() ) return mpTrue;
                }
                else if( _type == FormulaType::OR )
                {
                    if( iterToFalse != _subformulas.end() ) _subformulas.erase( iterToFalse );
                    if( iterToTrue != _subformulas.end() ) return mpTrue;
                    else if( _subformulas.empty() ) return mpFalse;
                }
                else // _type == FormulaType::IFF
                {
                    if( iterToFalse != _subformulas.end() && iterToTrue != _subformulas.end() )
                    {
                        return mpFalse;
                    }
                }
            }
            #endif
            if( _subformulas.size() == 1 )
                return newFormulaWithOneSubformula( _type, *(_subformulas.begin()) );
        }
        return add( new Element( _type, std::move( _subformulas ) ) );
    }
}    // namespace carl
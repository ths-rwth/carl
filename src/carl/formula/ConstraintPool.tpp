/**
 * @file ConstraintPool.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "ConstraintPool.h"

using namespace std;

namespace carl
{
    template<typename Pol>
    ConstraintPool<Pol>::ConstraintPool( unsigned _capacity ):
        Singleton<ConstraintPool<Pol>>(),
        mLastConstructedConstraintWasKnown( false ),
        mIdAllocator( 1 ),
        mConsistentConstraint( new Constraint<Pol>( Pol( typename Pol::NumberType( 0 ) ), Relation::EQ, 1 ) ),
        mInconsistentConstraint( new Constraint<Pol>( Pol( typename Pol::NumberType( 0 ) ), Relation::LESS, 2 ) ),
        mConstraints()
    {
        mConstraints.reserve( _capacity );
        mConstraints.insert( mConsistentConstraint );
        mConstraints.insert( mInconsistentConstraint );
        mIdAllocator = 3;
    }

    template<typename Pol>
    ConstraintPool<Pol>::~ConstraintPool()
    {
        mConstraints.erase( mConsistentConstraint );
        delete mConsistentConstraint;
        mConstraints.erase( mInconsistentConstraint );
        delete mInconsistentConstraint;
        while( !mConstraints.empty() )
        {
            const Constraint<Pol>* pCons = (*mConstraints.begin());
            mConstraints.erase( mConstraints.begin() );
            delete pCons;
        }
    }

    template<typename Pol>
    void ConstraintPool<Pol>::clear()
    {
        CONSTRAINT_POOL_LOCK_GUARD
        mConstraints.erase( mConsistentConstraint );
        mConstraints.erase( mInconsistentConstraint );
        while( !mConstraints.empty() )
        {
            const Constraint<Pol>* pCons = (*mConstraints.begin());
            mConstraints.erase( mConstraints.begin() );
            delete pCons;
        }
        mConstraints.insert( mConsistentConstraint );
        mConstraints.insert( mInconsistentConstraint );
        mIdAllocator = 3;
    }
    
    template<typename Pol>
    const Constraint<Pol>* ConstraintPool<Pol>::newBound( const Variable& _var, const Relation _rel, const typename Pol::NumberType& _bound )
    {
        CONSTRAINT_POOL_LOCK_GUARD
        // TODO: Maybe it's better to increment the allocator even if the constraint already exists.
        //       Avoids long waiting for access (mutual exclusion) but increases the allocator to fast.
        Constraint<Pol>* constraint = createNormalizedBound( _var, _rel, _bound );
        auto iterBoolPair = mConstraints.insert( constraint );
        if( iterBoolPair.second )
            mLastConstructedConstraintWasKnown = false;
        else
        {
            mLastConstructedConstraintWasKnown = true;
            delete constraint;
        }
        return *iterBoolPair.first;
    }

    template<typename Pol>
    const Constraint<Pol>* ConstraintPool<Pol>::newConstraint( const Pol& _lhs, const Relation _rel )
    {
        CONSTRAINT_POOL_LOCK_GUARD
        // TODO: Maybe it's better to increment the allocator even if the constraint already exists.
        //       Avoids long waiting for access (mutual exclusion) but increases the allocator to fast.
//        cout << "create polynomial  " << _lhs << " " << Constraint::relationToString( _rel ) << "0" << endl;
        Constraint<Pol>* constraint = createNormalizedConstraint( _lhs, _rel );
//        cout << "   " << *constraint << endl;
        if( constraint->variables().empty() )
        {
            bool constraintConsistent = Constraint<Pol>::evaluate( constraint->constantPart(), constraint->relation() );
            delete constraint;
            return ( constraintConsistent ? mConsistentConstraint : mInconsistentConstraint );
        }
        const Constraint<Pol>* result = addConstraintToPool( constraint );
        return result;
    }

    template<typename Pol>
    Constraint<Pol>* ConstraintPool<Pol>::createNormalizedBound( const Variable& _var, const Relation _rel, const typename Pol::NumberType& _bound ) const
    {
        assert( _rel != Relation::EQ && _rel != Relation::NEQ );
        if( _rel == Relation::GREATER )
        {
            Pol lhs = Pol( _bound ) - Pol( _var );
            return new Constraint<Pol>( lhs, Relation::LESS, mIdAllocator );
        }
        else if( _rel == Relation::GEQ )
        {
            Pol lhs = Pol( _bound ) - Pol( _var );
            return new Constraint<Pol>( lhs, Relation::LEQ, mIdAllocator );
        }
        else
        {
            Pol lhs = Pol( _var ) - Pol( _bound );
            return new Constraint<Pol>( lhs, _rel, mIdAllocator );
        }
    }
    
    template<typename Pol>
    Constraint<Pol>* ConstraintPool<Pol>::createNormalizedConstraint( const Pol& _lhs, const Relation _rel ) const
    {
        if( _rel == Relation::GREATER )
        {
            Pol lhs = _lhs.isZero() ? Pol( typename Pol::NumberType( 0 ) ) : _lhs.coprimeCoefficients();
            if( !lhs.isZero() && (_lhs.lterm().coeff() < 0) == (lhs.lterm().coeff() < 0) )
            {
                lhs = -lhs;
            }
            return new Constraint<Pol>( lhs, Relation::LESS );
        }
        else if( _rel == Relation::GEQ )
        {
            Pol lhs = _lhs.isZero() ? Pol( typename Pol::NumberType( 0 ) ) : _lhs.coprimeCoefficients();
            if( !lhs.isZero() && (_lhs.lterm().coeff() < 0) == (lhs.lterm().coeff() < 0) )
            {
                lhs = -lhs;
            }
            return new Constraint<Pol>( lhs, Relation::LEQ );
        }
        else
        {
            Pol lhs = _lhs.isZero() ? Pol( typename Pol::NumberType( 0 ) ) : _lhs.coprimeCoefficients();
            if( _rel == Relation::EQ || _rel == Relation::NEQ ) 
            {
                if( !_lhs.isZero() && lhs.lterm().coeff() < typename Pol::NumberType( 0 ) ) lhs = -lhs;
            }
            else if( !lhs.isZero() && (_lhs.lterm().coeff() < 0) != (lhs.lterm().coeff() < 0) )
            {
                lhs = -lhs;
            }
            return new Constraint<Pol>( lhs, _rel );
        }
    }

    template<typename Pol>
    const Constraint<Pol>* ConstraintPool<Pol>::addConstraintToPool( Constraint<Pol>* _constraint )
    {
        mLastConstructedConstraintWasKnown = false;
        unsigned constraintConsistent = _constraint->isConsistent();
//        cout << *_constraint << " is consistent: " << constraintConsistent << endl;
		///@todo Use appropriate constant instead of 2.
        if( constraintConsistent == 2 ) // Constraint contains variables.
        {
            auto iterBoolPair = mConstraints.insert( _constraint );
            if( !iterBoolPair.second ) // Constraint has already been generated.
            {
                mLastConstructedConstraintWasKnown = true;
                delete _constraint;
            }
            else
            {
                Constraint<Pol>* constraint = _constraint->simplify();
                if( constraint != nullptr ) // Constraint could be simplified.
                {
//                    cout << *_constraint << " can be simplified to " << *constraint << endl;
                    mConstraints.erase( iterBoolPair.first );
                    delete _constraint;
                    auto iterBoolPairB = mConstraints.insert( constraint );
                    if( !iterBoolPairB.second ) // Simplified version already exists
                    {
                        mLastConstructedConstraintWasKnown = true;
                        delete constraint;
                    }
                    else // Simplified version has not been generated before.
                    {
                        constraint->init();
                        constraint->mID = mIdAllocator;
                        ++mIdAllocator;
                    }
                    return *iterBoolPairB.first;
                }
                else // Constraint could not be simplified.
                {
                    _constraint->mID = mIdAllocator;
                    ++mIdAllocator;
                }
            }
            return *iterBoolPair.first;
        }
        else // Constraint contains no variables.
        {
            mLastConstructedConstraintWasKnown = true;
            delete _constraint;
            return (constraintConsistent ? mConsistentConstraint : mInconsistentConstraint );
        }
    }

    template<typename Pol>
    void ConstraintPool<Pol>::print( ostream& _out ) const
    {
        CONSTRAINT_POOL_LOCK_GUARD
        _out << "Constraint pool:" << endl;
        for( auto constraint = mConstraints.begin(); constraint != mConstraints.end(); ++constraint )
            _out << "    " << **constraint << "  [id=" << (*constraint)->id() << ", hash=" << (*constraint)->getHash() << "]" << endl;
        _out << "---------------------------------------------------" << endl;
    }
    
    template<typename Pol>
    const Constraint<Pol>* newBound( const Variable& _var, const Relation _rel, const typename Pol::NumberType& _bound )
    {
        return ConstraintPool<Pol>::getInstance().newBound( _var, _rel, _bound );
    }

    template<typename Pol>
    const Constraint<Pol>* newConstraint( const Pol& _lhs, const Relation _rel )
    {
        return ConstraintPool<Pol>::getInstance().newConstraint( _lhs, _rel );
    }

    template<typename Pol>
    const ConstraintPool<Pol>& constraintPool()
    {
        return ConstraintPool<Pol>::getInstance();
    }
}    // namespace carl


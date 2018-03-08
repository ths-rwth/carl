/**
 * @file ConstraintPool.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "ConstraintPool.h"
#include "../converter/OldGinacConverter.h"

using namespace std;

namespace carl
{
    template<typename Pol>
    ConstraintPool<Pol>::ConstraintPool( unsigned _capacity ):
        Singleton<ConstraintPool<Pol>>(),
        mLastConstructedConstraintWasKnown( false ),
        mIdAllocator( 1 ),
        mConsistentConstraint( new ConstraintContent<Pol>( Pol( typename Pol::NumberType( 0 ) ), Relation::EQ, 1 ) ),
        mInconsistentConstraint( new ConstraintContent<Pol>( Pol( typename Pol::NumberType( 0 ) ), Relation::LESS, 2 ) ),
        mConstraints(),
        mpPolynomialCache(nullptr)
    {
        VariablePool::getInstance();
		MonomialPool::getInstance();
        if( needs_cache<Pol>::value )
        {
            mpPolynomialCache = std::shared_ptr<typename Pol::CACHE>(new typename Pol::CACHE());
#ifdef USE_GINAC
            setGinacConverterPolynomialCache<Pol>( mpPolynomialCache );
#endif
        }
		/* Make sure that the MonomialPool is created before the ConstraintPool.
		 * Thereby, the MonomialPool gets destroyed after the ConstraintPool.
		 * Thereby, destroying the constraints (and the Monomials contained) works correctly.
		 */
        mConstraints.reserve( _capacity );
        mConstraints.insert( mConsistentConstraint );
        mConstraints.insert( mInconsistentConstraint );
        mConsistentConstraint->mUsages = 1; // avoids deleting it
        mInconsistentConstraint->mUsages = 1; // avoids deleting it
        mIdAllocator = 3;
    }

    template<typename Pol>
    ConstraintPool<Pol>::~ConstraintPool()
    {
        mConstraints.clear();
        delete mConsistentConstraint;
        delete mInconsistentConstraint;
    }

    template<typename Pol>
    void ConstraintPool<Pol>::clear()
    {
        CONSTRAINT_POOL_LOCK_GUARD
        mIdAllocator = 3;
    }
    
    template<typename Pol>
    const ConstraintContent<Pol>* ConstraintPool<Pol>::create( const Variable& _var, const Relation _rel, const typename Pol::NumberType& _bound )
    {
        CONSTRAINT_POOL_LOCK_GUARD
        ConstraintContent<Pol>* constraint = createNormalizedBound( _var, _rel, _bound );
        auto iterBoolPair = mConstraints.insert( constraint );
        if( iterBoolPair.second )
        {
            constraint->mVariables.insert(_var);
            constraint->initEager();
            constraint->mID = mIdAllocator;
            ++mIdAllocator;
            mLastConstructedConstraintWasKnown = false;
        }
        else
        {
            mLastConstructedConstraintWasKnown = true;
            delete constraint;
        }
        return *iterBoolPair.first;
    }

    template<typename Pol>
    const ConstraintContent<Pol>* ConstraintPool<Pol>::create( const Pol& _lhs, Relation _rel )
    {
        CONSTRAINT_POOL_LOCK_GUARD
        if( _lhs.isConstant() )
            return evaluate( _lhs.constantPart(), _rel ) ? mConsistentConstraint : mInconsistentConstraint;
        if( _lhs.totalDegree() == 1 && (_rel != Relation::EQ && _rel != Relation::NEQ) && _lhs.isUnivariate() )
        {
            if( carl::isNegative( _lhs.lcoeff() ) )
            {
                switch( _rel )
                {
                    case Relation::LESS: _rel = Relation::GREATER; break;
                    case Relation::GREATER: _rel = Relation::LESS; break;
                    case Relation::LEQ: _rel = Relation::GEQ; break;
                    default: assert( _rel == Relation::GEQ); _rel = Relation::LEQ; break;
                }
            }
            return create( _lhs.getSingleVariable(), _rel, (-_lhs.constantPart())/_lhs.lcoeff() );
        }
        return addConstraintToPool( createNormalizedConstraint( _lhs, _rel ) );
    }

    template<typename Pol>
    ConstraintContent<Pol>* ConstraintPool<Pol>::createNormalizedBound( Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound ) const
    {
        Pol lhs = makePolynomial<Pol>( _var );
        switch( _rel )
        {
            case Relation::GREATER:
                lhs = -lhs;
                if( _var.type() == VariableType::VT_INT )
                {
                    if( isInteger( _bound ) )
                        lhs += _bound + typename Pol::NumberType( 1 );
                    else
                        lhs += carl::ceil( _bound );
                    _rel = Relation::LEQ;
                }
                else
                {
                    lhs += _bound;
                    _rel = Relation::LESS;
                }
                break;
            case Relation::GEQ:
                lhs = -lhs;
                if( _var.type() == VariableType::VT_INT )
                {
                    if( isInteger( _bound ) )
                        lhs += _bound;
                    else
                        lhs += carl::ceil( _bound );
                    _rel = Relation::LEQ;
                }
                else
                {
                    lhs += _bound;
                    _rel = Relation::LEQ;
                }
                break;
            case Relation::LESS:
                if( _var.type() == VariableType::VT_INT )
                {
                    if( isInteger( _bound ) )
                        lhs -= (_bound - typename Pol::NumberType( 1 ));
                    else
                        lhs -= carl::floor( _bound );
                    _rel = Relation::LEQ;
                }
                else
                {
                    lhs -= _bound;
                }
                break;
            case Relation::LEQ:
                if( _var.type() == VariableType::VT_INT )
                {
                    if( isInteger( _bound ) )
                        lhs -= _bound;
                    else
                        lhs -= carl::floor( _bound );
                }
                else
                    lhs -= _bound;
                break;
			case Relation::EQ:
				if (_var.type() == VariableType::VT_INT) {
					if (isInteger(_bound)) {
						lhs -= _bound;
					} else {
						return new ConstraintContent<Pol>( Pol( typename Pol::NumberType( 0 ) ), Relation::LESS );
					}
				} else {
					lhs -= _bound;
				}
				break;
			case Relation::NEQ:
				if (_var.type() == VariableType::VT_INT) {
					if (isInteger(_bound)) {
						lhs -= _bound;
					} else {
						return new ConstraintContent<Pol>( Pol( typename Pol::NumberType( 0 ) ), Relation::EQ );
					}
				} else {
					lhs -= _bound;
				}
				break;
        }
        return new ConstraintContent<Pol>( std::move(lhs), _rel );
    }
    
    template<typename Pol>
    ConstraintContent<Pol>* ConstraintPool<Pol>::createNormalizedConstraint( const Pol& _lhs, const Relation _rel ) const
    {
        if( _rel == Relation::GREATER )
        {
            Pol lhs = _lhs.isZero() ? Pol( typename Pol::NumberType( 0 ) ) : _lhs.coprimeCoefficients();
            if( !lhs.isZero() && (_lhs.lterm().coeff() < 0) == (lhs.lterm().coeff() < 0) )
            {
                lhs = -lhs;
            }
            return new ConstraintContent<Pol>( std::move(lhs), Relation::LESS );
        }
        else if( _rel == Relation::GEQ )
        {
            Pol lhs = _lhs.isZero() ? Pol( typename Pol::NumberType( 0 ) ) : _lhs.coprimeCoefficients();
            if( !lhs.isZero() && (_lhs.lterm().coeff() < 0) == (lhs.lterm().coeff() < 0) )
            {
                lhs = -lhs;
            }
            return new ConstraintContent<Pol>( std::move(lhs), Relation::LEQ );
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
            return new ConstraintContent<Pol>( std::move(lhs), _rel );
        }
    }

    template<typename Pol>
    const ConstraintContent<Pol>* ConstraintPool<Pol>::addConstraintToPool( ConstraintContent<Pol>* _constraint )
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
                ConstraintContent<Pol>* constraint = _constraint->simplify();
                if( constraint != nullptr ) // Constraint could be simplified.
                {
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
                        constraint->initLazy();
                        constraint->initEager();
                        constraint->mID = mIdAllocator;
                        ++mIdAllocator;
                    }
                    assert( (*iterBoolPairB.first)->mUsages < std::numeric_limits<size_t>::max() );
                    ++(*iterBoolPairB.first)->mUsages;
                    return *iterBoolPairB.first;
                }
                else // Constraint could not be simplified.
                {
                    _constraint->initEager();
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
            const ConstraintContent<Pol>* result = (constraintConsistent ? mConsistentConstraint : mInconsistentConstraint );
            return result;
        }
    }

    template<typename Pol>
    void ConstraintPool<Pol>::print( ostream& _out ) const
    {
        CONSTRAINT_POOL_LOCK_GUARD
        _out << "Constraint pool:" << endl;
        for( auto constraint = mConstraints.begin(); constraint != mConstraints.end(); ++constraint )
            _out << "    " << **constraint << "  [id=" << (*constraint)->mID << ", hash=" << (*constraint)->hash() << ", usages=" << (*constraint)->mUsages << "]" << endl;
        _out << "---------------------------------------------------" << endl;
    }

    template<typename Pol>
    const ConstraintPool<Pol>& constraintPool()
    {
        return ConstraintPool<Pol>::getInstance();
    }
}    // namespace carl

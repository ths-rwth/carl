/**
 * @file ConstraintPool.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include "../util/Singleton.h"
#include "../util/Common.h"
#include "Constraint.h"
#include <limits>
#include <mutex>

namespace carl
{
    template<typename Pol>
    class ConstraintPool : public Singleton<ConstraintPool<Pol>>
    {
        friend Singleton<ConstraintPool>;
        private:
            // Members:

            /// A flag indicating whether the last constraint which has been tried to add to the pool, was already an element of it.
            bool mLastConstructedConstraintWasKnown;
            /// id allocator
            unsigned mIdAllocator;
            /// The constraint (0=0) representing a valid constraint.
            const ConstraintContent<Pol>* mConsistentConstraint;
            /// The constraint (0>0) representing an inconsistent constraint.
            const ConstraintContent<Pol>* mInconsistentConstraint;
            /// Mutex to avoid multiple access to the pool
            mutable std::recursive_mutex mMutexPool;
            /// The constraint pool.
            FastPointerSet<ConstraintContent<Pol>> mConstraints;
            /// Pointer to the polynomial cache, if cache is needed of the polynomial type, otherwise, it is nullptr.
            std::shared_ptr<typename Pol::CACHE> mpPolynomialCache;
            
            #ifdef THREAD_SAFE
            #define CONSTRAINT_POOL_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock1( mMutexPool );
            #define CONSTRAINT_POOL_LOCK mMutexPool.lock();
            #define CONSTRAINT_POOL_UNLOCK mMutexPool.unlock();
            #else
            #define CONSTRAINT_POOL_LOCK_GUARD
            #define CONSTRAINT_POOL_LOCK
            #define CONSTRAINT_POOL_UNLOCK
            #endif
            
            /**
             * Creates a normalized constraint, which has the same solutions as the constraint consisting of the given
             * left-hand side and relation symbol.
             * Note, that this method uses the allocator which is locked before calling.
             * @param _var The left-hand side of the constraint before normalization,
             * @param _rel The relation symbol of the constraint before normalization,
             * @param _bound
             * @return The constructed constraint.
             */
            ConstraintContent<Pol>* createNormalizedBound( Variable::Arg _var, Relation _rel, const typename Pol::NumberType& _bound ) const;
            
            /**
             * Creates a normalized constraint, which has the same solutions as the constraint consisting of the given
             * left-hand side and relation symbol.
             * Note, that this method uses the allocator which is locked before calling.
             * @param _lhs The left-hand side of the constraint before normalization,
             * @param _rel The relation symbol of the constraint before normalization,
             * @return The constructed constraint.
             */
            ConstraintContent<Pol>* createNormalizedConstraint( const Pol& _lhs, const Relation _rel ) const;
            
            /**
             * Adds the given constraint to the pool, if it does not yet occur in there.
             * Note, that this method uses the allocator which is locked before calling.
             * @sideeffect The given constraint will be deleted, if it already occurs in the pool.
             * @param _constraint The constraint to add to the pool.
             * @return The given constraint, if it did not yet occur in the pool;
             *          The equivalent constraint already occurring in the pool.
             */
            const ConstraintContent<Pol>* addConstraintToPool( ConstraintContent<Pol>* _constraint );
            
            /**
             * @return A pointer to the constraint which represents any constraint for which it is easy to 
             *          decide, whether it is consistent, e.g. 0=0, -1!=0, x^2+1>0
             */
            const ConstraintContent<Pol>* consistentConstraint() const
            {
                return mConsistentConstraint;
            }
                        
            /**
             * @return A pointer to the constraint which represents any constraint for which it is easy to 
             *          decide, whether it is consistent, e.g. 1=0, 0!=0, x^2+1=0
            */
            const ConstraintContent<Pol>* inconsistentConstraint() const
            {
                return mInconsistentConstraint;
            }

        protected:
            
            /**
             * Constructor of the constraint pool.
             * @param _capacity Expected necessary capacity of the pool.
             */
            ConstraintPool( unsigned _capacity = 10000 );

        public:

            /**
             * Destructor.
             */
            ~ConstraintPool();

            /**
             * @return An iterator to the first constraint in this pool.
             */
            typename FastPointerSet<ConstraintContent<Pol>>::const_iterator begin() const
            {
                // TODO: Will begin() be valid if we insert elements?
                CONSTRAINT_POOL_LOCK_GUARD
                auto result = mConstraints.begin();
                return result;
            }

            /**
             * @return An iterator to the end of the container of the constraints in this pool.
             */
            typename FastPointerSet<ConstraintContent<Pol>>::const_iterator end() const
            {
                // TODO: Will end() be changed if we insert elements?
                CONSTRAINT_POOL_LOCK_GUARD
                auto result = mConstraints.end();
                return result;
            }

            /**
             * @return The number of constraint in this pool.
             */
            size_t size() const
            {
                CONSTRAINT_POOL_LOCK_GUARD
                size_t result = mConstraints.size();
                return result;
            }
            
            /**
             * @return true, the last constraint which has been tried to add to the pool, was already an element of it;
             *         false, otherwise.
             */
            bool lastConstructedConstraintWasKnown() const
            {
                return mLastConstructedConstraintWasKnown;
            }
            
            const std::shared_ptr<typename Pol::CACHE>& pPolynomialCache() const
            {
                return mpPolynomialCache;
            }

            /**
             * Note: This method makes the other accesses to the constraint pool waiting.
             * @return The highest degree occurring in all constraints
             */
            std::size_t maxDegree() const
            {
                std::size_t result = 0;
                CONSTRAINT_POOL_LOCK_GUARD
                for( auto constraint = mConstraints.begin(); constraint != mConstraints.end(); ++constraint )
                {
                    std::size_t maxdeg = (*constraint)->mLhs.isZero() ? 0 : (*constraint)->mLhs.totalDegree();
                    if(maxdeg > result) 
                        result = maxdeg;
                }
                return result;
            }
            
            /**
             * Note: This method makes the other accesses to the constraint pool waiting.
             * @return The number of non-linear constraints in the pool.
             */
            unsigned nrNonLinearConstraints() const
            {
                unsigned nonlinear = 0;
                CONSTRAINT_POOL_LOCK_GUARD
                for( auto constraint = mConstraints.begin(); constraint != mConstraints.end(); ++constraint )
                {
                    if( !(*constraint)->mLhs.isLinear() ) 
                        ++nonlinear;
                }
                return nonlinear;
            }
            
            /**
             * Resets the constraint pool.
             * Note: Do not use it. It is only made for the Benchmax-Tool.
             */
            void clear();
            
            /**
             * Constructs a new constraint and adds it to the pool, if it is not yet a member. If it is a
             * member, this will be returned instead of a new constraint.
             * Note, that the left-hand side of the constraint is simplified and normalized, hence it is
             * not necessarily equal to the given left-hand side. The same holds for the relation symbol.
             * However, it is assured that the returned constraint has the same solutions as
             * the expected one.
             * @param _var The left-hand side of the constraint.
             * @param _rel The relation symbol of the constraint.
             * @param _bound An over-approximation of the variables which occur on the left-hand side.
             * @return The constructed constraint.
             */
            const ConstraintContent<Pol>* create( const Variable& _var, Relation _rel, const typename Pol::NumberType& _bound );
            
            /**
             * Constructs a new constraint and adds it to the pool, if it is not yet a member. If it is a
             * member, this will be returned instead of a new constraint.
             * Note, that the left-hand side of the constraint is simplified and normalized, hence it is
             * not necessarily equal to the given left-hand side. The same holds for the relation symbol.
             * However, it is assured that the returned constraint has the same solutions as
             * the expected one.
             * @param _lhs The left-hand side of the constraint.
             * @param _rel The relation symbol of the constraint.
             * @return The constructed constraint.
             */
            const ConstraintContent<Pol>* create( const Pol& _lhs, Relation _rel );

            /**
             * @return If _true = true, the valid constraint 0=0, otherwise the invalid formula 0<0.
             */
            const ConstraintContent<Pol>* create( bool _true )
            {
                return _true ? consistentConstraint() : inconsistentConstraint();
            }
            
            const ConstraintContent<Pol>* create( carl::Variable::Arg _var, Relation _rel )
            {
                return create( makePolynomial<Pol>(_var), _rel );
            }
            
            template<typename P = Pol, EnableIf<needs_cache<P>> = dummy>
            const ConstraintContent<Pol>* create( const typename Pol::PolyType& _lhs, Relation _rel )
            {
                return create( makePolynomial<Pol>( _lhs ), _rel );
            }
            
            void free( const ConstraintContent<Pol>* _cc ) noexcept
            {
                CONSTRAINT_POOL_LOCK_GUARD
                assert( _cc->mUsages > 0 );
                --_cc->mUsages;
                if( _cc->mUsages == 0 )
                {
                    mConstraints.erase( _cc );
                    delete _cc;
                }
            }
            
            void reg( const ConstraintContent<Pol>* _cc ) const
            {
                CONSTRAINT_POOL_LOCK_GUARD
                assert( _cc->mUsages < std::numeric_limits<size_t>::max() );
                ++_cc->mUsages;
            }
            
            /**
             * Prints all constraints in the constraint pool on the given stream.
             *
             * @param _out The stream to print on.
             */
            void print( std::ostream& _out = std::cout ) const;
    };

     /**
      * @return A constant reference to the shared constraint pool.
      */
     template<typename Pol>
     const ConstraintPool<Pol>& constraintPool();
    
}    // namespace carl

#include "ConstraintPool.tpp"

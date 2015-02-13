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
            const Constraint<Pol>* mConsistentConstraint;
            /// The constraint (0>0) representing an inconsistent constraint.
            const Constraint<Pol>* mInconsistentConstraint;
            /// Mutex to avoid multiple access to the pool
            mutable std::mutex mMutexPool;
            /// The constraint pool.
            FastPointerSet<Constraint<Pol>> mConstraints;
            /// Pointer to the polynomial cache, if cache is needed of the polynomial type, otherwise, it is nullptr.
            std::shared_ptr<typename Pol::CACHE> mpPolynomialCache;
            
            #ifdef SMTRAT_STRAT_PARALLEL_MODE
            #define CONSTRAINT_POOL_LOCK_GUARD std::lock_guard<std::mutex> lock1( mMutexPool );
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
            Constraint<Pol>* createNormalizedBound( const Variable& _var, const Relation _rel, const typename Pol::NumberType& _bound ) const;
            
            /**
             * Creates a normalized constraint, which has the same solutions as the constraint consisting of the given
             * left-hand side and relation symbol.
             * Note, that this method uses the allocator which is locked before calling.
             * @param _lhs The left-hand side of the constraint before normalization,
             * @param _rel The relation symbol of the constraint before normalization,
             * @return The constructed constraint.
             */
            Constraint<Pol>* createNormalizedConstraint( const Pol& _lhs, const Relation _rel ) const;
            
            /**
             * Adds the given constraint to the pool, if it does not yet occur in there.
             * Note, that this method uses the allocator which is locked before calling.
             * @sideeffect The given constraint will be deleted, if it already occurs in the pool.
             * @param _constraint The constraint to add to the pool.
             * @return The given constraint, if it did not yet occur in the pool;
             *          The equivalent constraint already occurring in the pool.
             */
            const Constraint<Pol>* addConstraintToPool( Constraint<Pol>* _constraint );

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
            typename FastPointerSet<Constraint<Pol>>::const_iterator begin() const
            {
                // TODO: Will begin() be valid if we insert elements?
                CONSTRAINT_POOL_LOCK_GUARD
                auto result = mConstraints.begin();
                return result;
            }

            /**
             * @return An iterator to the end of the container of the constraints in this pool.
             */
            typename FastPointerSet<Constraint<Pol>>::const_iterator end() const
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
            
            /**
             * @return A pointer to the constraint which represents any constraint for which it is easy to 
             *          decide, whether it is consistent, e.g. 0=0, -1!=0, x^2+1>0
             */
            const Constraint<Pol>* consistentConstraint() const
            {
                return mConsistentConstraint;
            }
                        
            /**
             * @return A pointer to the constraint which represents any constraint for which it is easy to 
             *          decide, whether it is consistent, e.g. 1=0, 0!=0, x^2+1=0
            */
            const Constraint<Pol>* inconsistentConstraint() const
            {
                return mInconsistentConstraint;
            }
            
            const std::shared_ptr<typename Pol::CACHE>& pPolynomialCache() const
            {
                return mpPolynomialCache;
            }

            /**
             * Note: This method makes the other accesses to the constraint pool waiting.
             * @return The highest degree occurring in all constraints
             */
            exponent maxDegree() const
            {
                exponent result = 0;
                CONSTRAINT_POOL_LOCK_GUARD
                for( auto constraint = mConstraints.begin(); constraint != mConstraints.end(); ++constraint )
                {
                    exponent maxdeg = (*constraint)->lhs().isZero() ? 0 : (*constraint)->lhs().totalDegree();
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
                    if( !(*constraint)->lhs().isLinear() ) 
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
            const Constraint<Pol>* newBound( const Variable& _var, Relation _rel, const typename Pol::NumberType& _bound );
            
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
            const Constraint<Pol>* newConstraint( Pol&& _lhs, Relation _rel );
            
            const Constraint<Pol>* newConstraint( const Pol& _lhs, Relation _rel )
            {
                return newConstraint(std::move(Pol(_lhs)), _rel );
            }
            
            const Constraint<Pol>* newConstraint( const carl::Variable::Arg _var, Relation _rel )
            {
                return newConstraint( std::move( makePolynomial<Pol>(_var) ), _rel );
            }
            
            template<typename P = Pol, EnableIf<needs_cache<P>> = dummy>
            const Constraint<Pol>* newConstraint( typename Pol::PolyType&& _lhs, Relation _rel )
            {
                return newConstraint( std::move( makePolynomial<Pol>( std::move( _lhs ) ) ), _rel );
            }
            
            template<typename P = Pol, EnableIf<needs_cache<P>> = dummy>
            const Constraint<Pol>* newConstraint( const typename Pol::PolyType& _lhs, Relation _rel )
            {
                return newConstraint( std::move( makePolynomial<Pol>( _lhs ) ), _rel );
            }
            
            /**
             * Prints all constraints in the constraint pool on the given stream.
             *
             * @param _out The stream to print on.
             */
            void print( std::ostream& _out = std::cout ) const;
    };
    
    /**
     * Constructs a new constraint and adds it to the shared constraint pool, if it is not yet a member. If it is a
     * member, this will be returned instead of a new constraint.
     * Note, that the left-hand side of the constraint is simplified and normalized, hence it is
     * not necessarily equal to the given left-hand side. The same holds for the relation symbol.
     * However, it is assured that the returned constraint has the same solutions as
     * the expected one.
     * @param _var The left-hand side of the constraint.
     * @param _rel The relation symbol of the constraint.
     * @param _bound
     * @return The constructed constraint.
     */
    template<typename Pol>
    const Constraint<Pol>* newBound( const Variable& _var, const Relation _rel, const typename Pol::NumberType& _bound );

    /**
     * Constructs a new constraint and adds it to the shared constraint pool, if it is not yet a member. If it is a
     * member, this will be returned instead of a new constraint.
     * Note, that the left-hand side of the constraint is simplified and normalized, hence it is
     * not necessarily equal to the given left-hand side. The same holds for the relation symbol.
     * However, it is assured that the returned constraint has the same solutions as
     * the expected one.
     * @param _lhs The left-hand side of the constraint.
     * @param _rel The relation symbol of the constraint.
     * @return The constructed constraint.
     */
    template<typename Pol>
    const Constraint<Pol>* newConstraint( const Pol& _lhs, Relation _rel );
    template<typename Pol>
    const Constraint<Pol>* newConstraint( const carl::Variable::Arg _var, Relation _rel );
    template<typename Pol, EnableIf<needs_cache<Pol>> = dummy>
    const Constraint<Pol>* newConstraint( const typename Pol::PolyType& _lhs, Relation _rel );
    template<typename Pol, EnableIf<needs_cache<Pol>> = dummy>
    const Constraint<Pol>* newConstraint( typename Pol::PolyType&& _lhs, Relation _rel );
    template<typename Pol>
    const Constraint<Pol>* newConstraint( Pol&& _lhs, Relation _rel );

     /**
      * @return A constant reference to the shared constraint pool.
      */
     template<typename Pol>
     const ConstraintPool<Pol>& constraintPool();
    
}    // namespace carl

#include "ConstraintPool.tpp"
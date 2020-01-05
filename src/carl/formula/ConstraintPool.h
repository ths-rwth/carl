/**
 * @file ConstraintPool.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include "../util/Common.h"
#include "../util/Singleton.h"
#include "Constraint.h"
#include "ConstraintRaw.h"
#include <boost/intrusive/unordered_set.hpp>
#include <limits>
#include <memory>
#include <mutex>

namespace carl {

template<typename Pol>
inline std::size_t hash_value(const carl::ConstraintContent<Pol>& content) {
    return content.hash();
}

template<typename Pol>
class ConstraintPool : public Singleton<ConstraintPool<Pol>> {
    friend Singleton<ConstraintPool>;

    struct content_equal {
        bool operator()(const RawConstraint<Pol>& data, const ConstraintContent<Pol>& content) const { 
            return data.mLhs == content.mLhs && data.mRelation == content.mRelation;
        }

        bool operator()(const ConstraintContent<Pol>& content, const RawConstraint<Pol>& data) const {
            return (*this)(data, content);
        }
    };

    struct content_hash {
        std::size_t operator()(const RawConstraint<Pol>& data) const {
            return CONSTRAINT_HASH(data.mLhs, data.mRelation, Pol);
        }
    };

private:
    // Members:

    /// id allocator
    unsigned mIdAllocator;
    /// The constraint (0=0) representing a valid constraint.
    std::shared_ptr<ConstraintContent<Pol>> mConsistentConstraint;
    /// The constraint (0>0) representing an inconsistent constraint.
    std::shared_ptr<ConstraintContent<Pol>> mInconsistentConstraint;
    /// Mutex to avoid multiple access to the pool
    mutable std::recursive_mutex mMutexPool;

    using underlying_set = boost::intrusive::unordered_set<ConstraintContent<Pol>>;
    std::unique_ptr<typename underlying_set::bucket_type[]> mPoolBuckets;
    /// The constraint pool.
    underlying_set mPool;
    
    /// Pointer to the polynomial cache, if cache is needed of the polynomial type, otherwise, it is nullptr.
    std::shared_ptr<typename Pol::CACHE> mpPolynomialCache;

    #ifdef THREAD_SAFE
    #define CONSTRAINT_POOL_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock1(mMutexPool);
    #define CONSTRAINT_POOL_LOCK mMutexPool.lock();
    #define CONSTRAINT_POOL_UNLOCK mMutexPool.unlock();
    #else
    #define CONSTRAINT_POOL_LOCK_GUARD
    #define CONSTRAINT_POOL_LOCK
    #define CONSTRAINT_POOL_UNLOCK
    #endif

    std::shared_ptr<ConstraintContent<Pol>> add(RawConstraint<Pol>&& _constraint);

    std::shared_ptr<ConstraintContent<Pol>> addToPool(RawConstraint<Pol>&& _constraint);

    /**
     * @return A pointer to the constraint which represents any constraint for which it is easy to 
     *          decide, whether it is consistent, e.g. 0=0, -1!=0, x^2+1>0
     */
    std::shared_ptr<ConstraintContent<Pol>> consistentConstraint() const {
        return mConsistentConstraint;
    }

    /**
     * @return A pointer to the constraint which represents any constraint for which it is easy to 
     *          decide, whether it is consistent, e.g. 1=0, 0!=0, x^2+1=0
    */
    std::shared_ptr<ConstraintContent<Pol>> inconsistentConstraint() const {
        return mInconsistentConstraint;
    }

    const std::shared_ptr<typename Pol::CACHE>& pPolynomialCache() const {
        return mpPolynomialCache;
    }

protected:
    /**
     * Constructor of the constraint pool.
     * @param _capacity Expected necessary capacity of the pool.
     */
    explicit ConstraintPool(unsigned _capacity = 10000);

public:
    /**
     * Destructor.
     */
    ~ConstraintPool();

    /**
     * @return The number of constraint in this pool.
     */
    size_t size() const {
        CONSTRAINT_POOL_LOCK_GUARD
        size_t result = mPool.size();
        return result;
    }

    /**
     * Note: This method makes the other accesses to the constraint pool waiting.
     * @return The highest degree occurring in all constraints
     */
    std::size_t maxDegree() const {
        std::size_t result = 0;
        CONSTRAINT_POOL_LOCK_GUARD
        for (auto constraint = mPool.begin(); constraint != mPool.end(); ++constraint) {
            std::size_t maxdeg = isZero(constraint->mLhs) ? 0 : constraint->mLhs.totalDegree();
            if (maxdeg > result)
                result = maxdeg;
        }
        return result;
    }

    /**
     * Note: This method makes the other accesses to the constraint pool waiting.
     * @return The number of non-linear constraints in the pool.
     */
    unsigned nrNonLinearConstraints() const {
        unsigned nonlinear = 0;
        CONSTRAINT_POOL_LOCK_GUARD
        for (auto constraint = mPool.begin(); constraint != mPool.end(); ++constraint) {
            if (!constraint->mLhs.isLinear())
                ++nonlinear;
        }
        return nonlinear;
    }

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
    std::shared_ptr<ConstraintContent<Pol>> create(const Variable& _var, Relation _rel, const typename Pol::NumberType& _bound);

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
    std::shared_ptr<ConstraintContent<Pol>> create(const Pol& _lhs, Relation _rel);

    /**
     * @return If _true = true, the valid constraint 0=0, otherwise the invalid formula 0<0.
     */
    std::shared_ptr<ConstraintContent<Pol>> create(bool _true) {
        return _true ? consistentConstraint() : inconsistentConstraint();
    }

    std::shared_ptr<ConstraintContent<Pol>> create(carl::Variable::Arg _var, Relation _rel) {
        return create(makePolynomial<Pol>(_var), _rel);
    }

    template<typename P = Pol, EnableIf<needs_cache<P>> = dummy>
    std::shared_ptr<ConstraintContent<Pol>> create(const typename Pol::PolyType& _lhs, Relation _rel) {
        return create(makePolynomial<Pol>(_lhs), _rel);
    }

    void free(const ConstraintContent<Pol>* _cc) noexcept {
        CONSTRAINT_POOL_LOCK_GUARD
        auto it = mPool.find(*_cc);
        assert(it != mPool.end());
        mPool.erase(it);
    }

    /**
     * Prints all constraints in the constraint pool on the given stream.
     *
     * @param _out The stream to print on.
     */
    void print(std::ostream& _out = std::cout) const;
};

/**
 * @return A constant reference to the shared constraint pool.
 */
template<typename Pol>
const ConstraintPool<Pol>& constraintPool();

} // namespace carl

#include "ConstraintPool.tpp"

/**
 * @file ConstraintPool.tpp
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "../converter/OldGinacConverter.h"
#include "ConstraintPool.h"

namespace carl {

template<typename Pol>
ConstraintPool<Pol>::ConstraintPool(unsigned _capacity)
	: Singleton<ConstraintPool<Pol>>(),
	  mIdAllocator(1),
	  mPoolBuckets(new typename underlying_set::bucket_type[mRehashPolicy.numBucketsFor(_capacity)]),
	  mPool(typename underlying_set::bucket_traits(mPoolBuckets.get(), mRehashPolicy.numBucketsFor(_capacity))),
	  mpPolynomialCache(nullptr) {
	VariablePool::getInstance();
	MonomialPool::getInstance();
	if (needs_cache<Pol>::value) {
		mpPolynomialCache = std::shared_ptr<typename Pol::CACHE>(new typename Pol::CACHE());
        #ifdef USE_GINAC
		setGinacConverterPolynomialCache<Pol>(mpPolynomialCache);
        #endif
	}
	/* Make sure that the MonomialPool is created before the ConstraintPool.
    * Thereby, the MonomialPool gets destroyed after the ConstraintPool.
    * Thereby, destroying the constraints (and the Monomials contained) works correctly.
    */

	mConsistentConstraint = addToPool(RawConstraint<Pol>(true));
	mInconsistentConstraint = addToPool(RawConstraint<Pol>(false));
	CARL_LOG_DEBUG("carl.pool", "ConstraintPool constructed");
}

template<typename Pol>
ConstraintPool<Pol>::~ConstraintPool() {
	mPool.clear();
}

template<typename Pol>
std::shared_ptr<ConstraintContent<Pol>> ConstraintPool<Pol>::create(const Variable& _var, const Relation _rel, const typename Pol::NumberType& _bound) {
	return add(RawConstraint<Pol>(_var, _rel, _bound));
}

template<typename Pol>
std::shared_ptr<ConstraintContent<Pol>> ConstraintPool<Pol>::create(const Pol& _lhs, Relation _rel) {
	return add(RawConstraint<Pol>(_lhs, _rel));
}

template<typename Pol>
std::shared_ptr<ConstraintContent<Pol>> ConstraintPool<Pol>::add(RawConstraint<Pol>&& _constraint) {
	CONSTRAINT_POOL_LOCK_GUARD

	CARL_LOG_DEBUG("carl.pool", "Adding constraint");
	unsigned constraintConsistent = _constraint.is_consistent();
	CARL_LOG_DEBUG("carl.pool", "Consistent? " << constraintConsistent);

	///@todo Use appropriate constant instead of 2.
	if (constraintConsistent == 2) { // Constraint contains variables.
		auto res = mPool.find(_constraint, content_hash(), content_equal());
		if (res != mPool.end()) {
			return res->mWeakPtr.lock();
		} else {
			_constraint.simplify();
			return addToPool(std::move(_constraint));
		}
	} else { // Constraint contains no variables.
		return constraintConsistent ? mConsistentConstraint : mInconsistentConstraint;
	}
}

template<typename Pol>
std::shared_ptr<ConstraintContent<Pol>> ConstraintPool<Pol>::addToPool(RawConstraint<Pol>&& _constraint) {
	typename underlying_set::insert_commit_data insert_data;
	auto res = mPool.insert_check(_constraint, content_hash(), content_equal(), insert_data);
	if (!res.second) {
		return res.first->mWeakPtr.lock();
	} else {
		auto shared = std::shared_ptr<ConstraintContent<Pol>>(new ConstraintContent<Pol>(mIdAllocator, std::move(_constraint.mLhs), _constraint.mRelation, std::move(_constraint.mVariables), _constraint.mLhsDefiniteness, _constraint.is_consistent()));
		++mIdAllocator;
		shared.get()->mWeakPtr = shared;
		mPool.insert_commit(*shared.get(), insert_data);
		check_rehash();
		return shared;
	}
}

template<typename Pol>
void ConstraintPool<Pol>::print(std::ostream& _out) const {
	CONSTRAINT_POOL_LOCK_GUARD
	_out << "Constraint pool:" << std::endl;
	for (auto constraint = mPool.begin(); constraint != mPool.end(); ++constraint)
		_out << "    " << *constraint << "  [id=" << constraint->mID << ", hash=" << constraint->hash() << "]" << std::endl;
	_out << "---------------------------------------------------" << std::endl;
}

template<typename Pol>
const ConstraintPool<Pol>& constraintPool() {
	return ConstraintPool<Pol>::getInstance();
}

} // namespace carl

/**
 * @file Pool.h
 *
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 */

#pragma once

#include "../../util/Common.h"
#include <carl-common/memory/Singleton.h>

#include <mutex>

namespace carl
{

	template<typename Element>
	class Pool
	{
		// friend Singleton<Pool>;

		using ElementPtr = Element*;
		using ConstElementPtr = const Element*;

	private:

		// Members:
		/// id allocator
		unsigned mIdAllocator = 1;
		/// The formula pool.
		FastPointerSet<Element> mPool;
		/// Mutex to avoid multiple access to the pool
		mutable std::mutex mMutexPool;

#define POOL_LOCK_GUARD std::lock_guard<std::mutex> lock( mMutexPool );
#define POOL_LOCK mMutexPool.lock();
#define POOL_UNLOCK mMutexPool.unlock();

	protected:

		/**
		 * Constructor of the pool.
		 * @param _capacity Expected necessary capacity of the pool.
		 */
		explicit Pool(unsigned _capacity = 10000) {
			mPool.reserve(_capacity);
		}

		~Pool()
		{
			while(!mPool.empty()) {
				ConstElementPtr element = *mPool.begin();
				mPool.erase(mPool.begin());
				delete element;
			}
		}

		/**
		 * Assigns a unique id to the generated element.
		 * Note that this method serves as a callback for subclasses.
		 * The actual assignment of the id is done there.
		 * @param _element The element for which to add the id.
		 * @param _id A unique id.
		 */
		virtual void assignId(ElementPtr /* _element */, std::size_t /* _id */)
		{ }

	public:

		void print() const
		{
			std::cout << "Pool contains:" << std::endl;
			for(const auto& ele : mPool) {
				std::cout << "- " << *ele << std::endl;
			}
			std::cout << std::endl;
		}

		/**
		 * Inserts the given element into the pool, if it does not yet occur in there.
		 * @param _element The element to add to the pool.
		 * @param _assertFreshness When true, an assertion fails if the element is not fresh
		 *                         (i.e., if it already occurs in the pool).
		 * @return The position of the given element in the pool and true, if it did not yet occur in the pool;
		 *         The position of the equivalent element in the pool and false, otherwise.
		 */
		std::pair<typename FastPointerSet<Element>::iterator, bool> insert(ElementPtr _element, bool _assertFreshness = false)
		{
			POOL_LOCK_GUARD
			auto iterBoolPair = mPool.insert(_element);
			assert(iterBoolPair.second || !_assertFreshness);

			if(iterBoolPair.second) { // Element has just been inserted
				// Assign a new id
				assignId(_element, mIdAllocator++); // id should be set here to avoid conflicts when multi-threading
			} else {
				// The argument can be deleted, return the already existent instance
				delete _element;
			}

			return iterBoolPair;
		}

		/**
		 * Adds the given element to the pool, if it does not yet occur in there.
		 * Note, that this method uses the allocator which is locked before calling.
		 * @param _element The element to add to the pool.
		 * @return The given element, if it did not yet occur in the pool;
		 *         The equivalent element already occurring in the pool, otherwise.
		 */
		ConstElementPtr add(ElementPtr _element)
		{
			return *(insert(_element).first);
		}
	};
} // namespace carl

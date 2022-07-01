#pragma once

#include "../config.h"
#include "../datastructures/Bitset.h"

#include <iostream>
#include <mutex>

namespace carl {

	class IDPool {
	private:
		Bitset mFreeIDs = Bitset(true);
		std::size_t mLargestID = 0;
#ifdef THREAD_SAFE
		mutable std::mutex mMutex;
#define IDPOOL_LOCK std::lock_guard<std::mutex> lock(mMutex)
#else
#define IDPOOL_LOCK
#endif
	public:
		std::size_t size() const {
			IDPOOL_LOCK;
			return mFreeIDs.size();
		}
		std::size_t largestID() const {
			IDPOOL_LOCK;
			return mLargestID;
		}
		std::size_t get() {
			IDPOOL_LOCK;
			std::size_t pos = mFreeIDs.find_first();
			if (pos == Bitset::npos) {
				pos = mFreeIDs.size();
				mFreeIDs.resize((mFreeIDs.num_blocks() + 1) * Bitset::bits_per_block);
			}
			mFreeIDs.reset(pos);
			if (pos > mLargestID) mLargestID = pos;
			return pos;
		}
		void free(std::size_t id) {
			IDPOOL_LOCK;
			assert(id < mFreeIDs.size());
			mFreeIDs.set(id);
		}
		void clear() {
			IDPOOL_LOCK;
			mFreeIDs = Bitset(true);
		}
		friend std::ostream& operator<<(std::ostream& os, const IDPool& p) {
			return os << "Free: " << p.mFreeIDs;
		}

#undef IDPOOL_LOCK
	};

}

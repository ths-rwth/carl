#pragma once

#include "../config.h"
#include "Bitset.h"

#include <iostream>

namespace carl {

	class IDPool {
	private:
		Bitset mFreeIDs = Bitset(true);
#ifdef THREAD_SAFE
		std::mutex mMutex;
#endif
	public:
		std::size_t nextID() const {
#ifdef THREAD_SAFE
			std::lock_guard<std::mutex> lock(mMutex);
#endif
			return mFreeIDs.find_first();
		}
		std::size_t get() {
#ifdef THREAD_SAFE
			std::lock_guard<std::mutex> lock(mMutex);
#endif
			std::size_t pos = mFreeIDs.find_first();
			if (pos == Bitset::npos) {
				pos = mFreeIDs.size();
				mFreeIDs.resize((mFreeIDs.num_blocks() + 1) * Bitset::bits_per_block);
			}
			mFreeIDs.reset(pos);
			return pos;
		}
		void free(std::size_t id) {
#ifdef THREAD_SAFE
			std::lock_guard<std::mutex> lock(mMutex);
#endif
			assert(id < mFreeIDs.size());
			mFreeIDs.set(id);
		}
		void clear() {
#ifdef THREAD_SAFE
			std::lock_guard<std::mutex> lock(mMutex);
#endif
			mFreeIDs = Bitset(true);
		}
		friend std::ostream& operator<<(std::ostream& os, const IDPool& p) {
			return os << "Free: " << p.mFreeIDs;
		}
	};

}

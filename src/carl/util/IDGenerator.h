/**
 * @file IDGenerator.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <queue>
#include <stack>
#include <mutex>

namespace carl {

#define SMART_ID_GENERATOR
#ifdef SMART_ID_GENERATOR

class IDGenerator {
private:
	std::size_t mNext;
	std::priority_queue<std::size_t> mFree;
	mutable std::mutex mMutex;
public:
	IDGenerator(): mNext(1) {}
	std::size_t get() {
		std::lock_guard<std::mutex> lock(mMutex);
		std::size_t res = mNext;
		if (mFree.empty()) mNext++;
		else {
			res = mFree.top();
			mFree.pop();
		}
		return res;
	}

	void free(std::size_t id) {
		assert(id > 0);
		std::lock_guard<std::mutex> lock(mMutex);
		if (id == mNext-1) {
			mNext--;
			while (!mFree.empty()) {
				if (mFree.top() != mNext-1) break;
				mNext--;
				mFree.pop();
			}
		} else {
			mFree.push(id);
		}
	}
	std::size_t nextID() const {
		std::lock_guard<std::mutex> lock(mMutex);
		return mNext;
	}
};

#else

class IDGenerator {
private:
	std::size_t mNext;
	std::stack<std::size_t> mFree;
public:
	IDGenerator(): mNext(1) {}
	std::size_t get() {
		std::size_t res = mNext;
		if (mFree.empty()) mNext++;
		else {
			res = mFree.top();
			mFree.pop();
		}
		return res;
	}

	void free(std::size_t id) {
		mFree.push(id);
	}
	std::size_t nextID() const {
		return mNext;
	}
};

#endif


}

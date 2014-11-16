/**
 * @file IDGenerator.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <queue>
#include <stack>

namespace carl {

#define SMART_ID_GENERATOR
#ifdef SMART_ID_GENERATOR

class IDGenerator {
private:
	std::size_t mNext;
	std::priority_queue<std::size_t> mFree;
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
		if (id == mNext-1) {
			mNext--;
			while (mFree.top() == mNext-1) {
				mNext--;
				mFree.pop();
			}
		} else {
			mFree.push(id);
		}
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
};

#endif


}
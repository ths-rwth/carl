/**
 * @file Variables.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>
#include <vector>

#include <carl/core/Variable.h>

namespace carl {
namespace cad {

class Variables final {
	/**
	 * Fix list of variables for all computations.
	 */
	std::vector<Variable> mCurVars;
	
	/**
	 * list of new variables introduced by the scheduled elimination polynomials (mNewvariables and mVeriables are disjoint)
	 */
	std::vector<Variable> mNewVars;
public:
	bool empty() const {
		return mCurVars.empty();
	}
	bool newEmpty() const {
		return mNewVars.empty();
	}
	
	std::size_t size() const {
		return mCurVars.size();
	}
	std::size_t newSize() const {
		return mNewVars.size();
	}
	
	std::vector<Variable>::const_iterator begin() const {
		return mCurVars.begin();
	}
	std::vector<Variable>::iterator begin() {
		return mCurVars.begin();
	}
	std::vector<Variable>::const_iterator end() const {
		return mCurVars.end();
	}
	std::vector<Variable>::iterator end() {
		return mCurVars.end();
	}
	std::vector<Variable>::const_reverse_iterator rbegin() const {
		return mCurVars.rbegin();
	}
	std::vector<Variable>::reverse_iterator rbegin() {
		return mCurVars.rbegin();
	}
	std::vector<Variable>::const_reverse_iterator rend() const {
		return mCurVars.rend();
	}
	std::vector<Variable>::reverse_iterator rend() {
		return mCurVars.rend();
	}
	
	const std::vector<Variable>& getCurrent() const {
		return mCurVars;
	}
	
	void clear() {
		mCurVars.clear();
		mNewVars.clear();
	}
	
	bool contains(Variable::Arg v) const {
		if (std::find(mCurVars.begin(), mCurVars.end(), v) != mCurVars.end()) return true;
		if (std::find(mNewVars.begin(), mNewVars.end(), v) != mNewVars.end()) return true;
		return false;
	}
	Variable front() const {
		return mCurVars.front();
	}
	Variable first() const {
		assert(!empty() || !newEmpty());
		if (!empty()) return mCurVars.front();
		if (!newEmpty()) return mNewVars.front();
		return Variable::NO_VARIABLE;
	}
	
	void setNewVariables(const std::vector<Variable>& v) {
		mNewVars = v;
	}
	void addNew(Variable::Arg v) {
		mNewVars.push_back(v);
	}
	
	void appendNewToCur() {
		mNewVars.insert(mNewVars.end(), mCurVars.begin(), mCurVars.end());
		mCurVars.clear();
		std::swap(mCurVars, mNewVars);
	}
	
	/**
	 * Adds all variables that are not already present to the new variables.
     * @param v List of variables.
     */
	void complete(const std::vector<Variable>& v) {
		for (const auto& it : v) {
			if (!contains(it)) addNew(it);
		}
	}
	
	Variable::Arg operator[](std::size_t i) const {
		assert(i < mCurVars.size());
		return mCurVars[i];
	}
	Variable& operator[](std::size_t i) {
		assert(i < mCurVars.size());
		return mCurVars[i];
	}
	
	std::size_t indexOf(Variable::Arg v) {
		for (std::size_t i = 0; i < this->mCurVars.size(); i++) {
			if (v == mCurVars[i]) return i;
		}
		assert(false);
		return 0;
	}
	
	friend std::ostream& operator<<(std::ostream& os, const Variables& v);
};

inline std::ostream& operator<<(std::ostream& os, const Variables& v) {
	bool first = true;
	for (const auto& it: v.mCurVars) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	first = true;
	os << " || ";
	for (const auto& it: v.mNewVars) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os;
}

}
}

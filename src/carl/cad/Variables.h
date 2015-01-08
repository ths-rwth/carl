/**
 * @file Variables.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>
#include <vector>

#include "../core/Variable.h"

namespace carl {
namespace cad {

class Variables final {
	/**
	 * Fix list of variables for all computations.
	 */
	std::vector<Variable> curVars;
	
	/**
	 * list of new variables introduced by the scheduled elimination polynomials (mNewvariables and mVeriables are disjoint)
	 */
	std::vector<Variable> newVars;

public:
	bool empty() const {
		return curVars.empty();
	}
	bool newEmpty() const {
		return newVars.empty();
	}
	
	std::size_t size() const {
		return curVars.size();
	}
	std::size_t newSize() const {
		return newVars.size();
	}
	
	std::vector<Variable>::const_iterator begin() const {
		return curVars.begin();
	}
	std::vector<Variable>::iterator begin() {
		return curVars.begin();
	}
	std::vector<Variable>::const_iterator end() const {
		return curVars.end();
	}
	std::vector<Variable>::iterator end() {
		return curVars.end();
	}
	
	const std::vector<Variable>& getCurrent() const {
		return curVars;
	}
	
	void clear() {
		curVars.clear();
		newVars.clear();
	}
	
	bool contains(Variable::Arg v) const {
		if (std::find(curVars.begin(), curVars.end(), v) != curVars.end()) return true;
		if (std::find(newVars.begin(), newVars.end(), v) != newVars.end()) return true;
		return false;
	}
	Variable front() const {
		return curVars.front();
	}
	Variable first() const {
		assert(!empty() || !newEmpty());
		if (!empty()) return curVars.front();
		if (!newEmpty()) return newVars.front();
		return Variable::NO_VARIABLE;
	}
	
	void setNewVariables(const std::vector<Variable>& v) {
		newVars = v;
	}
	void addNew(Variable::Arg v) {
		newVars.push_back(v);
	}
	
	void appendNewToCur() {
		newVars.insert(newVars.end(), curVars.begin(), curVars.end());
		curVars.clear();
		std::swap(curVars, newVars);
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
	
	Variable operator[](std::size_t i) const {
		return curVars[i];
	}
	Variable& operator[](std::size_t i) {
		return curVars[i];
	}
	
	std::size_t indexOf(Variable::Arg v) {
		for (std::size_t i = 0; i < this->curVars.size(); i++) {
			if (v == curVars[i]) return i;
		}
		assert(false);
		return 0;
	}
	
	friend std::ostream& operator<<(std::ostream& os, const Variables& v);
};

inline std::ostream& operator<<(std::ostream& os, const Variables& v) {
	bool first = true;
	for (const auto& it: v.curVars) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	first = true;
	os << " || ";
	for (const auto& it: v.newVars) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os;
}

}
}
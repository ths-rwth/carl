#pragma once

#include "Variable.h"
#include "../io/streamingOperators.h"
#include "../formula/bitvector/BVVariable.h"
#include "../formula/uninterpreted/UVariable.h"
#include "../util/SFINAE.h"

#include <algorithm>
#include <variant>
#include <vector>

namespace carl {

class variable_type_filter {
	bool filter_bool = true;
	bool filter_real = true;
	bool filter_int = true;
	bool filter_uninterpreted = true;
	bool filter_bitvector = true;

public:
	static variable_type_filter all() {
		return variable_type_filter();
	}	
	static variable_type_filter excluding(std::initializer_list<VariableType> i) {
		auto res = variable_type_filter();
		for (const auto t : i) {
			if (t == VariableType::VT_BOOL) res.filter_bool = false;
			else if (t == VariableType::VT_REAL) res.filter_real = false;
			else if (t == VariableType::VT_INT) res.filter_int = false;
			else if (t == VariableType::VT_UNINTERPRETED) res.filter_uninterpreted = false;
			else if (t == VariableType::VT_BITVECTOR) res.filter_bitvector = false;
		}
		return res;
	}
	static variable_type_filter only(std::initializer_list<VariableType> i) {
		auto res = variable_type_filter();
		res.filter_bool = false;
		res.filter_real = false;
		res.filter_int = false;
		res.filter_uninterpreted = false;
		res.filter_bitvector = false;
		for (const auto t : i) {
			if (t == VariableType::VT_BOOL) res.filter_bool = true;
			else if (t == VariableType::VT_REAL) res.filter_real = true;
			else if (t == VariableType::VT_INT) res.filter_int = true;
			else if (t == VariableType::VT_UNINTERPRETED) res.filter_uninterpreted = true;
			else if (t == VariableType::VT_BITVECTOR) res.filter_bitvector = true;
		}
		return res;
	}

	bool apply(VariableType v) const {
		return (
			(filter_bool && v == VariableType::VT_BOOL) ||
			(filter_real && v == VariableType::VT_REAL) ||
			(filter_int && v == VariableType::VT_INT) ||
			(filter_uninterpreted && v == VariableType::VT_UNINTERPRETED) ||
			(filter_bitvector && v == VariableType::VT_BITVECTOR)
		);
	}
	bool apply(Variable v) const {
		return apply(v.type());
	}
};
class carlVariables {
public:
	friend bool operator==(const carlVariables& lhs, const carlVariables& rhs);
	friend std::ostream& operator<<(std::ostream& os, const carlVariables& vars);
	using iterator = std::vector<Variable>::iterator;
	using const_iterator = std::vector<Variable>::const_iterator;
private:
	mutable std::vector<Variable> mVariables;
	mutable std::size_t mAddedSinceCompact = 0;
	variable_type_filter mFilter;

	void compact(bool force = false) const {
		if ((force && mAddedSinceCompact > 0) || (mAddedSinceCompact > mVariables.size() / 2)) {
			std::sort(mVariables.begin(), mVariables.end());
			mVariables.erase(std::unique(mVariables.begin(), mVariables.end()), mVariables.end());
			mAddedSinceCompact = 0;
		}
	}
public:
	carlVariables(variable_type_filter filter = variable_type_filter::all()) : mFilter(filter) {};
	explicit carlVariables(std::initializer_list<Variable> i, variable_type_filter filter = variable_type_filter::all()) : mFilter(filter) {
		add(i);
	}
	template<typename Iterator>
	explicit carlVariables(const Iterator& b, const Iterator& e, variable_type_filter filter = variable_type_filter::all()) : mFilter(filter) {
		add(b, e);
	}

	auto begin() const {
		compact(true);
		return mVariables.begin();
	}
	auto end() const {
		return mVariables.end();
	}
	auto begin() {
		compact(true);
		return mVariables.begin();
	}
	auto end() {
		return mVariables.end();
	}

	bool empty() const {
		return mVariables.empty();
	}
	std::size_t size() const {
		compact(true);
		return mVariables.size();
	}
	void clear() {
		mVariables.clear();
		mAddedSinceCompact = 0;
	}
	bool has(Variable var) const {
		return std::find(mVariables.begin(), mVariables.end(), var) != mVariables.end();
	}

	void add(Variable v) {
		if (mFilter.apply(v)) {
			mVariables.emplace_back(v);
			++mAddedSinceCompact;
			compact();
		}
	}
	template<typename Iterator>
	void add(const Iterator& b, const Iterator& e) {
		std::for_each(b, e,
			[this](const auto& v){ 
				if (mFilter.apply(v)) {
					mVariables.emplace_back(v);
					++mAddedSinceCompact;
				}
			 }
		);
		compact();
	}
	void add(std::initializer_list<Variable> i) {
		add(i.begin(), i.end());
	}

	void erase(Variable v) {
		mVariables.erase(std::remove(mVariables.begin(), mVariables.end(), v), mVariables.end());
	}

	const std::vector<Variable>& as_vector() const {
		compact(true);
		return mVariables;
	}

	std::set<Variable> as_set() const {
		compact(true);
		return std::set<Variable>(mVariables.begin(), mVariables.end());
	}

	carlVariables filter(variable_type_filter&& f) const {
		return carlVariables(begin(), end(), f);
	}

	auto boolean() const {
		return filter(variable_type_filter::only({VariableType::VT_BOOL}));
	}
	auto integer() const {
		return filter(variable_type_filter::only({VariableType::VT_INT}));
	}
	auto real() const {
		return filter(variable_type_filter::only({VariableType::VT_REAL}));
	}
	auto arithmetic() const {
		return filter(variable_type_filter::only({VariableType::VT_REAL, VariableType::VT_INT}));
	}
	auto bitvector() const {
		return filter(variable_type_filter::only({VariableType::VT_BITVECTOR}));
	}
	auto uninterpreted() const {
		return filter(variable_type_filter::only({VariableType::VT_UNINTERPRETED}));
	}
};

inline void swap(Variable& lhs, Variable& rhs) {
	auto tmp = lhs;
	lhs = rhs;
	rhs = tmp;
}

inline bool operator==(const carlVariables& lhs, const carlVariables& rhs) {
	lhs.compact(true);
	rhs.compact(true);
	return lhs.mVariables == rhs.mVariables;
}
inline std::ostream& operator<<(std::ostream& os, const carlVariables& vars) {
	return os << vars.mVariables;
}

/// Return the variables as collected by the methods above.
template<typename T>
inline carlVariables variables(const T& t) {
	carlVariables vars;
	variables(t, vars);
	return vars;
}

template<typename T>
inline carlVariables boolean_variables(const T& t) {
	carlVariables vars(variable_type_filter::only({VariableType::VT_BOOL}));
	variables(t, vars);
	return vars;
}

template<typename T>
inline carlVariables integer_variables(const T& t) {
	carlVariables vars(variable_type_filter::only({VariableType::VT_INT}));
	variables(t, vars);
	return vars;
}

template<typename T>
inline carlVariables real_variables(const T& t) {
	carlVariables vars(variable_type_filter::only({VariableType::VT_REAL}));
	variables(t, vars);
	return vars;
}

template<typename T>
inline carlVariables arithmetic_variables(const T& t) {
	carlVariables vars(variable_type_filter::only({VariableType::VT_INT, VariableType::VT_REAL}));
	variables(t, vars);
	return vars;
}

template<typename T>
inline carlVariables bitvector_variables(const T& t) {
	carlVariables vars(variable_type_filter::only({VariableType::VT_BITVECTOR}));
	variables(t, vars);
	return vars;
}

template<typename T>
inline carlVariables uninterpreted_variables(const T& t) {
	carlVariables vars(variable_type_filter::only({VariableType::VT_UNINTERPRETED}));
	variables(t, vars);
	return vars;
}

}
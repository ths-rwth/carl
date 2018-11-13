#pragma once

#include "Variable.h"
#include "../formula/bitvector/BVVariable.h"
#include "../formula/uninterpreted/UVariable.h"

#include <algorithm>
#include <variant>
#include <vector>

namespace carl {

class carlVariables {
public:
	friend bool operator==(const carlVariables& lhs, const carlVariables& rhs);
	friend std::ostream& operator<<(std::ostream& os, const carlVariables& vars);
private:
	using VarTypes = std::variant<Variable,BVVariable,UVariable>;
	std::vector<VarTypes> mVariables;
public:
	carlVariables() = default;
	explicit carlVariables(std::initializer_list<VarTypes> i):
		mVariables(i)
	{}
	template<typename Iterator>
	explicit carlVariables(const Iterator& b, const Iterator& e) {
		add(b, e);
	}
	template<typename Iterator, typename F>
	carlVariables(const Iterator& b, const Iterator& e, F&& f) {
		add(b, e, std::forward<F>(f));
	}

	auto begin() const {
		return mVariables.begin();
	}
	auto end() const {
		return mVariables.end();
	}
	auto begin() {
		return mVariables.begin();
	}
	auto end() {
		return mVariables.end();
	}

	void compact() {
		CARL_LOG_DEBUG("carl.core", "Before: " << *this);
		std::sort(begin(), end());
		CARL_LOG_DEBUG("carl.core", "Sorted: " << *this);
		mVariables.erase(std::unique(begin(), end()), mVariables.end());
		CARL_LOG_DEBUG("carl.core", "After:  " << *this);
	}

	void add(VarTypes v) {
		mVariables.emplace_back(v);
	}
	void add(std::initializer_list<VarTypes> i) {
		mVariables.insert(end(), i.begin(), i.end());
	}
	template<typename Iterator>
	void add(const Iterator& b, const Iterator& e) {
		mVariables.insert(end(), b, e);
	}
	template<typename Iterator, typename F>
	void add(const Iterator& b, const Iterator& e, F&& f) {
		std::for_each(b, e,
			[this, &f](const auto& elem){ mVariables.emplace_back(f(elem)); }
		);
	}

	void erase(VarTypes v) {
		mVariables.erase(std::remove(begin(), end(), v));
	}

	template<typename F>
	carlVariables filter(F&& f) const {
		carlVariables res;
		std::copy_if(begin(), end(), std::back_inserter(res.mVariables), std::forward<F>(f));
		return res;
	}
	std::vector<Variable> underlyingVariables() const {
		std::vector<Variable> res;
		std::for_each(begin(), end(), [&res](const auto& var) {
			std::visit(overloaded {
				[&res](Variable v){ res.emplace_back(v); },
				[&res](BVVariable v){ res.emplace_back(v.variable()); },
				[&res](UVariable v){ res.emplace_back(v.variable()); },
			}, var);
		});
		return res;
	}
};

inline bool operator==(const carlVariables& lhs, const carlVariables& rhs) {
	return lhs.mVariables == rhs.mVariables;
}
inline std::ostream& operator<<(std::ostream& os, const carlVariables& vars) {
	return os << vars.mVariables;
}

}
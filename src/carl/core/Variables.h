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
	using VarTypes = std::variant<Variable,BVVariable,UVariable>;
private:
	mutable std::vector<VarTypes> mVariables;
	mutable std::size_t mAddedSinceCompact = 0;

	void compact(bool force = false) const {
		if (force || (mAddedSinceCompact > mVariables.size() / 2)) {
			std::sort(mVariables.begin(), mVariables.end());
			mVariables.erase(std::unique(mVariables.begin(), mVariables.end()), mVariables.end());
			mAddedSinceCompact = 0;
		}
	}
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
		compact();
		return mVariables.begin();
	}
	auto end() const {
		return mVariables.end();
	}
	auto begin() {
		compact();
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
	void add(VarTypes v) {
		mVariables.emplace_back(v);
		++mAddedSinceCompact;
		compact();
	}
	void add(std::initializer_list<VarTypes> i) {
		mVariables.insert(end(), i.begin(), i.end());
		mAddedSinceCompact += i.size();
		compact();
	}
	template<typename Iterator>
	void add(const Iterator& b, const Iterator& e) {
		mVariables.insert(end(), b, e);
		mAddedSinceCompact += static_cast<std::size_t>(std::distance(b, e));
		compact();
	}
	template<typename Iterator, typename F>
	void add(const Iterator& b, const Iterator& e, F&& f) {
		std::for_each(b, e,
			[this, &f](const auto& elem){ this->add(f(elem)); }
		);
	}

	void erase(VarTypes v) {
		mVariables.erase(std::remove(begin(), end(), v), end());
	}

	template<typename F>
	carlVariables filter(F&& f) const {
		carlVariables res;
		std::copy_if(begin(), end(), std::back_inserter(res.mVariables), std::forward<F>(f));
		return res;
	}
	std::vector<Variable> underlyingVariables() const {
		compact(true);
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

	template<typename T>
	auto filter_type() const {
		return filter([](const auto& v) {
			return std::holds_alternative<T>(v);
		});
	}
	auto filter_type(VariableType vt) const {
		return filter([vt](const auto& v) {
			return std::holds_alternative<carl::Variable>(v) && std::get<carl::Variable>(v).type() == vt;
		});
	}

	auto boolean() const {
		return filter_type(VariableType::VT_BOOL);
	}
	auto integer() const {
		return filter_type(VariableType::VT_INT);
	}
	auto real() const {
		return filter_type(VariableType::VT_REAL);
	}
	auto bitvector() const {
		return filter_type<BVVariable>();
	}
	auto uninterpreted() const {
		return filter_type<BVVariable>();
	}
};

inline void swap(carlVariables::VarTypes& lhs, carlVariables::VarTypes& rhs) {
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

}
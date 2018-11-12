#pragma once

#include "Variable.h"

#include <algorithm>
#include <vector>

namespace carl {

class carlVariables {
public:
	friend bool operator==(const carlVariables& lhs, const carlVariables& rhs);
private:
	std::vector<Variable> mVariables;
public:
	carlVariables() = default;
	explicit carlVariables(std::initializer_list<Variable> i):
		mVariables(i)
	{}
	template<typename Iterable>
	explicit carlVariables(const Iterable& container) {
		add(container);
	}
	template<typename Iterable, typename F>
	carlVariables(const Iterable& container, F&& f) {
		add(container, std::forward<F>(f));
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
		std::sort(begin(), end());
		mVariables.erase(std::unique(begin(), end()));
	}

	void add(Variable v) {
		mVariables.emplace_back(v);
	}
	void add(std::initializer_list<Variable> i) {
		mVariables.insert(end(), i.begin(), i.end());
	}
	template<typename Iterable>
	void add(const Iterable& container) {
		mVariables.insert(end(), container.begin(), container.end());
	}
	template<typename Iterable, typename F>
	void add(const Iterable& container, F&& f) {
		for (const auto& elem: container) {
			mVariables.emplace_back(f(elem));
		}
	}

	void erase(Variable v) {
		mVariables.erase(std::remove(begin(), end(), v));
	}

	template<typename F>
	carlVariables filter(F&& f) const {
		carlVariables res;
		std::copy_if(begin(), end(), std::back_inserter(res.mVariables), std::forward<F>(f));
		return res;
	}
};

inline bool operator==(const carlVariables& lhs, const carlVariables& rhs) {
	return lhs.mVariables == rhs.mVariables;
}

}
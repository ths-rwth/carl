#pragma once

#include <carl-logging/carl-logging.h>

#include "ModelVariable.h"
#include "ModelValue.h"

namespace carl
{
	/**
	 * Represent a collection of assignments/mappings from variables to values.
	 * We use a ModelVariable to abstract over the different kinds of variables
	 * in CARL, and a ModelValue to abstract over the different kinds of values
	 * for these variables.
	 * Most notably, a value can be a "carl::ModelSubstitution" whose value depends
	 * on the values of other variables in the Model.
	 */
	template<typename Rational, typename Poly>
	class Model {
	public:
		using key_type = ModelVariable;
		using mapped_type = ModelValue<Rational,Poly>;
		using Map = std::map<key_type,mapped_type>;
		static_assert(std::is_same<key_type, typename Map::key_type>::value, "Should be the same type");
		static_assert(std::is_same<mapped_type, typename Map::mapped_type>::value, "Should be the same type");
	private:
		Map mData;
		std::map<key_type, std::size_t> mUsedInSubstitution;
		void resetCaches() const {
			for (const auto& d: mData) {
				if (d.second.isSubstitution()) {
					d.second.asSubstitution()->resetCache();
				}
			}
		}
	public:
		// Element access
		const auto& at(const key_type& key) const {
			return mData.at(key);
		}
		
		// Iterators
		auto begin() const {
			return mData.begin();
		}
		auto end() const {
			return mData.end();
		}
		// Capacity
		auto empty() const {
			return mData.empty();
		}
		auto size() const {
			return mData.size();
		}
		// Modifiers
		void clear() {
			mData.clear();
		}
		template<typename P>
		auto insert(const P& pair) {
			resetCaches();
			return mData.insert(pair);
		}
		template<typename P>
		auto insert(typename Map::const_iterator it, const P& pair) {
			resetCaches();
			return mData.insert(it, pair);
		}
		template<typename... Args>
		auto emplace(const key_type& key, Args&& ...args) {
			resetCaches();
			return mData.emplace(key,std::forward<Args>(args)...);
		}
		template<typename... Args>
		auto emplace_hint(typename Map::const_iterator it, const key_type& key, Args&& ...args) {
			resetCaches();
			return mData.emplace_hint(it, key,std::forward<Args>(args)...);
		}
		typename Map::iterator erase(const ModelVariable& variable) {
			resetCaches();
			return erase(mData.find(variable));
		}
		typename Map::iterator erase(const typename Map::iterator& it) {
			resetCaches();
			return erase(typename Map::const_iterator(it));
		}
		typename Map::iterator erase(const typename Map::const_iterator& it) {
			if (it == mData.end()) return mData.end();
			return mData.erase(it);
		}
        void clean() {
            for (auto& m: mData) {
				const auto& val = m.second;
				if (!val.isSubstitution()) continue;
				const auto& subs = val.asSubstitution();
                CARL_LOG_DEBUG("carl.formula.model", "Evaluating " << m.first << " ->  " << subs << " as.");
                m.second = subs->evaluate(*this);
			}
        }
		// Lookup
		auto find(const typename Map::key_type& key) const {
			return mData.find(key);
		}
		auto find(const typename Map::key_type& key) {
			return mData.find(key);
		}
		
		// Additional (w.r.t. std::map)
		Model() = default;
		Model(const std::map<Variable, Rational>& assignment) {
			for (const auto& a: assignment) {
				mData.emplace(a.first, a.second);
			}
		}
		template<typename Container>
		bool contains(const Container& c) const {
			for (const auto& var: c) {
				if (mData.find(var) == mData.end()) return false;
			}
			return true;
		}
		template<typename T>
		void assign(const typename Map::key_type& key, const T& t) {
			auto it = mData.find(key);
			if (it == mData.end()) mData.emplace(key, t);
			else it->second = t;
		}
		void update(const Model& model, bool disjoint = true) {
			for (const auto& m: model) {
				auto res = mData.insert(m);
				if (disjoint) {
					assert(res.second);
				} else {
					if (!res.second) {
						res.first->second = m.second;
					}
				}
			}
		}
		/**
		 * Return the ModelValue for the given key, evaluated if it's a ModelSubstitution and evaluatable,
		 * otherwise return it raw.
		 * @param key The model must contain an assignment with the given key.
		 */
		const ModelValue<Rational,Poly>& evaluated(const typename Map::key_type& key) const {
			const auto& it = at(key);
			if (it.isSubstitution()) return it.asSubstitution()->evaluate(*this);
			else return it;
		}
		void print(std::ostream& os, bool simple = true) const {
			os << "(model" << std::endl;
			for (const auto& ass: mData) {
				auto value = ass.second;
				if (simple) value = evaluated(ass.first);

				if (ass.first.isVariable()) {
					os << "\t(define-fun " << ass.first << " () " << ass.first.asVariable().type() << std::endl;
					os << "\t\t" << value << ")" << std::endl;
				} else if (ass.first.isBVVariable()) {
					os << "\t(define-fun " << ass.first << " () " << ass.first.asBVVariable().sort() << std::endl;
					os << "\t\t" << value << ")" << std::endl;
				} else if (ass.first.isUVariable()) {
					os << "\t(define-fun " << ass.first << " () " << ass.first.asUVariable().domain() << std::endl;
					os << "\t\t" << value << ")" << std::endl;
				} else if (ass.first.isFunction()) {
					os << value;
				} else {
					CARL_LOG_ERROR("carl.model", "Encountered an unknown type of ModelVariable: " << ass.first);
					assert(false);
				}
			}
			os << ")" << std::endl;
		}
		void printOneline(std::ostream& os, bool simple = false) const {
			os << "{";
			bool first = true;
			for (const auto& ass: mData) {
				if (!first) os << ", ";
				auto value = ass.second;
				if (simple) value = evaluated(ass.first);
				
				if (ass.first.isVariable()) {
					os << ass.first << " = " << value;
				} else if (ass.first.isBVVariable()) {
					os << ass.first << " = " << ass.first.asBVVariable().sort() << "(" << value << ")";
				} else if (ass.first.isUVariable()) {
					os << ass.first << " = " << ass.first.asUVariable().domain() << "(" << value << ")";
				} else if (ass.first.isFunction()) {
					os << value;
				} else {
					CARL_LOG_ERROR("carl.model", "Encountered an unknown type of ModelVariable: " << ass.first);
					assert(false);
				}
				first = false;
			}
			os << "}";
		}
	};

	template<typename Rational, typename Poly>
	std::ostream& operator<<(std::ostream& os, const Model<Rational,Poly>& model) {
		model.printOneline(os);
		return os;
	}
}

#include "ModelSubstitution.h"

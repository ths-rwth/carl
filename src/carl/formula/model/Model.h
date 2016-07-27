#pragma once

#include "../../core/logging.h"

#include "ModelVariable.h"
#include "ModelValue.h"

namespace carl
{
	/**
	 * This class represents a model, that is an assignment for variables to some values.
	 * A variable can be assigned to different values that are represented by a ModelValue.
	 * Most notably, a value may be a substitution based on the values of other variables.
	 * If a variable that is used by a substitution for another variable is erased from the model, this substitution is evaluated and replaced by the result.
	 */
	template<typename Rational, typename Poly>
	class Model {
	public:
		//using Map = std::unordered_map<ModelVariable,ModelValue>;
		using Map = std::map<ModelVariable,ModelValue<Rational,Poly>>;
	private:
		Map mData;
		std::map<ModelVariable, std::size_t> mUsedInSubstitution;
		void resetCaches() const {
			for (const auto& d: mData) {
				if (d.second.isSubstitution()) {
					d.second.asSubstitution()->resetCache();
				}
			}
		}
	public:
		// Element access
		const auto& at(const typename Map::key_type& key) const {
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
		auto emplace(const typename Map::key_type& key, Args&& ...args) {
			resetCaches();
			return mData.emplace(key,std::forward<Args>(args)...);
		}
		template<typename... Args>
		auto emplace_hint(typename Map::const_iterator it, const typename Map::key_type& key, Args&& ...args) {
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
			for (auto& m: mData) {
				const auto& val = m.second;
				if (!val.isSubstitution()) continue;
				const auto& subs = val.asSubstitution();
				if (subs->dependsOn(it->first)) {
					CARL_LOG_DEBUG("carl.formula.model", "Evaluating " << m.first << " ->  " << subs << " as " << it->first << " is removed from the model.");
					m.second = subs->evaluate(*this);
				}
			}
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
					os << "\t(define-fun " << ass.first << " () " << ass.first.asVariable().getType() << std::endl;
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
	};

	template<typename Rational, typename Poly>
	std::ostream& operator<<(std::ostream& os, const Model<Rational,Poly>& model) {
		model.print(os);
		return os;
	}

	template<typename Rational, typename Poly>
	static const Model<Rational,Poly> EMPTY_MODEL = Model<Rational,Poly>();
}

#include "ModelSubstitution.h"

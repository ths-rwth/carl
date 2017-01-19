/**
 * @file SortManager.cpp
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "SortManager.h"

namespace carl
{
	
	std::ostream& SortManager::print(std::ostream& os, const Sort& sort) const {
		const SortContent& sc = getContent(sort);
		if (sc.indices != nullptr) os << "(_ ";
		if (sc.parameters != nullptr) os << "(";
		os << sc.name;
		if (sc.parameters != nullptr) {
			for (const Sort& s: *sc.parameters) os << " " << s;
			os << ")";
		}
		if (sc.indices != nullptr) {
			for (std::size_t i: *sc.indices) os << " " << i;
			os << ")";
		}
		return os;
	}
	
	void SortManager::exportDefinitions(std::ostream& os) const {
		for (const auto& decl: mDeclarations) {
			os << "(declare-sort " << decl.first << " " << decl.second << ")" << std::endl;
		}
		///@todo fix this
		//for (const auto& def: mDeclarations) {
		//	os << "(define-sort " << decl.first << " " << decl.second << ")" << std::endl;
		//}
	}
	
	Sort SortManager::replace(const Sort& sort, const std::map<std::string, Sort>& parameters) {
		const SortContent& sc = getContent(sort);
		auto pIter = parameters.find(sc.name);
		if (pIter != parameters.end()) return pIter->second;
		if (sc.parameters == nullptr) return sort;

		std::vector<Sort> v;
		v.reserve(sc.parameters->size());
		for (const auto& sd: *sc.parameters) v.push_back(replace(sd, parameters));
		return getSort(new SortContent(sc.name, std::move(v)), VariableType::VT_UNINTERPRETED);
	}
	
	bool SortManager::declare(const std::string& name, std::size_t arity) {
		if (!isSymbolFree(name)) return false;
		mDeclarations[name] = arity;
		if (arity == 0) addSortContent(new SortContent(name), VariableType::VT_UNINTERPRETED);
		return true;
	}

	bool SortManager::define(const std::string& name, const std::vector<std::string>& params, const Sort& sort) {
		if (!isSymbolFree(name)) return false;
		mDefinitions[name] = SortTemplate(params, sort);
		return true;
	}

	size_t SortManager::getArity(const Sort& sort) const {
		const SortContent& sc = getContent(sort);
		if (sc.parameters == nullptr) return 0;
		return sc.parameters->size();
	}
	
	Sort SortManager::addSort(const std::string& name, VariableType type) {
		assert(isSymbolFree(name));
		return Sort(addSortContent(new SortContent(name), type));
	}
	Sort SortManager::addSort(const std::string& name, const std::vector<Sort>& parameters, VariableType type) {
		assert(isSymbolFree(name));
		return Sort(addSortContent(new SortContent(name, parameters), type));
	}
	void SortManager::makeSortIndexable(const Sort& sort, std::size_t indices, VariableType type) {
		mIndexable[sort] = std::make_pair(indices, type);
	}
	
	Sort SortManager::index(const Sort& sort, const std::vector<std::size_t>& indices) {
		if (indices.empty()) return sort;
		const SortContent& sc = getContent(sort);
		auto newsc = new SortContent(sc);
		if (newsc->indices == nullptr) newsc->indices = std::make_unique<std::vector<std::size_t>>(indices);
		else newsc->indices->insert(newsc->indices->end(), indices.begin(), indices.end());
		return getSort(newsc, checkIndices(sc.getUnindexed(), newsc->indices->size()));
	}
	
	Sort SortManager::getSort(const std::string& name) {
		SortContent* sc = new SortContent(name);
		// Find an instantiation of the given sort template.
		auto it = mSortMap.find(sc);
		if(it == mSortMap.end()) {
			CARL_LOG_ERROR("carl.formula", "The sort " << name << " has not been declared or defined.");
			delete sc;
			return Sort(0);
		}
		delete sc;
		return Sort(it->second);
	}
	
	Sort SortManager::getSort(const std::string& name, const std::vector<Sort>& params) {
		assert(!params.empty());
		auto decl = mDeclarations.find(name);
		if (decl != mDeclarations.end()) {
			std::size_t arity = decl->second;
			if (arity != params.size()) {
				CARL_LOG_ERROR("carl.formula", "The sort " << name << " was declared to have " << arity << " parameters, but " << params.size() << " were given.");
				return Sort(0);
			}
			SortContent* sc = new SortContent(name, params);
			return getSort(sc, VariableType::VT_UNINTERPRETED);
		}
		auto def = mDefinitions.find(name);
		if (def != mDefinitions.end()) {
			const SortTemplate& st = def->second;
			if (st.first.size() != params.size()) {
				CARL_LOG_ERROR("carl.formula", "The sort " << name << " was defined to have " << st.first.size() << " parameters, but " << params.size() << " were given.");
				return Sort(0);
			}
			std::map<std::string,Sort> repl;
			for (std::size_t i = 0; i < params.size(); i++) repl[st.first[i]] = params[i];
			return replace(st.second, repl);
		}
		CARL_LOG_ERROR("carl.formula", "The sort " << name << " was neither declared nor defined and thus cannot be instantiated.");
		return Sort(0);
	}
	
	Sort SortManager::getSort(const std::string& name, const std::vector<std::size_t>& indices) {
		return index(getSort(name), indices);
	}
	
	Sort SortManager::getSort(const std::string& name, const std::vector<std::size_t>& indices, const std::vector<Sort>& params) {
		return index(getSort(name, params), indices);
	}
}

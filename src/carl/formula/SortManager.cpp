/**
 * @file SortManager.cpp
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#include "SortManager.h"

using namespace std;

namespace carl
{
	const string& SortManager::getName( const Sort& _sort ) const
	{
		assert( _sort.id() != 0 );
		assert( _sort.id() < mSorts.size() );
		return mSorts[_sort.id()]->name;
	}
	
	ostream& SortManager::print( ostream& _out, const Sort& _sort ) const
	{
		assert( _sort.id() < mSorts.size() );
		const SortContent& sc = *mSorts[_sort.id()];
		if (sc.indices != nullptr) _out << "(_ ";
		if (sc.parameters != nullptr) _out << "(";
		_out << getName(_sort);
		if (sc.parameters != nullptr) {
			for (const Sort& s: *sc.parameters) _out << " " << s;
			_out << ")";
		}
		if (sc.indices != nullptr) {
			for (std::size_t i: *sc.indices) _out << " " << i;
			_out << ")";
		}
		return _out;
	}
	
	Sort SortManager::replace( const Sort& _sort, const map<string, Sort>& _parameters )
	{
		assert( _sort.id() < mSorts.size() );
		const SortContent& sc = *mSorts[_sort.id()];
		auto pIter = _parameters.find(sc.name);
		if (pIter != _parameters.end()) return pIter->second;
		if (sc.parameters == nullptr) return _sort;

		vector<Sort> v;
		v.reserve(sc.parameters->size());
		for (const auto& sd : *sc.parameters) v.push_back(replace(sd, _parameters));
		return getSort(new SortContent( sc.name, std::move( v ) ));
	}
	
	bool SortManager::declare( const string& _name, unsigned _arity )
	{
		if (mDeclarations.count(_name) > 0) return false;
		mDeclarations[_name] = _arity;
		if (_arity == 0) getSort(new SortContent(_name));
		return true;
	}

	bool SortManager::define( const string& _name, const vector<string>& _params, const Sort& _sort )
	{
		if (mDefinitions.count(_name) > 0) return false;
		mDefinitions[_name] = SortTemplate(_params, _sort);
		return true;
	}
	
	size_t SortManager::arity( const string& _name ) const
	{
		auto decIter = mDeclarations.find(_name);
		if (decIter != mDeclarations.end()) return decIter->second;
		auto defIter = mDefinitions.find(_name);
		if (defIter != mDefinitions.end()) return defIter->second.first.size();
		cerr << "The sort " << _name << " has not been declared or defined." << endl;
		return 0;
	}

	size_t SortManager::getArity( const Sort& _sort ) const
	{
		assert(_sort.id() != 0);
		assert(_sort.id() < mSorts.size());
		const SortContent& sc = *mSorts[_sort.id()];
		if (sc.interpreted || sc.parameters == nullptr) return 0;
		return sc.parameters->size();
	}

	Sort SortManager::interpretedSort( const string& _name, VariableType _type )
	{
		SortContent* sc = new SortContent(_name, _type);
		assert(mSortcontentIdMap.find(sc) == mSortcontentIdMap.end());
		mSortcontentIdMap.emplace(sc, mSorts.size());
		Sort s(mSorts.size());
		mSorts.push_back(sc);
		mInterpretedSorts.emplace(_type, s);
		return s;
	}
	
	Sort SortManager::instantiate(const std::string& name, const std::vector<Sort>& params) {
		auto decl = mDeclarations.find(name);
		if (decl != mDeclarations.end()) {
			std::size_t arity = decl->second;
			if (arity != params.size()) {
				CARL_LOG_ERROR("carl.formula", "The sort " << name << " was declared to have " << arity << " parameters, but " << params.size() << " were given.");
				return Sort(0);
			}
			std::vector<Sort> tmp(params);
			SortContent* sc = new SortContent(name, std::move(tmp));
			return getSort(sc);
		}
		auto def = mDefinitions.find(name);
		if (def != mDefinitions.end()) {
			const SortTemplate& st = def->second;
			if (st.first.size() != params.size()) {
				CARL_LOG_ERROR("carl.formula", "The sort " << name << " was defined to have " << st.first.size() << " parameters, but " << params.size() << " were given.");
				return Sort(0);
			}
			std::map<std::string,Sort> repl;
			for (std::size_t i = 0; i < params.size(); i++) {
				repl[st.first[i]] = params[i];
			}
			return replace(st.second, repl);
		}
		CARL_LOG_ERROR("carl.formula", "The sort " << name << " was neither declared nor defined and thus cannot be instantiated.");
		return Sort(0);
	}
	
	Sort SortManager::index(const Sort& sort, const std::vector<std::size_t>& indices) {
		assert(sort.id() != 0);
		assert(sort.id() < mSorts.size());
		if (indices.size() == 0) return sort;
		const SortContent& sc = *mSorts[sort.id()];
		SortContent* newsc = new SortContent(sc);
		if (newsc->indices == nullptr) newsc->indices = new std::vector<std::size_t>(indices);
		else newsc->indices->insert(newsc->indices->end(), indices.begin(), indices.end());
		return getSort(newsc);
	}
	
	Sort SortManager::getSort( const SortContent* _sc )
	{
		auto nameIdPair = mSortcontentIdMap.find(_sc);
		// Check if this sort has already been created
		if (nameIdPair != mSortcontentIdMap.end()) {
			delete _sc;
			return Sort(nameIdPair->second);
		}
		// Create the sort
		mSortcontentIdMap.emplace(_sc, mSorts.size());
		Sort s(mSorts.size());
		mSorts.push_back(_sc);
		return s;
	}
	
	Sort SortManager::getSort( const string& _name )
	{
		SortContent* sc = new SortContent(_name);
		// Find an instantiation of the given sort template.
		auto nameIdPair = mSortcontentIdMap.find(sc);
		if(nameIdPair == mSortcontentIdMap.end()) {
			cerr << "The sort " << _name << " has not been declared or defined." << endl;
			delete sc;
			return Sort(0);
		}
		delete sc;
		return Sort(nameIdPair->second);
	}
	
	Sort SortManager::getSort( const string& _name, const vector<Sort>& _params )
	{
		return instantiate(_name, _params);
	}
	
	Sort SortManager::getSort( const std::string& _name, const std::vector<std::size_t>& _indices )
	{
		return index(getSort(_name), _indices);
	}
	
	Sort SortManager::getSort( const std::string& _name, const std::vector<std::size_t>& _indices, const std::vector<Sort>& _params )
	{
		return index(getSort(_name, _params), _indices);
	}
}

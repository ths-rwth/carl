/**
 * @file SortManager.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include <carl/util/pointerOperations.h>
#include <carl-common/memory/Singleton.h>
#include "Sort.h"
#include <carl/core/Variable.h>

#include <cassert>
#include <ostream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace carl {

/// The actual content of a sort.
struct SortContent {
	/// The sort's name.
	std::string name;
	/// The sort's argument types. It is nullptr, if the sort's arity is zero.
	std::unique_ptr<std::vector<Sort>> parameters;
	/// The sort's indices. A sort can be indexed with the "_" operator. It is nullptr, if no indices are present.
	std::unique_ptr<std::vector<std::size_t>> indices;

	SortContent() = delete; // The default constructor is disabled.

	/**
	 * Constructs a sort content.
	 * @param _name The name of the sort content to construct.
	 */
	explicit SortContent(std::string _name) noexcept
		: name(std::move(_name))
		{}

	/**
	 * Constructs a sort content.
	 * @param _name The name  of the sort content to construct.
	 * @param _parameters The sorts of the arguments of the sort content to construct.
	 */
	explicit SortContent(std::string _name, const std::vector<Sort>& _parameters)
		: name(std::move(_name)),
		  parameters(std::make_unique<std::vector<Sort>>(_parameters))
		{}
	explicit SortContent(std::string _name, std::vector<Sort>&& _parameters)
		: name(std::move(_name)),
		  parameters(std::make_unique<std::vector<Sort>>(std::move(_parameters)))
		{}

	SortContent(const SortContent& sc)
		: name(sc.name)
	{
		if (sc.parameters) parameters = std::make_unique<std::vector<Sort>>(*sc.parameters);
		if (sc.indices) indices = std::make_unique<std::vector<std::size_t>>(*sc.indices);
	}

	/// Destructs a sort content.
	~SortContent() noexcept = default;

	SortContent& operator=(const SortContent& sc) = delete;
	SortContent(SortContent&& sc) noexcept = default;
	SortContent& operator=(SortContent&& sc) = default;

	/**
	 * Return a copy of this SortContent without any indices.
	 */
	SortContent getUnindexed() const {
		if (parameters == nullptr) return SortContent(name);
		return SortContent(name, *parameters);
	}
};

/**
 * @param lhs Left SortContent
 * @param rhs Right SortContent
 * @return `lhs < rhs`
 */
inline bool operator<(const SortContent& lhs, const SortContent& rhs) {
	if (lhs.name != rhs.name) return lhs.name < rhs.name;

	if (!lhs.parameters && rhs.parameters) return true;
	if (lhs.parameters && !rhs.parameters) return false;
	if (lhs.parameters && rhs.parameters) {
		if (*lhs.parameters != *rhs.parameters) return *lhs.parameters < *rhs.parameters;
	}
	if (!lhs.indices && rhs.indices) return true;
	if (lhs.indices && !rhs.indices) return false;
	if (lhs.indices && rhs.indices) {
		if (*lhs.indices != *rhs.indices) return *lhs.indices < *rhs.indices;
	}
	return false;
}

/**
 * Implements a manager for sorts, containing the actual contents of these sort and allocating their ids.
 */
class SortManager : public Singleton<SortManager> {
	friend Singleton<SortManager>;

public:
	/// The type of a sort template, define by define-sort.
	using SortTemplate = std::pair<std::vector<std::string>, Sort>;

private:
	// Members.

	/// Maps the unique ids to the sort content.
	std::vector<std::unique_ptr<SortContent>> mSorts;
	/// Maps the unique ids to the sort types.
	std::vector<VariableType> mSortTypes;
	/// Maps the sort contents to unique ids.
	std::map<const SortContent*, std::size_t, carl::less<const SortContent*>> mSortMap;
	/// Stores all sort declarations invoked by a declare-sort.
	std::map<std::string, std::size_t> mDeclarations;
	/// Stores all sort definitions invoked by a define-sort.
	std::map<std::string, SortTemplate> mDefinitions;
	/// Stores all sorts that may become interpreted when indexed.
	std::map<Sort, std::pair<std::size_t, VariableType>> mIndexable;
	/// Maps variable types to actual sorts.
	std::map<VariableType, Sort> mInterpreted;

	/**
	 * Constructs a sort manager.
	 */
	SortManager() {
		clear();
	}

	const SortContent& getContent(const Sort& sort) const {
		assert(sort.id() > 0);
		assert(sort.id() < mSorts.size());
		return *mSorts.at(sort.id());
	}
	bool isSymbolFree(const std::string& name) const {
		for (const auto& s : mSorts) {
			if (s == nullptr) continue;
			if (s->name == name) return false;
		}
		if (mDeclarations.find(name) != mDeclarations.end()) return false;
		if (mDefinitions.find(name) != mDefinitions.end()) return false;
		return true;
	}
	Sort getSort(std::unique_ptr<SortContent>&& content, VariableType type) {
		auto it = mSortMap.find(content.get());
		if (it != mSortMap.end()) {
			return Sort(it->second);
		}
		return Sort(addSortContent(std::move(content), type));
	}

	std::size_t addSortContent(std::unique_ptr<SortContent>&& content, VariableType type) {
		mSorts.emplace_back(std::move(content));
		mSortTypes.push_back(type);
		mSortMap[mSorts.back().get()] = mSorts.size() - 1;
		return mSorts.size() - 1;
	}

	VariableType checkIndices(const SortContent& content, std::size_t count) const {
		auto sortIt = mSortMap.find(&content);
		if (sortIt == mSortMap.end()) return VariableType::VT_UNINTERPRETED;
		Sort baseSort(sortIt->second);
		auto it = mIndexable.find(baseSort);
		if (it == mIndexable.end()) return VariableType::VT_UNINTERPRETED;
		if (it->second.first != count) return VariableType::VT_UNINTERPRETED;
		return it->second.second;
	}

public:
	SortManager(const SortManager&) = delete;
	SortManager(SortManager&&) = delete;
	SortManager& operator=(const SortManager&) = delete;
	SortManager& operator=(SortManager&&) = delete;
	~SortManager() noexcept override = default;

	void clear() {
		mSorts.clear();
		mSortTypes.clear();
		mSortMap.clear();
		mDeclarations.clear();
		mDefinitions.clear();
		mIndexable.clear();
		mInterpreted.clear();
		mSorts.emplace_back(nullptr); // default value
		mSortTypes.emplace_back(VariableType::VT_UNINTERPRETED);
	}

	/**
	 * @param sort A sort.
	 * @return The name if the given sort.
	 */
	const std::string& getName(const Sort& sort) const {
		return getContent(sort).name;
	}
	const std::vector<Sort>* getParameters(const Sort& sort) const {
		return getContent(sort).parameters.get();
	}
	const std::vector<std::size_t>* getIndices(const Sort& sort) const {
		return getContent(sort).indices.get();
	}
	VariableType getType(const Sort& sort) const {
		assert(sort.id() > 0);
		assert(sort.id() < mSortTypes.size());
		return mSortTypes.at(sort.id());
	}

	/**
	 * Prints the given sort on the given output stream.
	 * @param os The output stream to print the given sort on.
	 * @param sort The sort to print.
	 * @return The output stream after printing the given sort on it.
	 */
	std::ostream& print(std::ostream& os, const Sort& sort) const;

	void exportDefinitions(std::ostream& os) const;

	Sort getInterpreted(VariableType type) const {
		assert(mInterpreted.find(type) != mInterpreted.end());
		return mInterpreted.at(type);
	}

	/**
	 * Recursively replaces sorts within the given sort according to the mapping of sort names to sorts as declared by the given map.
	 * @param sort The sort to replace sorts by sorts in.
	 * @param parameters The map of sort names to sorts.
	 * @return The resulting sort.
	 */
	Sort replace(const Sort& sort, const std::map<std::string, Sort>& parameters);

	/**
	 * Adds a sort declaration.
	 * @param name The name of the declared sort.
	 * @param arity The arity of the declared sort.
	 * @return true, if the given sort declaration has not been added before;
	 *		 false, otherwise.
	 */
	bool declare(const std::string& name, std::size_t arity);

	/**
	 * Adds a sort template definitions.
	 * @param name The name of the defined sort template.
	 * @param params The template parameter of the defined sort.
	 * @param sort The sort to instantiate into.
	 * @return true, if the given sort template definition has not been added before;
	 *		 false, otherwise.
	 */
	bool define(const std::string& name, const std::vector<std::string>& params, const Sort& sort);

	/**
	 * @param sort The sort to get the arity for.
	 * @return The arity of the given sort.
	 */
	std::size_t getArity(const Sort& sort) const;

	Sort addInterpretedMapping(const Sort& sort, VariableType type) {
		assert(mInterpreted.find(type) == mInterpreted.end());
		mInterpreted[type] = sort;
		return sort;
	}
	Sort addInterpretedSort(const std::string& name, VariableType type) {
		return addInterpretedMapping(addSort(name, type), type);
	}
	Sort addInterpretedSort(const std::string& name, const std::vector<Sort>& parameters, VariableType type) {
		return addInterpretedMapping(addSort(name, parameters, type), type);
	}
	Sort addSort(const std::string& name, VariableType type = VariableType::VT_UNINTERPRETED);
	Sort addSort(const std::string& name, const std::vector<Sort>& parameters, VariableType type = VariableType::VT_UNINTERPRETED);
	void makeSortIndexable(const Sort& sort, std::size_t indices, VariableType type);

	/**
	 * @param sort A sort.
	 * @return true, if the given sort is interpreted.
	 */
	bool isInterpreted(const Sort& sort) const {
		return getType(sort) != VariableType::VT_UNINTERPRETED;
	}

	Sort index(const Sort& sort, const std::vector<std::size_t>& indices);

	/**
	 * Gets the sort with arity zero (thus it is maybe interpreted) corresponding the given name.
	 * @param name The name of the sort to get.
	 * @return The resulting sort.
	 */
	Sort getSort(const std::string& name);

	/**
	 * Gets the sort with arity greater than zero corresponding the given name and having the arguments
	 * of the given sorts.
	 * @param name The name of the sort to get.
	 * @param params The sort of the arguments of the sort to get.
	 * @return The resulting sort.
	 */
	Sort getSort(const std::string& name, const std::vector<Sort>& params);

	Sort getSort(const std::string& name, const std::vector<std::size_t>& indices);

	Sort getSort(const std::string& name, const std::vector<std::size_t>& indices, const std::vector<Sort>& params);
};

/**
 * Gets the sort specified by the arguments.
 * Forwards to SortManager::getSort().
 */
template<typename... Args>
inline Sort getSort(Args&&... args) {
	return SortManager::getInstance().getSort(std::forward<Args>(args)...);
}

} // namespace carl

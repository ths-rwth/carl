/**
 * @file UFInstanceManager.h
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-30
 * @version 2014-10-30
 */

#pragma once

#include <carl/util/Common.h>
#include <carl-common/memory/Singleton.h>
#include <carl-common/datastructures/hash.h>
#include "../sort/Sort.h"
#include "UFInstance.h"
#include "UTerm.h"
#include "UVariable.h"

#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace carl {

// Forward declaration.
class UFInstanceManager;

/**
 * The actual content of an uninterpreted function instance.
 */
class UFInstanceContent {
	friend class UFInstanceManager;

private:
	/// The underlying uninterpreted function of theinstance.
	UninterpretedFunction mUninterpretedFunction;
	/// The uninterpreted function instance's arguments.
	std::vector<UTerm> mArgs;

public:
	UFInstanceContent() = delete;
	UFInstanceContent(const UFInstanceContent&) = delete;
	UFInstanceContent(UFInstanceContent&&) = delete;

	/**
	 * Constructs the content of an uninterpreted function instance.
	 * @param uf The underlying function of the uninterpreted function instance to construct.
	 * @param args The arguments of the uninterpreted function instance to construct.
	 */
	explicit UFInstanceContent(const UninterpretedFunction& uf, std::vector<UTerm>&& args)
		: mUninterpretedFunction(uf),
		  mArgs(std::move(args)) {}

	/**
	 * Constructs the content of an uninterpreted function instance.
	 * @param uf The underlying function of the uninterpreted function instance to construct.
	 * @param args The arguments of the uninterpreted function instance to construct.
	 */
	explicit UFInstanceContent(const UninterpretedFunction& uf, const std::vector<UTerm>& args)
		: mUninterpretedFunction(uf),
		  mArgs(args) {}

	/**
	 * @return The underlying function of the uninterpreted function instance
	 */
	const UninterpretedFunction& uninterpretedFunction() const {
		return mUninterpretedFunction;
	}

	/**
	 * @return The arguments of the uninterpreted function instance.
	 */
	const std::vector<UTerm>& args() const {
		return mArgs;
	}

	/**
	 * @param ufic The uninterpreted function instance's content to compare with.
	 * @return true, if this uninterpreted function instance's content is less than the given one.
	 */
	bool operator==(const UFInstanceContent& ufic) const {
		return (mUninterpretedFunction == ufic.uninterpretedFunction() && mArgs == ufic.args());
	}

	/**
	 * @param ufic The uninterpreted function instance's content to compare with.
	 * @return true, if this uninterpreted function instance's content is less than the given one.
	 */
	bool operator<(const UFInstanceContent& ufic) const {
		if (mUninterpretedFunction < ufic.uninterpretedFunction()) {
			return true;
		}
		if (ufic.uninterpretedFunction() < mUninterpretedFunction) {
			return false;
		}
		if (mArgs.size() < ufic.args().size()) {
			return true;
		}
		if (mArgs.size() > ufic.args().size()) {
			return false;
		}
		auto argA = mArgs.begin();
		auto argB = ufic.args().begin();
		while (argA != mArgs.end()) {
			assert(argB != ufic.args().end());
			if (*argA < *argB) {
				return true;
			}
			if (*argB < *argA) {
				return false;
			}
			++argA;
			++argB;
		}
		return false;
	}
};
} // end namespace carl

namespace std {
/**
 * Implements std::hash for uninterpreted function instance's contents.
 */
template<>
struct hash<carl::UFInstanceContent> {
public:
	/**
	 * @param ufun The uninterpreted function to get the hash for.
	 * @return The hash of the given uninterpreted function.
	 */
	std::size_t operator()(const carl::UFInstanceContent& ufun) const {
		return carl::hash_all(ufun.uninterpretedFunction(), ufun.args());
	}
};
} // end namespace std

namespace carl {

/**
 * Implements a manager for uninterpreted function instances, containing their actual contents and allocating their ids.
 */
class UFInstanceManager : public Singleton<UFInstanceManager> {
	friend Singleton<UFInstanceManager>;

private:
	/// Stores all instantiated uninterpreted function instance's contents and maps them to their unique id.
	FastPointerMap<UFInstanceContent, std::size_t> mUFInstanceIdMap;
	/// Maps the unique ids to the instantiated uninterpreted function instance's content.
	std::vector<std::unique_ptr<UFInstanceContent>> mUFInstances;

	/**
	 * Constructs an uninterpreted function instances manager.
	 */
	UFInstanceManager()
		: mUFInstanceIdMap(),
		  mUFInstances() {
		mUFInstances.emplace_back(nullptr); // default value
	}
	~UFInstanceManager() override {
		mUFInstanceIdMap.clear();
		mUFInstances.clear();
	}

	/**
	 * Tries to add the given uninterpreted function instance's content to the so far stored uninterpreted function instance's
	 * contents. If it has already been stored, if will be deleted and the id of the already existing uninterpreted
	 * function instance's content will be used to create the returned uninterpreted function instance.
	 * @param ufic The uninterpreted function instance's content to store.
	 * @return The uninterpreted function instance corresponding to the given content.
	 */
	UFInstance newUFInstance(std::unique_ptr<UFInstanceContent>&& ufic);

public:
	/**
	 * @param ufi An uninterpreted function instance.
	 * @return The underlying uninterpreted function of the uninterpreted function of the given uninterpreted function instance.
	 */
	const UninterpretedFunction& getUninterpretedFunction(const UFInstance& ufi) const {
		assert(ufi.id() != 0);
		assert(ufi.id() < mUFInstances.size());
		return mUFInstances[ufi.id()]->uninterpretedFunction();
	}

	/**
	 * @param ufi An uninterpreted function instance.
	 * @return The arguments of the given uninterpreted function instance.
	 */
	const std::vector<UTerm>& getArgs(const UFInstance& ufi) const {
		assert(ufi.id() != 0);
		assert(ufi.id() < mUFInstances.size());
		return mUFInstances[ufi.id()]->args();
	}

	/**
	 * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
	 * @param uf The underlying function of the uninterpreted function instance to get.
	 * @param args The arguments of the uninterpreted function instance to get.
	 * @return The resulting uninterpreted function instance.
	 */
	UFInstance newUFInstance(const UninterpretedFunction& uf, std::vector<UTerm>&& args) {
		auto result = std::make_unique<UFInstanceContent>(uf, std::move(args));
		assert(argsCorrect(*result));
		return newUFInstance(std::move(result));
	}

	/**
	 * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
	 * @param uf The underlying function of the uninterpreted function instance to get.
	 * @param args The arguments of the uninterpreted function instance to get.
	 * @return The resulting uninterpreted function instance.
	 */
	UFInstance newUFInstance(const UninterpretedFunction& uf, const std::vector<UTerm>& args) {
		auto result = std::make_unique<UFInstanceContent>(uf, args);
		assert(argsCorrect(*result));
		return newUFInstance(std::move(result));
	}

	/**
         * @return true, if the arguments domains coincide with those of the domain.
         */
	static bool argsCorrect(const UFInstanceContent& ufic);
};

/**
 * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
 * @param uf The underlying function of the uninterpreted function instance to get.
 * @param args The arguments of the uninterpreted function instance to get.
 * @return The resulting uninterpreted function instance.
 */
inline UFInstance newUFInstance(const UninterpretedFunction& uf, std::vector<UTerm>&& args) {
	return UFInstanceManager::getInstance().newUFInstance(uf, std::move(args));
}

/**
 * Gets the uninterpreted function instance with the given name, domain, arguments and codomain.
 * @param uf The underlying function of the uninterpreted function instance to get.
 * @param args The arguments of the uninterpreted function instance to get.
 * @return The resulting uninterpreted function instance.
 */
inline UFInstance newUFInstance(const UninterpretedFunction& uf, const std::vector<UTerm>& args) {
	return UFInstanceManager::getInstance().newUFInstance(uf, args);
}

} // end namespace carl

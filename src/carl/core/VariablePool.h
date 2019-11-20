/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#pragma once

#include "../config.h"
#include "../util/Singleton.h"
#include "Variable.h"

#include <array>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace carl {

/**
 * This class generates new variables and stores human-readable names for them.
 *
 * As we want only a single unique VariablePool and need global access to it, it is implemented as a singleton.
 *
 * All methods that modify the pool, that are getInstance(), getFreshVariable() and setName(), are thread-safe.
 */
class VariablePool : public Singleton<VariablePool> {
	friend Singleton<VariablePool>;

private:
	/**
	 * Contains the id of the next variable to be created for each type.
	 * As such, is also a counter of the variables that exist.
	 */
	std::array<std::size_t, static_cast<std::size_t>(VariableType::TYPE_SIZE)> mNextIDs;

	/**
	 * Mutex for calling getFreshVariable().
	 */
	mutable std::mutex freshVarMutex;

	/**
	 * Mutex for calling setVariableName().
	 */
	mutable std::mutex setNameMutex;

	std::size_t& nextID(VariableType vt) noexcept {
		assert(static_cast<std::size_t>(vt) < mNextIDs.size());
		return mNextIDs[static_cast<std::size_t>(vt)];
	}
	std::size_t nextID(VariableType vt) const noexcept {
		assert(static_cast<std::size_t>(vt) < mNextIDs.size());
		return mNextIDs[static_cast<std::size_t>(vt)];
	}

	/**
	 * Contains persistent variables that are restored after clear was called.
	 */
	std::vector<std::pair<Variable, std::string>> mPersistentVariables;

	/**
	 * Stores human-readable names for variables that can be set via setVariableName().
	 */
	std::map<Variable, std::string> mVariableNames;

	/**
	 * Stores a prefix for printing variables that have no human-readable name.
	 */
	std::string mVariablePrefix;

#ifdef THREAD_SAFE
#define FRESHVAR_LOCK_GUARD std::lock_guard<std::mutex> lock1(freshVarMutex);
#define SETNAME_LOCK_GUARD std::lock_guard<std::mutex> lock2(setNameMutex);
#else
#define FRESHVAR_LOCK_GUARD
#define SETNAME_LOCK_GUARD
#endif

protected:
	/**
	 * Private default constructor.
	 */
	VariablePool() noexcept;

	/**
	 * Get a variable which was not used before.
	 * This method is thread-safe.
	 * @param type Type for the new variable.
	 * @return A new variable.
	 */
	Variable getFreshVariable(VariableType type = VariableType::VT_REAL) noexcept;

	/**
	 * Get a variable with was not used before and set a name for it.
	 * This method is thread-safe.
	 * @param name Name for the new variable.
	 * @param type Type for the new variable.
	 * @return A new variable.
	 */
	Variable getFreshVariable(const std::string& name, VariableType type = VariableType::VT_REAL);

public:
	Variable getFreshPersistentVariable(VariableType type = VariableType::VT_REAL) noexcept;
	Variable getFreshPersistentVariable(const std::string& name, VariableType type = VariableType::VT_REAL);

	/**
	 * Clears everything already created in this pool.
	 */
	void clear() noexcept {
		mVariableNames.clear();
		mNextIDs.fill(1);
		for (const auto& pv : mPersistentVariables) {
			Variable v = pv.first;
			while (nextID(v.type()) < v.id()) {
				getFreshVariable(v.type());
			}
			if (!pv.second.empty()) {
				getFreshVariable(pv.second, v.type());
			} else {
				getFreshVariable(v.type());
			}
		}
	}

	/**
	 * Searches in the friendly names list for a variable with the given name.
	 * @param name The friendly variable name to look for.
	 * @return The first variable with that friendly name.
	 */
	Variable findVariableWithName(const std::string& name) const noexcept;
	/**
	 * Get a human-readable name for the given variable.
	 * If the given Variable is Variable::NO_VARIABLE, "NO_VARIABLE" is returned.
	 * If friendlyVarName is true, the name that was set via setVariableName() for this Variable, if there is any, is returned.
	 * Otherwise "x_<id>" is returned, id being the internal id of the Variable.
	 * @param v Variable.
	 * @param variableName Flag, if a name set via setVariableName shall be considered.
	 * @return Some name for the Variable.
	 */
	std::string getName(Variable v, bool variableName = true) const;
	/**
	 * Add a name for a given Variable.
	 * This method is thread-safe.
	 * @param v Variable.
	 * @param name Some string naming the variable.
	 */
	void setName(Variable v, const std::string& name);

	/**
	 * Sets the prefix used when printing anonymous variables.
	 * The default is "_", hence they look like "_x_5".
     * @param prefix Prefix for anonymous variable names.
     */
	void setPrefix(std::string prefix = "_") noexcept {
		mVariablePrefix = std::move(prefix);
	}

	/**
	 * Returns the number of variables initialized by the pool.
	 * @return Number of variables.
	 */
	std::size_t nrVariables(VariableType type = VariableType::VT_REAL) const noexcept {
		return nextID(type) - 1;
	}

	/**
	 * Print variable names to the stream.
	 */
	void printVariableNamesToStream(std::ostream& os) {
		for (auto const& v : mVariableNames) {
			os << v.second << " ";
		}
	}

	friend Variable freshVariable(VariableType vt) noexcept;
	friend Variable freshVariable(const std::string& name, VariableType vt);
};

inline Variable freshVariable(VariableType vt) noexcept {
	return VariablePool::getInstance().getFreshVariable(vt);
}
inline Variable freshVariable(const std::string& name, VariableType vt) {
	return VariablePool::getInstance().getFreshVariable(name, vt);
}

inline Variable freshBitvectorVariable() noexcept {
	return freshVariable(VariableType::VT_BITVECTOR);
}
inline Variable freshBitvectorVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_BITVECTOR);
}
inline Variable freshBooleanVariable() noexcept {
	return freshVariable(VariableType::VT_BOOL);
}
inline Variable freshBooleanVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_BOOL);
}
inline Variable freshRealVariable() noexcept {
	return freshVariable(VariableType::VT_REAL);
}
inline Variable freshRealVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_REAL);
}
inline Variable freshIntegerVariable() noexcept {
	return freshVariable(VariableType::VT_INT);
}
inline Variable freshIntegerVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_INT);
}
inline Variable freshUninterpretedVariable() noexcept {
	return freshVariable(VariableType::VT_UNINTERPRETED);
}
inline Variable freshUninterpretedVariable(const std::string& name) {
	return freshVariable(name, VariableType::VT_UNINTERPRETED);
}

inline void printRegisteredVariableNames(std::ostream& os) {
	return VariablePool::getInstance().printVariableNamesToStream(os);
}

} // namespace carl

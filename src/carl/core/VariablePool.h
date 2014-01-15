/**
 * @file VariablePool.h 
 * @author Sebastian Junges
 */

#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <map>
#include "initialize.h"
#include "Variable.h"


namespace carl 
{

/**
 * This class generates new variables and stores human-readable names for them.
 *
 * As we want only a single unique VariablePool and need global access to it, it is implemented as a singleton.
 *
 * All methods that modify the pool, that are getInstance(), getFreshVariable() and setName(), are thread-safe.
 */
class VariablePool
{
private:
	/**
	 * Contains the id of the next variable to be created.
	 * As such, is also a counter of the variables that exist.
	 */
	unsigned mNextVarId;

	/**
	 * The unique instance of VariablePool.
	 */
	static std::unique_ptr<VariablePool>  instance;

	/**
	 * Mutex for the creation of the singleton instance.
	 */
	static std::mutex singletonMutex;

	/**
	 * Mutex for calling getFreshVariable().
	 */
	std::mutex freshVarMutex;

	/**
	 * Mutex for calling setVariableName().
	 */
	std::mutex setNameMutex;

	/**
	 * Stores human-readable names for variables that can be set via setVariableName().
	 */
	std::map<Variable, std::string> mFriendlyNames;

	/**
	 * Private default constructor.
	 */
	VariablePool();

	/**
	 * This class is a singleton, hence there shall be no copy constructor.
	 */
	VariablePool(const VariablePool&) = delete;

	/**
	 * This class is a singleton, hence there shall be no assignment operator.
	 */
	VariablePool& operator=(const VariablePool&) = delete;
public:
	/**
	 * Returns the single instance of this class by reference.
	 * If there is no instance yet, a new one is created.
	 * This method is thread-safe.
	 */
	static VariablePool& getInstance();

	/**
	 * Get a variable which was not used before.
	 * This method is thread-safe.
	 * @param type Type for the new variable.
	 * @return A new variable.
	 */
	Variable getFreshVariable(VariableType type = VT_REAL);

	/**
	 * Get a variable with was not used before and set a name for it.
	 * This method is thread-safe.
	 * @param name Name for the new variable.
	 * @param type Type for the new variable.
	 * @return A new variable.
	 */
	Variable getFreshVariable(const std::string& name, VariableType type = VT_REAL);

	/**
	 * Get a human-readable name for the given variable.
	 * If the given Variable is Variable::NO_VARIABLE, "NO_VARIABLE" is returned.
	 * If friendlyVarName is true, the name that was set via setVariableName() for this Variable, if there is any, is returned.
	 * Otherwise "x_<id>" is returned, id being the internal id of the Variable.
	 * @param v Variable.
	 * @param friendlyVarName Flag, if a name set via setVariableName shall be considered.
	 * @return Some name for the Variable.
	 */

	const std::string getName(Variable::Arg v, bool friendlyVarName = true) const;
	/**
	 * Add a name for a given Variable.
	 * This method is thread-safe.
	 * @param v Variable.
	 * @param name Some string naming the variable.
	 */
	void setName(Variable::Arg v, const std::string& name);
   
	/**
	 * Returns the number of variables initialized by the pool.
	 * @return Number of variables.
	 */
	unsigned nrVariables() const {
		return mNextVarId;
	}
};

}
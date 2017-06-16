/**
 * @file Singleton.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <memory>
#include <mutex>

namespace carl
{

/**
 * Base class that implements a singleton.
 * 
 * A class that shall be a singleton can inherit from this class (the template argument being the class itself, see CRTP for this).
 * It takes care of
 * <ul>
 * <li>deleting the copy constructor and the assignment operator,</li>
 * <li>providing a protected default constructor and a virtual destructor and</li>
 * <li>providing getInstance() that returns the one single object of this type.</li>
 * </ul>
 */
template<typename T>
class Singleton
{
private:
	/**
	 * There shall be no copy constructor.
	 */
	Singleton(const Singleton&) = delete;

	/**
	 * There shall be no assignment operator.
	 */
	Singleton& operator=(const Singleton&) = delete;

protected:
	/**
	 * Protected default constructor.
	 */
	Singleton() = default;

public:
	/**
	 * Virtual destructor.
	 */
	virtual ~Singleton() = default;

	/**
	 * Returns the single instance of this class by reference.
	 * If there is no instance yet, a new one is created.
	 */
	static inline T& getInstance() {
		static T t;
		return t;
	}
};

/**
 * Base class that implements an instantiated singleton.
 * 
 * This singleton is instantiated to fix problems with singletons which can occur multiple times in shared libraries.
 */
template<typename T>
class SingletonInstantiated
{
private:
	/**
	 * There shall be no copy constructor.
	 */
	SingletonInstantiated(const SingletonInstantiated&) = delete;

	/**
	 * There shall be no assignment operator.
	 */
	SingletonInstantiated& operator=(const SingletonInstantiated&) = delete;

protected:
	/**
	 * Protected default constructor.
	 */
	SingletonInstantiated() = default;

public:
	/**
	 * Virtual destructor.
	 */
	virtual ~SingletonInstantiated() = default;

	/**
	 * Returns the single instance of this class by reference.
	 * If there is no instance yet, a new one is created.
	 */
	static T& getInstance();
};

}

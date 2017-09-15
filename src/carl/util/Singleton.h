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
protected:
	/**
	 * Protected default constructor.
	 */
	Singleton() = default;

public:
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;

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

}

/**
 * @file Singleton.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <memory>
#include <mutex>

namespace carl
{

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
	Singleton() {};

public:
	virtual ~Singleton() {};
	/**
	 * Returns the single instance of this class by reference.
	 * If there is no instance yet, a new one is created.
	 */
	static T& getInstance() {
		static T t;
		return t;
	}
};

}

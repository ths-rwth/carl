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
	 * The unique instance of T.
	 */
	static std::unique_ptr<T>  instance;

	/**
	 * Mutex for the creation of the singleton instance.
	 */
	static std::mutex singletonMutex;

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
	/**
	 * Returns the single instance of this class by reference.
	 * If there is no instance yet, a new one is created.
	 * This method is thread-safe.
	 */
	static T& getInstance()
	{
		if (!Singleton<T>::instance) {
			std::lock_guard<std::mutex> lock(Singleton<T>::singletonMutex);
			if (!Singleton<T>::instance) {
				Singleton<T>::instance.reset(new T());
			}
		}
		return *Singleton<T>::instance;
	}
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;
template<typename T>
std::mutex Singleton<T>::singletonMutex;

}

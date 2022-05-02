/**
 * @file debug.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * This file contains convenience routines for debugging, e.g. to print more complex data structures like STL container or print stack traces.
 */

#pragma once

#include <csignal>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <carl-common/meta/platform.h>

#ifndef __VS
#include <unistd.h>
#endif

namespace carl {

/**
 * Uses GDB to print a stack trace.
 */
void printStacktrace();

std::string demangle(const char* name);

std::string callingFunction();

template<typename T>
std::string typeString() {
	const char* name = typeid(T).name();
	return demangle(name);
}

#ifndef NDEBUG
/**
 * Stores a textual representation of the last assertion that was registered via REGISTER_ASSERT.
 */
extern std::string last_assertion_string;
/**
 * Stores an integer representation of the last assertion that was registered via REGISTER_ASSERT.
 */
extern int last_assertion_code;
/**
 * Registers an upcoming assertion with the SIGABRT signal handler.
 * 
 * If the program is compiled in debug mode, a signal handler is installed automatically that catches SIGABRT that is send when an assertion fails.
 * The signal handler uses the data in last_assertion_string and last_assertion_code to generate an additional message and a custom exit code whenever an assertion is thrown.
 * As for last_assertion_code the line number of the usage of this macro is used.
 * 
 * If REGISTER_ASSERT was not called before SIGABRT is catched, the exit code is 23.
 * 
 * This macro is intended to be used to identify a single assertion from the exit code in automated testing, for example using the delta debugger.
 * The usage will usually look like this:
 * @code{.cpp}
 * REGISTER_ASSERT; assert( ... );
 * @endcode
 */
#ifdef __VS
#define __func__ __FUNCTION__
#endif

#define REGISTER_ASSERT {\
	std::stringstream ss; \
	ss << __FILE__ << ":" << __LINE__ << " in " << __func__ << "()"; \
	carl::last_assertion_string = ss.str(); \
	carl::last_assertion_code = __LINE__; \
	}
#define UNREGISTER_ASSERT {\
	carl::last_assertion_string = ""; \
	carl::last_assertion_code = 23; \
	}
#define REGISTERED_ASSERT(condition) REGISTER_ASSERT; assert(condition); UNREGISTER_ASSERT

#else
#define REGISTER_ASSERT
#define UNREGISTER_ASSERT
#define REGISTERED_ASSERT(condition)
#endif

}

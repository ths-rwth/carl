/**
 * @file debug.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * This file contains convenience routines for debugging, e.g. to print more complex data structures like STL container or print stack traces.
 */

#pragma once

#include <csignal>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <unistd.h>

namespace carl {

#define CALLFUNC(logger,args) LOGMSG_TRACE(logger, this << " :: " << __func__ << "(" << args << ")");

/**
 * Output a list with arbitrary content.
 * The format is `[<length>: <item>, <item>, ...]`
 * @param os Output stream.
 * @param l list to be printed.
 * @return Output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& l) {
	os << "[" << l.size() << ": ";
	bool first = true;
	for (auto it: l) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

/**
 * Output a vector with arbitrary content.
 * The format is `[<length>: <item>, <item>, ...]`
 * @param os Output stream.
 * @param v Vector to be printed.
 * @return Output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	os << "[" << v.size() << ": ";
	bool first = true;
	for (auto it: v) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

/**
 * Output a map with arbitrary content.
 * The format is `{<key>:<value>, <key>:<value>, ...}`
 * @param os Output stream.
 * @param m map to be printed.
 * @return Output stream.
 */
template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const std::map<Key, Value>& m) {
	os << "{";
	bool first = true;
	for (auto it: m) {
		if (!first) os << ", ";
		first = false;
		os << it.first << ":" << it.second;
	}
	return os << "}";
}

#ifdef DEBUG

/**
 * Uses GDB to print a stack trace.
 */
void printStacktrace(bool interaction = false);

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
#define REGISTER_ASSERT #warning "REGISTER_ASSERT is disabled as DEBUG is not defined."
#define UNREGISTER_ASSERT #warning "UNREGISTER_ASSERT is disabled as DEBUG is not defined."
#define REGISTERED_ASSERT(condition) #warning "REGISTER_ASSERT(condition) is disabled as DEBUG is not defined."
#endif

}

/**
 * @file debug.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * This file contains convenience routines for debugging, e.g. to print more complex data structures like STL container or print stack traces.
 */

#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "Singleton.h"

namespace carl {
	
#define CALLFUNC(logger,args) LOGMSG_TRACE(logger, this << " :: " << __func__ << "(" << args << ")");

/**
 * Output a list with arbitrary content.
 * The format is [<length>: <item>, <item>, ...]
 * @param Output stream.
 * @param list to be printed.
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
 * The format is [<length>: <item>, <item>, ...]
 * @param Output stream.
 * @param Vector to be printed.
 * @return Output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& l) {
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
 * Output a map with arbitrary content.
 * The format is {<key>:<value>, <key>:<value>, ...}
 * @param Output stream.
 * @param map to be printed.
 * @return Output stream.
 */
template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, const std::map<Key, Value>& l) {
	os << "{";
	bool first = true;
	for (auto it: l) {
		if (!first) os << ", ";
		first = false;
		os << it.first << ":" << it.second;
	}
	return os << "}";
}


/**
 * Convenience class to print stack traces to an output stream.
 *
 * Retrieves the current stack trace (up to 128 stack frames) and prints them to a given output stream.
 * This class can store multiple stack traces and merge them, i.e. determine a common prefix of stack frames and print this.
 * For supported compilers (that is gcc and clang), it will try to demangle the symbol names using the abi.
 *
 * This class is a singleton.
 * However, if you want to print a single stacktrace without having it stored in the global Stacktrace object, you can use the printStacktrace routine.
 */
class Stacktrace : public Singleton<Stacktrace> {
friend Singleton<Stacktrace>;
private:
	// Stores stack traces.
	std::map<std::string, std::vector<std::string>> traces;
	// Stores ANSI color codes.
	std::map<std::string, std::string> col = {
		{"red", "\033[31m"},
		{"green", "\033[32m"},
		{"gray", "\033[37m"},
		{"reset", "\033[39m"}
	};
	
	/**
	 * Use abi to demangle symbol of a stackframe.
	 * @param stackframe Pointer to the current stackframe.
	 * @param sym Symbols of the current stackframe.
	 * @return Somewhat human readable version of the stackframe.
	 */
	std::string demangle(const void* stackframe, const char* sym) const;
public:
	/**
	 * Clear the stored stack traces.
	 */
	void clear() {
		this->traces.clear();
	}
	/**
	 * Store the current stack trace using the given name.
	 * @param name Name for the stack trace.
	 */
	void storeTrace(const std::string& name);
	
	/**
	 * Output all stored stack traces.
	 * @param os Output stream.
	 * @return Output stream.
	 */
	std::ostream& all(std::ostream& os = std::cerr) const;
	/**
	 * Output all stored stack traces with common stack traces merged.
	 * @param os Output stream.
	 * @return Output stream.
	 */
	std::ostream& merged(std::ostream& os = std::cerr) const;
	
	friend std::ostream& operator<<(std::ostream& os, const Stacktrace& s);
	friend std::ostream& printStacktrace(std::ostream& os);
};

/**
 * Convenience method to use the Stacktrace class.
 * If this method is used, the stack trace is not stored and hence not considered in the output of the global Stacktrace object.
 */
std::ostream& printStacktrace(std::ostream& os = std::cerr);

}

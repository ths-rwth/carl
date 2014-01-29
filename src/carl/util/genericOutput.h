/**
 * @file genericOutput.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * This file contains convenience routines to print more complex data structures like STL container.
 */

#include <iostream>
#include <list>
#include <map>
#include <vector>

namespace carl {

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

}

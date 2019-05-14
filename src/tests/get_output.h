#pragma once

#include <sstream>
#include <string>

/// Helper to check the result of operator<<().
template<typename T>
std::string get_output(const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

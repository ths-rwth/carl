#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include <string>

namespace carl {
	
	template<typename T>
	inline T rationalize(double n);

	template<typename T>
	inline T rationalize(float n);

	template<typename T>
	inline T rationalize(int n);

	template<typename T>
	inline T rationalize(std::size_t n);

	template<typename T>
	inline T rationalize(const std::string& n);
	
	
}

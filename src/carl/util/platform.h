/** 
 * @file:   platform.h
 * @author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file contains platform and compiler specific macros.
 */

#pragma once

//////////////////////
// Compiler identification
// taken from http://sourceforge.net/p/predef/wiki/Compilers/

#define STRINGIFY(s) #s

#ifdef __clang__
	#define __CLANG

	#define CLANG_WARNING_DISABLE(warning)\
		_Pragma("clang diagnostic push")\
		_Pragma( STRINGIFY(clang diagnostic ignored warning) )
	#define CLANG_WARNING_RESET\
		_Pragma("clang diagnostic pop")
#elif __GNUC__
	#define __GCC
	
	#define CLANG_WARNING_DISABLE(warning)
	#define CLANG_WARNING_RESET
#else
	#warning "You are using an unsupported compiler."
	#define __UNSUPPORTED
#endif

#include <iostream>

/**
 * Output the current stack trace to the given output stream.
 *
 * For supported compilers (that is gcc and clang), this will output a full stack trace (up to 128 stack frames).
 * It will try to demangle the symbol names using the abi.
 *
 * If colors is set to true, it will use terminal color codes to colorize the output.
 * @param os Output stream.
 * @param colors Flag is coloring shall be enabled.
 * @return os.
 */
std::ostream& printStacktrace(std::ostream& os, bool colors = true);

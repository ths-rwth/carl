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

#if defined __clang__
	#define __CLANG

	#define CLANG_WARNING_DISABLE(warning)\
		_Pragma("clang diagnostic push")\
		_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")\
		_Pragma("clang diagnostic ignored \"-Wunknown-warning-option\"")\
		_Pragma( STRINGIFY(clang diagnostic ignored warning) )
	#define CLANG_WARNING_RESET\
		_Pragma("clang diagnostic pop")
#elif defined __GNUC__
	#define __GCC

	#define CLANG_WARNING_DISABLE(warning)
	#define CLANG_WARNING_RESET
#elif defined _MSC_VER
	#define __VS

	#define CLANG_WARNING_DISABLE(warning)
	#define CLANG_WARNING_RESET
#else
	#warning "You are using an unsupported compiler."
	#define __UNSUPPORTED
#endif

#if defined _WIN32
	#define __WIN
#elif defined _WIN64
	#define __WIN
#elif defined __APPLE__
	#define __MACOS
#elif defined __linux__
	#define __LINUX
#else
	#warning "You are using an unsupported operating system."
#endif

#define __BITS (sizeof(void*))

#define UNUSED(x) (void)(x)
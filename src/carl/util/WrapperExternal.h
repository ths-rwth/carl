/**
* @file WrapperExternal.h
* @author Matthias Volk
* Defines the exported functions for the library
*/
#pragma once


#ifdef __WIN
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT 
#endif

namespace carl {
	class WrapperExternal
	{
	public:
		DLL_EXPORT void sendTest(const char* s);

		DLL_EXPORT const char* getResult();
	};
}
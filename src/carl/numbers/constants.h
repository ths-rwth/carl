/** 
 * @file   constants.h
 * @author Sebastian Junges
 *
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#define constant_zero_declaration_type(type, zero_decl) \
template<> \
	struct constant_zero<type> { \
		static const type& get()  \
		{ \
			static type t = zero_decl; \
			return t; \
		} \
	}; 

#define constant_one_declaration_type(type, one_decl) \
template<> \
	struct constant_one<type> { \
		static const type& get()  \
		{ \
			static type t = one_decl; \
			return t; \
		} \
	}; 

//
#define constant_declarations(type, zero_decl, one_decl) \
	constant_zero_declaration(type, zero_decl)  \
	constant_one_declaration(type, one_decl)

namespace carl
{
	template<typename T>
	struct constant_zero {
		static const T& get() 
		{
			static T t(0); // NOLINT
			return t;
		}
	};
	
	template<typename T>
	struct constant_one {
		static const T& get() 
		{
			static T t(1); // NOLINT
			return t;
		}
	};
}

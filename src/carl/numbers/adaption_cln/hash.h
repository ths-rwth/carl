/** 
 * @file   adaption_cln/hash.h
 * @ingroup cln
 * @author Sebastian Junges
 * @author  Florian Corzilius
 *
 */

#pragma once
#include <functional>
#include "../../util/platform.h"
CLANG_WARNING_DISABLE("-Wmismatched-tags")
#include <cln/cln.h>
CLANG_WARNING_RESET
namespace std
{
	
template<>
struct hash<cln::cl_RA>
{
	size_t operator()(const cln::cl_RA& cln_rational) const
	{
		return cln::equal_hashcode(cln_rational);
	}
};

}
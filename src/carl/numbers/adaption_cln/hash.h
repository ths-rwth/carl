/** 
 * @file   hash.h
 * @ingroup cln
 * @author Sebastian Junges
 * @author  Florian Corzilius
 *
 */

#pragma once
#include <functional>
#include <cln/cln.h>
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
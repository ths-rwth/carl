/** 
 * @file:   cyclic.h
 * @author: Sebastian Junges
 *
 * @since November 4, 2014
 */

#pragma once

#include <cassert>
#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-io/StringParser.h>

namespace carl
{
	namespace benchmarks 
	{
	
template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> cyclic2()
{
	carl::io::StringParser sp;
	sp.setVariables({"x", "y"});
	std::vector<MultivariatePolynomial<C, O, P>> res;
	// x + y
    res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x + y"));
	// x*y - 1
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x*y + -1"));
	return res;
}

template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> cyclic3()
{
	carl::io::StringParser sp;
	sp.setVariables({"x", "y", "z"});
	std::vector<MultivariatePolynomial<C, O, P>> res;
	// x + y + z
    res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x + y + z"));
	// x*y + x*z + y*z
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x*y + x*z + y*z"));
	// x*y*z - 1
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x*y*z + -1"));
	return res;
}



#define run_cyclic_case(INDEX)	case INDEX: return cyclic##INDEX<C, O, P>()
	
template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> cyclic(unsigned index)
{
	switch(index)
	{
		run_cyclic_case(2);
		run_cyclic_case(3);
		//run_katsura_case(4);
		//run_katsura_case(5);
		//run_katsura_case(6);
		//run_katsura_case(4);
		default:
			assert(index > 1);
			assert(index < 4);
	}
	return std::vector<MultivariatePolynomial<C, O, P>>();
}
	

	}
}
/** 
 * @file   katsura.h
 * @author Sebastian Junges
 *
 */

#pragma once

#include "../../core/MultivariatePolynomial.h"
#include "../../util/stringparser.h"

namespace carl 
{
namespace benchmarks
{



template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> katsura2()
{
	carl::StringParser sp;
	sp.setVariables({"x", "y"});
	std::vector<MultivariatePolynomial<C, O, P>> res;
	//x + 2*y - 1, 
    res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x + 2*y + -1"));
	//x^2 + 2*y^2 - x
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x^2 + 2*y^2 + -1*x"));
	return res;
}

template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> katsura3()
{
	
}



#define run_katsura_case(INDEX)	case INDEX: return katsura##INDEX<C, O, P>()
	
template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> katsura(unsigned index)
{
	switch(index)
	{
		run_katsura_case(2);
		run_katsura_case(3);
		//run_katsura_case(4);
	}
	
}
	
	
}
}
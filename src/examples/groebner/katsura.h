/** 
 * @file   katsura.h
 * @author Sebastian Junges
 *
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
std::vector<MultivariatePolynomial<C, O, P>> katsura2()
{
	carl::io::StringParser sp;
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
	carl::io::StringParser sp;
	sp.setVariables({"x", "y", "z"});
	std::vector<MultivariatePolynomial<C, O, P>> res;
	//x + 2*y + 2*z - 1,
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x + 2*y + 2*z + -1"));	
	//x^2 + 2*y^2 + 2*z^2 - x, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x^2 + 2*y^2 + 2*z^2 + -1*x"));
	//2*x*y + 2*y*z - y
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("2*x*y + 2*y*z + -1*y"));
	return res;
}


template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> katsura4()
{
	carl::io::StringParser sp;
	sp.setVariables({"x", "y", "z", "t"});
	std::vector<MultivariatePolynomial<C, O, P>> res;
	//x + 2*y + 2*z + 2*t - 1, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x + 2*y + 2*z + 2*t + -1"));
	//x^2 + 2*y^2 + 2*z^2 + 2*t^2 - x, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x^2 + 2*y^2 + 2*z^2 + 2*t^2 + -1*x"));
	//2*x*y + 2*y*z + 2*z*t - y, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("2*x*y + 2*y*z + 2*z*t + -1*y"));
	//y^2 + 2*x*z + 2*y*t - z
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("y^2 + 2*x*z + 2*y*t + -1*z"));
	return res;
}



template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> katsura5()
{
	carl::io::StringParser sp;
	sp.setVariables({"x", "y", "z", "t", "u"});
	std::vector<MultivariatePolynomial<C, O, P>> res;
	//x + 2*y + 2*z + 2*t + 2*u - 1, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x + 2*y + 2*z + 2*t + 2*u + -1"));
	//x^2 + 2*y^2 + 2*z^2 + 2*t^2 + 2*u^2 - x, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("x^2 + 2*y^2 + 2*z^2 + 2*t^2 + 2*u^2 + -1*x"));
	//2*x*y + 2*y*z + 2*z*t + 2*t*u - y, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("2*x*y + 2*y*z + 2*z*t + 2*t*u + -1*y"));
	//y^2 + 2*x*z + 2*y*t + 2*z*u - z, 
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("y^2 + 2*x*z + 2*y*t + 2*z*u + -1*z"));
	//2*y*z + 2*x*t + 2*y*u - t
	res.push_back(sp.parseMultivariatePolynomial<C, O, P>("2*y*z + 2*x*t + 2*y*u + -1*t"));
	return res;
}




#define run_katsura_case(INDEX)	case INDEX: return katsura##INDEX<C, O, P>()
	
template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C, O, P>> katsura(unsigned index)
{
	switch(index)
	{
		run_katsura_case(2);
		run_katsura_case(3);
		run_katsura_case(4);
		run_katsura_case(5);
		//run_katsura_case(6);
		//run_katsura_case(4);
		default:
			assert(index > 1);
			assert(index < 6);
	}
	return std::vector<MultivariatePolynomial<C, O, P>>();
}
	
	
}
}
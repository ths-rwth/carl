/** 
 * @file   PrimitiveEuclideanAlgorithm.h
 * @author Sebastian Junges
 *
 */

#pragma once

namespace carl
{
	
	template<typename C>
	class UnivariatePolynomial;
/**
 * Algorithm to compute the GCD in Unique factorization domains.
 * @see @cite GCL92, Algorithm 2.3 
 */
struct PrimitiveEuclidean
{
	template<typename Coeff>
	UnivariatePolynomial<Coeff> operator()(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b) const;
};
}

#include "PrimitiveEuclideanAlgorithm.tpp"

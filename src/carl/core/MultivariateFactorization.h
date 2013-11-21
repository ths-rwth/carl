/** 
 * @file:   MultivariateFactorization.h
 * @author: Sebastian Junges
 *
 * @since September 3, 2013
 */

#pragma once

template<Polynomial>
class MultivariateFactorization
{
	public: 
		static Polynomial squareFreePart(const Polynomial& p);
	private:
		static Polynomial squareFree(const Polynomial& p, Variable::Arg v)
		{
			
		}		
		
};
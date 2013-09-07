/** 
 * @file:   PrimeFactory.h
 * @author: Sebastian Junges
 *
 * @since September 6, 2013
 */

#pragma once
#include <cassert>



namespace carl
{

template<typename NumberType>
class PrimeFactory
{
	static const unsigned nrPrimes = 12;
	static const NumberType PRIMES[nrPrimes];
	unsigned nextPrimeNr = 0;
	public:
	NumberType nextPrime();
};

template<typename NumberType>
const NumberType PrimeFactory<NumberType>::PRIMES[nrPrimes] = { (NumberType)2, (NumberType)3, (NumberType)5, 
                                      (NumberType)7, (NumberType)11, (NumberType)13,
                                      (NumberType)17, (NumberType)19, (NumberType)23,
                                      (NumberType)29, (NumberType)31, (NumberType)37 };

template<typename NumberType>
NumberType PrimeFactory<NumberType>::nextPrime()
{
    assert(nextPrimeNr < nrPrimes);
    return PRIMES[nextPrimeNr++];
}

}
/** 
 * @file   DivisionResult.h
 * @author Sebastian Junges
 *
 * @since July 19, 2013
 */

#pragma once

namespace carl
{
/**
 * The result of
 * 
 * Notice that the DivisionLookupResult does not take ownership of the elements, 
 * i.e. during destruction, nothing happens. Furthermore, if the original divisor element is erased, 
 * the divisor becomes invalid. Instances of DivisionLookupResults are therefore merely suitable for passing information 
 * to be directly processed.
 */
template<typename Polynomial>
struct DivisionLookupResult
{
	DivisionLookupResult() :
    mDivisor(nullptr), mFactor(nullptr)
    {
        
    }
	
    DivisionLookupResult(const Polynomial* divisor, const Term<typename Polynomial::CoeffType>* factor) :
    mDivisor(divisor), mFactor(factor)
    {
        
    }
    
    bool success()
    {
        return mDivisor != nullptr;
    }
    
    
    const Polynomial* const mDivisor;
    const Term<typename Polynomial::CoeffType>* mFactor;
};

}
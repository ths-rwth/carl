/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "Monomial.h"
#include "MonomialPool.h"

namespace carl
{
    Monomial* Monomial::dropVariable(Variable::Arg v) const
    {
        LOG_FUNC("carl.core.monomial", mExponents << ", " << v);
        auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);

        if (it == mExponents.cend())
        {
            #ifdef USE_MONOMIAL_POOL
            return this;
            #else
            return new Monomial(*this);
            #endif
        }
        if (mExponents.size() == 1) return nullptr;

        exponent tDeg = mTotalDegree - it->second;
        std::vector<std::pair<Variable, exponent>> newExps(mExponents.begin(), it);
        it++;
        newExps.insert(newExps.end(), it, mExponents.end());
        #ifdef USE_MONOMIAL_POOL
        return MonomialPool::getInstance().create( std::move(newExps), tDeg );
        #else
        return new Monomial(std::move(newExps), tDeg);
        #endif
    }
    
    Monomial* Monomial::divide(Variable::Arg v) const
    {
        auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);
        if(it == mExponents.cend()) return nullptr;
        else {
            std::vector<std::pair<Variable, exponent>> newExps;
            // If the exponent is one, the variable does not occur in the new monomial.
            if(it->second == 1) {
                if(it != mExponents.begin()) {
                    newExps.assign(mExponents.begin(), it);
                }
                newExps.insert(newExps.end(), it+1,mExponents.end());
            } else {
                // We have to decrease the exponent of the variable by one.
                newExps.assign(mExponents.begin(), mExponents.end());
                newExps[(unsigned)(it - mExponents.begin())].second -= 1;
            }
            #ifdef USE_MONOMIAL_POOL
            return MonomialPool::getInstance().create( std::move(newExps), (exponent)(mTotalDegree - 1) );
            #else
            return new Monomial( std::move(newExps), (exponent)(mTotalDegree - 1) );
            #endif
        }
    }
    
//    std::pair<Monomial*,bool> divide(const Monomial& m) const
//    {
//        LOG_FUNC("carl.core.monomial", *this << ", " << m);
//        if(m.mTotalDegree > mTotalDegree || m.mExponents.size() > mExponents.size())
//        {
//            // Division will fail.
//            LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
//            return std::make_pair(nullptr,false);
//        }
//        std::vector<std::pair<Variable, exponent>> newExps;
//        Monomial* result = new Monomial();
//        result->mTotalDegree =  mTotalDegree - m.mTotalDegree;
//
//        // Linear, as we expect small monomials.
//        auto itright = m.mExponents.begin();
//        for(auto itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
//        {
//            // Done with division
//            if(itright == m.mExponents.end())
//            {
//                // Insert remaining part
//                newExps.insert(newExps.end(), itleft, mExponents.end());
//                #ifdef USE_MONOMIAL_POOL
//                shared_pointer<const Monomial> result = MonomialPool::getInstance().create( std::move(newExps), (exponent)(mTotalDegree - m.mTotalDegree) );
//                #else
//                Monomial* result = new Monomial( std::move(newExps), (exponent)(mTotalDegree - m.mTotalDegree) );
//                #endif
//                LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
//                return std::make_pair(result,true);;
//            }
//            // Variable is present in both monomials.
//            if(itleft->first == itright->first)
//            {
//                if (itleft->second < itright->second)
//                {
//                    // Underflow, itright->exp was larger than itleft->exp.
//                    LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
//                    return std::make_pair(nullptr,false);
//                }
//                exponent newExp = itleft->second - itright->second;
//                if(newExp > 0)
//                {
//                    newExps.push_back(std::make_pair(itleft->first, newExp));
//                }
//                itright++;
//            }
//            // Variable is not present in lhs, division fails.
//            else if(itleft->first > itright->first) 
//            {
//                delete result;
//                LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
//                return std::make_pair(nullptr,false);
//            }
//            else
//            {
//                assert(itright->first > itleft->first);
//                result->mExponents.push_back(*itleft);
//            }
//        }
//        // If there remain variables in the m, it fails.
//        if(itright != m.mExponents.end())
//        {
//            delete result;
//            LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
//            return std::make_pair(nullptr,false);
//        }
//        if (result->mExponents.empty())
//        {
//            delete result;
//            LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
//            return std::make_pair(nullptr,true);
//        }
//        result->calcHash();
//        assert(result->isConsistent());
//        LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
//        return std::make_pair(result,true);
//
//    }
}
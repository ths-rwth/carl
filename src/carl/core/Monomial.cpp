/**
 * @file MonomialPool.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 */

#include "Monomial.h"
#include "MonomialPool.h"

namespace carl
{
    std::shared_ptr<const Monomial> Monomial::dropVariable(Variable::Arg v) const
    {
        LOG_FUNC("carl.core.monomial", mExponents << ", " << v);
        auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);

        if (it == mExponents.cend())
        {
            return std::shared_ptr<const Monomial>( this );
        }
        if (mExponents.size() == 1) return nullptr;

        exponent tDeg = mTotalDegree - it->second;
        std::vector<std::pair<Variable, exponent>> newExps(mExponents.begin(), it);
        it++;
        newExps.insert(newExps.end(), it, mExponents.end());
        #ifdef USE_MONOMIAL_POOL
        return MonomialPool::getInstance().create( std::move(newExps), tDeg );
        #else
        return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), tDeg ) );
        #endif
    }
    
    std::shared_ptr<const Monomial> Monomial::divide(Variable::Arg v) const
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
            return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), (exponent)(mTotalDegree - 1) ) );
            #endif
        }
    }
    
    std::pair<std::shared_ptr<const Monomial>,bool> Monomial::divide(const std::shared_ptr<const Monomial>& m) const
    {
        LOG_FUNC("carl.core.monomial", *this << ", " << m);
        if( !m )
            return std::make_pair(std::shared_ptr<const Monomial>(this), true);
        if(m->mTotalDegree > mTotalDegree || m->mExponents.size() > mExponents.size())
        {
            // Division will fail.
            LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
            return std::make_pair(nullptr,false);
        }
        std::vector<std::pair<Variable, exponent>> newExps;

        // Linear, as we expect small monomials.
        auto itright = m->mExponents.begin();
        for(auto itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
        {
            // Done with division
            if(itright == m->mExponents.end())
            {
                // Insert remaining part
                newExps.insert(newExps.end(), itleft, mExponents.end());
                #ifdef USE_MONOMIAL_POOL
                std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create( std::move(newExps), (exponent)(mTotalDegree - m->mTotalDegree) );
                #else
                std::shared_ptr<const Monomial> result = std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), (exponent)(mTotalDegree - m->mTotalDegree) ) );
                #endif
                LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
                return std::make_pair(result,true);;
            }
            // Variable is present in both monomials.
            if(itleft->first == itright->first)
            {
                if (itleft->second < itright->second)
                {
                    // Underflow, itright->exp was larger than itleft->exp.
                    LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
                    return std::make_pair(nullptr,false);
                }
                exponent newExp = itleft->second - itright->second;
                if(newExp > 0)
                {
                    newExps.push_back(std::make_pair(itleft->first, newExp));
                }
                itright++;
            }
            // Variable is not present in lhs, division fails.
            else if(itleft->first > itright->first) 
            {
                LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
                return std::make_pair(nullptr,false);
            }
            else
            {
                assert(itright->first > itleft->first);
                newExps.push_back(*itleft);
            }
        }
        // If there remain variables in the m, it fails.
        if(itright != m->mExponents.end())
        {
            LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
            return std::make_pair(nullptr,false);
        }
        if (newExps.empty())
        {
            LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
            return std::make_pair(nullptr,true);
        }
        #ifdef USE_MONOMIAL_POOL
        std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create( std::move(newExps), (exponent)(mTotalDegree - m->mTotalDegree) );
        #else
        std::shared_ptr<const Monomial> result = std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), (exponent)(mTotalDegree - m->mTotalDegree) ) );
        #endif
        LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
        return std::make_pair(result,true);
    }
    
    
    std::shared_ptr<const Monomial> Monomial::calcLcmAndDivideBy(const std::shared_ptr<const Monomial>& m) const
    {
        std::vector<std::pair<Variable, exponent>> newExps;
        exponent tdegree = mTotalDegree;
        // Linear, as we expect small monomials.
        auto itright = m->mExponents.begin();
        for(auto itleft = mExponents.begin(); itleft != mExponents.end();)
        {
            // Done with division
            if(itright == m->mExponents.end())
            {
                // Insert remaining part
                newExps.insert(newExps.end(), itleft, mExponents.end());
                #ifdef USE_MONOMIAL_POOL
                return MonomialPool::getInstance().create( std::move(newExps), tdegree );
                #else
                return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), tdegree ) );
                #endif
            }
            // Variable is present in both monomials.
            if(itleft->first == itright->first)
            {
                exponent newExp = std::max(itleft->second, itright->second) - itright->second;
                if(newExp != 0)
                {
                    newExps.push_back(std::make_pair(itleft->first, newExp));
                    tdegree -= itright->second;
                }
                else
                {
                    tdegree -= itleft->second;
                }
                ++itright;
                ++itleft;
            }
            // Variable is not present in lhs, dividing lcm yields variable will not occur in result

            else if(itleft->first > itright->first) 
            {
                ++itright;
            }
            else
            {
                assert(itright->first > itleft->first);
                newExps.push_back(*itleft);
                ++itleft;
            }
        }
        #ifdef USE_MONOMIAL_POOL
        return MonomialPool::getInstance().create( std::move(newExps), tdegree);
        #else
        return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), tdegree ) );
        #endif
    }
    
    std::shared_ptr<const Monomial> Monomial::separablePart() const
    {
        std::vector<std::pair<Variable, exponent>> newExps;
        for (auto& it: mExponents)
        {
            newExps.push_back( std::make_pair( it.first, 1 ) );
        }
        #ifdef USE_MONOMIAL_POOL
        return MonomialPool::getInstance().create( std::move(newExps), (exponent)mExponents.size() );
        #else
        return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), (exponent)mExponents.size() ) );
        #endif
    }
    
    std::shared_ptr<const Monomial> Monomial::pow(unsigned exp) const
    {
        if (exp == 0)
        {
            return nullptr;
        }
        std::vector<std::pair<Variable, exponent>> newExps;
        exponent expsum = 0;
        for (auto& it: mExponents)
        {
            newExps.push_back( std::make_pair( it.first, (exponent)(it.second * exp) ) );
            expsum += newExps.back().second;
        }
        #ifdef USE_MONOMIAL_POOL
        return MonomialPool::getInstance().create( std::move(newExps), expsum );
        #else
        return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), expsum ) );
        #endif
    }
    
    std::string Monomial::toString(bool infix, bool friendlyVarNames) const
    {
        if(mExponents.empty()) return "1";
        std::stringstream ss;
        if (infix) {
            for (auto vp = mExponents.begin(); vp != mExponents.end(); ++vp) {
                if (vp != mExponents.begin()) ss << "*";
                ss << vp->first;
                if (vp->second > 1) ss << "^" << vp->second;
            }
        } else {
            if (mExponents.size() > 1) ss << "(* ";
            for (auto vp = mExponents.begin(); vp != mExponents.end(); ++vp) {
                if (vp != mExponents.begin()) ss << " ";
                if (vp->second == 1) ss << vp->first;
                else {
                    std::string varName = VariablePool::getInstance().getName(vp->first, friendlyVarNames);
                    ss << "(*";
                    for (unsigned i = 0; i < vp->second; i++) ss << " " << varName;
                    ss << ")";
                }
            }
            if (mExponents.size() > 1) ss << ")";
        }
        return ss.str();
    }
    
    std::shared_ptr<const carl::Monomial> Monomial::gcd(const std::shared_ptr<const Monomial>& rhs, const std::shared_ptr<const Monomial>& lhs)
    {
        if(!lhs && !rhs) return nullptr;
        if(!lhs) return rhs;
        if(!rhs) return lhs;
			
        LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
        assert(lhs->isConsistent());
        assert(rhs->isConsistent());

        std::vector<std::pair<Variable, exponent>> newExps;
        exponent expsum = 0;
        // Linear, as we expect small monomials.
        auto itright = rhs->mExponents.cbegin();
        auto leftEnd = lhs->mExponents.cend();
        auto rightEnd = rhs->mExponents.cend();
        for(auto itleft = lhs->mExponents.cbegin(); (itleft != leftEnd && itright != rightEnd);)
        {
            // Variable is present in both monomials.
            if(itleft->first == itright->first)
            {
                exponent newExp = std::min(itleft->second, itright->second);
                newExps.push_back(std::make_pair(itleft->first, newExp));
                expsum += newExp;
                ++itright;
                ++itleft;
            }

            else if(itleft->first > itright->first) 
            {
                ++itright;
            }
            else
            {
                assert(itright->first > itleft->first);
                ++itleft;
            }
        }
         // Insert remaining part
        #ifdef USE_MONOMIAL_POOL
        std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create( std::move(newExps), expsum );
        #else
        std::shared_ptr<const Monomial> result = std::shared_ptr<const Monomial>( new Monomial(  std::move(newExps), expsum ) );
        #endif
        LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
        return result;
    }
    
    std::shared_ptr<const carl::Monomial> Monomial::lcm(const std::shared_ptr<const Monomial>& lhs, const std::shared_ptr<const Monomial>& rhs)
    {
        if (!lhs && !rhs) return nullptr;
        if (!lhs) return rhs;
        if (!rhs) return lhs;
        LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
        assert(lhs->isConsistent());
        assert(rhs->isConsistent());

        std::vector<std::pair<Variable, exponent>> newExps;
        exponent expsum = lhs->tdeg() + rhs->tdeg();
        // Linear, as we expect small monomials.
        auto itright = rhs->mExponents.cbegin();
        auto leftEnd = lhs->mExponents.cend();
        auto rightEnd = rhs->mExponents.cend();
        for(auto itleft = lhs->mExponents.cbegin(); itleft != leftEnd;)
        {
            // Done on right
            if(itright == rightEnd)
            {
                // Insert remaining part
                newExps.insert(newExps.end(), itleft, lhs->mExponents.end());
                #ifdef USE_MONOMIAL_POOL
                std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create( std::move(newExps), expsum );
                #else
                std::shared_ptr<const Monomial> result = std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), expsum ) );
                #endif
                LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
                return result;
            }
            // Variable is present in both monomials.
            if(itleft->first == itright->first)
            {
                exponent newExp = std::max(itleft->second, itright->second);
                newExps.push_back(std::make_pair(itleft->first, newExp));
                expsum -= std::min(itleft->second, itright->second);
                ++itright;
                ++itleft;
            }
            // Variable is not present in lhs, dividing lcm yields variable will not occur in result

            else if(itleft->first > itright->first) 
            {
                newExps.push_back(*itright);
                ++itright;
            }
            else
            {
                assert(itright->first > itleft->first);
                newExps.push_back(*itleft);
                ++itleft;
            }
        }
         // Insert remaining part
        newExps.insert(newExps.end(), itright, rhs->mExponents.end());
        #ifdef USE_MONOMIAL_POOL
        std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create( std::move(newExps), expsum );
        #else
        std::shared_ptr<const Monomial> result = std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), expsum ) );
        #endif
        LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
        return result;
    }
    
    bool Monomial::isConsistent() const
    {
        LOG_FUNC("carl.core.monomial", mExponents << ", " << mTotalDegree << ", " << mHash);
        if (mTotalDegree < 1) return false;
        if (mHash == 0) return false;
        unsigned tdegree = 0;
        Variable lastVar = Variable::NO_VARIABLE;
        for(auto ve : mExponents)
        {
            if (ve.second <= 0) return false;
			if (lastVar != Variable::NO_VARIABLE) {
				if (ve.first > lastVar) return false;
			}
            tdegree += ve.second;
            lastVar = ve.first;
        }
        if (tdegree != mTotalDegree) return false;
        return true;
    }
    
    CompareResult Monomial::lexicalCompare(const Monomial& lhs, const Monomial& rhs)
    {
        if (&lhs == &rhs) {
            return CompareResult::EQUAL;
        }
        auto lhsit = lhs.mExponents.begin( );
        auto rhsit = rhs.mExponents.begin( );
        auto lhsend = lhs.mExponents.end( );
        auto rhsend = rhs.mExponents.end( );
        while( lhsit != lhsend )
        {
            if( rhsit == rhsend )
                return CompareResult::GREATER;
            //which variable occurs first
            if( lhsit->first == rhsit->first )
            {
                //equal variables
                if( lhsit->second < rhsit->second )
                    return CompareResult::LESS;
                if( lhsit->second > rhsit->second )
                    return CompareResult::GREATER;
            }
            else
            {
                return (lhsit->first < rhsit->first ) ? CompareResult::LESS : CompareResult::GREATER;
            }
            ++lhsit;
            ++rhsit;
        }
        if( rhsit == rhsend )
            return CompareResult::EQUAL;
        return CompareResult::LESS;
    }
    
    std::shared_ptr<const carl::Monomial> operator*(const Monomial::Arg& lhs, const Monomial::Arg& rhs)
    {
        LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
        if(!lhs)
            return rhs;
        if(!rhs)
            return lhs;
        assert( rhs->tdeg() > 0 );
        assert( lhs->tdeg() > 0 );
        std::vector<std::pair<Variable, exponent>> newExps;

        // Linear, as we expect small monomials.
        auto itleft = lhs->begin();
        auto itright = rhs->begin();
        while( itleft != lhs->end() && itright != rhs->end() )
        {
            // Variable is present in both monomials.
            if(itleft->first == itright->first)
            {
                newExps.emplace_back( itleft->first, itleft->second + itright->second);
                ++itleft;
                ++itright;
            }
            // Variable is not present in lhs, we have to insert var-exp pair from rhs.
            else if(itleft->first < itright->first)
            {
                newExps.push_back( *itright );
                ++itright;
            }		
            else 
            {
                newExps.push_back( *itleft );
                ++itleft;
            }
        }
        // Insert remaining.
        if( itleft != lhs->end() )
            newExps.insert(newExps.end(), itleft, lhs->end());
        else if( itright != rhs->end() )
            newExps.insert(newExps.end(), itright, rhs->end());
        #ifdef USE_MONOMIAL_POOL
        std::shared_ptr<const Monomial> result = MonomialPool::getInstance().create( std::move(newExps), lhs->tdeg() + rhs->tdeg() );
        #else
        std::shared_ptr<const Monomial> result = std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), lhs->tdeg() + rhs->tdeg() ) );
        #endif
        LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
        return result;
    }

    std::shared_ptr<const carl::Monomial> operator*(const Monomial::Arg& lhs, Variable::Arg rhs)
    {
        if( !lhs )
            return std::make_shared<const Monomial>( rhs );
        std::vector<std::pair<Variable, exponent>> newExps;
        // Linear, as we expect small monomials.
		bool inserted = false;
		for (const auto& p: *lhs) {
			if (inserted) newExps.push_back(p);
			else if (p.first > rhs) newExps.push_back(p);
			else if (p.first == rhs) {
				newExps.emplace_back(rhs, p.second + 1);
				inserted = true;
			} else if (p.first < rhs) {
				newExps.emplace_back(rhs, 1);
				newExps.push_back(p);
				inserted = true;
			}
		}
		if (!inserted) newExps.emplace_back(rhs, 1);
        #ifdef USE_MONOMIAL_POOL
        return MonomialPool::getInstance().create( std::move(newExps), lhs->tdeg() + 1 );
        #else
        return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), lhs->tdeg() + 1 ) );
        #endif
    }
    
    std::shared_ptr<const carl::Monomial> operator*(Variable::Arg lhs, const Monomial::Arg& rhs)
    {
        return rhs * lhs;
    }
    
    std::shared_ptr<const carl::Monomial> operator*(Variable::Arg lhs, Variable::Arg rhs)
    {
        std::vector<std::pair<Variable, exponent>> newExps;
        if( lhs < rhs )
        {
            newExps.emplace_back( rhs, 1 );
            newExps.emplace_back( lhs, 1 );
        }
        else if( lhs > rhs )
        {
            newExps.emplace_back( lhs, 1 );
            newExps.emplace_back( rhs, 1 );
        }
        else
            newExps.emplace_back( lhs, 2 );
        #ifdef USE_MONOMIAL_POOL
        return MonomialPool::getInstance().create( std::move(newExps), 2 );
        #else
        return std::shared_ptr<const Monomial>( new Monomial( std::move(newExps), 2 ) );
        #endif
    }
}
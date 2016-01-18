#include "BitVector.h"

namespace carl
{

BitVector operator|(const BitVector& lhs, const BitVector& rhs)
{
    BitVector res;
    auto lhsIt = lhs.mBits.begin();
    auto rhsIt = rhs.mBits.begin();

    auto lhsEnd = lhs.mBits.end();
    auto rhsEnd = rhs.mBits.end();

    while( true)
    {
        if( lhsIt == lhsEnd)
        {
            res.mBits.insert( res.mBits.end(), rhsIt, rhsEnd);
            break;
        }

        if( rhsIt == rhsEnd)
        {
            res.mBits.insert( res.mBits.end(), lhsIt, lhsEnd);
            break;
        }

        res.mBits.push_back( *lhsIt | *rhsIt);
        ++rhsIt;
        ++lhsIt;
    }

    return res;
}

bool operator ==(const BitVector& lhs, const BitVector& rhs)
{
    return(lhs.mBits == rhs.mBits);
}

bool operator ==(const BitVector::forward_iterator& fi1, const BitVector::forward_iterator& fi2)
{
    return(fi1.posInVec == fi2.posInVec && fi1.vecIter == fi2.vecIter);
}

bool BitVector::subsetOf( const BitVector& superset)
{
    std::vector<unsigned>::const_iterator sub = mBits.begin();
    auto sup = superset.mBits.begin();

    if( sub == mBits.end())
    {
        return true;
    }

    while( sup != superset.mBits.end())
    {
        if((( *sub) & ~( *sup)) != 0)
        {
            return false;
        }

        ++sup;
        ++sub;
        if( sub == mBits.end())
        {
            return true;
        }
    }
    while( sub != mBits.end())
    {
        if( *sub != 0) return false;
    }
    return true;
}
}

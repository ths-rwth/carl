/**
 * @file SPolPair.h
 * @ingroup gb
 * @author Sebastian Junges
 */
#pragma once 

#include <carl-arith/poly/umvpoly/Monomial.h>

namespace carl 
{
    /**
     * Basic spol-pair. Optimizations could be deducing p2 from the structure where it is saved, and not saving the lcm.
     * Also sugar might be added.
     * @param p1 index of polynomial p1
     * @param p2 index of polynomial p2
     * @param lcm the lcm(lt(p1), lt(p2))
     */
    struct SPolPair
    {
        SPolPair( std::size_t p1, std::size_t p2, Monomial::Arg lcm ) : mP1(p1), mP2(p2), mLcm(std::move(lcm))
        {}

        const std::size_t mP1;
        const std::size_t mP2;
        const Monomial::Arg mLcm;

        void print(std::ostream& os = std::cout) const
        {
            os << "(" << mP1 << "," << mP2 << "): " << mLcm;
        }
    };

    template <class Compare>
    struct SPolPairCompare
    {
        bool operator( )(const SPolPair& s1, const SPolPair & s2 )
        {
            return Compare::less( s1.mLcm, s2.mLcm );
        }
    };
}

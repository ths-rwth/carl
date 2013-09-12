/* 
 * File:   PointIntervalFactory.h
 * Author: Stefan Schupp
 *
 * Created on September 12, 2013, 10:41 AM
 */

#pragma once;

#include "DoubleInterval.h"

namespace carl
{
template<typename Rational> class PointIntervalFactory
{
    private:
        /**
         * Members
         */
        std::map<Rational, DoubleInterval> mIntervals;
        
    public:
        PointIntervalFactory()
        {
            init();
        }
        
        
        ~PointIntervalFactory();
        
        
        const DoubleInterval& getPointInterval(const Rational& _rat)
        {
            if(mIntervals.count(_rat) > 0)
            {
                return mIntervals.at(_rat);
            }
            else
            {
                // create PointInterval without overapproximation
                return mIntervals.emplace(_rat,DoubleInterval(_rat,false)).first;
            }
        }
        
    private:
        void init()
        {
            mIntervals.emplace((Rational) 1, DoubleInterval((Rational) 1, false));
        }
};

}//namespace
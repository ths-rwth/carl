/** 
 * @file   IntervalEvaluation.h
 *         Created on August 23, 2013, 1:12 PM
 * @author: Sebastian Junges
 *
 * 
 */


#pragma once

namespace carl
{
class IntervalEvaluation
{
public:
    static DoubleInterval evaluate(const Monomial& m, const std::map<Variable, DoubleInterval>&);
    
private:

};
}



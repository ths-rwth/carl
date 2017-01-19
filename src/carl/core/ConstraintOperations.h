/** 
 * @file   core/ConstraintOperations.h
 * @author Sebastian Junges
 * @ingroup constraints
 *
 */
 
#pragma once

#include <iterator>

#include "../formula/Constraint.h"
#include "RationalFunction.h"



namespace carl 
{
	namespace constraints 
	{
		template<typename PolType, bool AS, typename InIt, typename InsertIt>
		void toPolynomialConstraints(InIt start, 
									 InIt end,
									 InsertIt out) 
		{
			using PCon = Constraint<PolType>;
		
			// TODO add static assertion.InsertIt
			for(auto it = start; it != end; ++it)
			{
				if(it->isTrivialTrue()) 
				{
					out = PCon(true);
				}
				else if(it->isTrivialFalse())
				{
					out = PCon(false);
				}
				else if(it->lhs().denominator().isOne())
				{
					out = PCon(it->lhs().nominator(), it->rel());
				} 
				else 
				{
					assert(!it->lhs().denominator().isConstant());
					out = PCon(it->lhs().nominator(), inverse(it->rel()));
					out = PCon(it->lhs().nominator() * it->lhs().denominator(), it->rel());
				}
				
				
			}
		}
	}
}

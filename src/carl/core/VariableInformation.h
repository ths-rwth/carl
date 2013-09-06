/** 
 * @file:   VariableInformation.h
 * @author: Sebastian Junges
 *
 * @since September 3, 2013
 */

#pragma once
#include <map>

#include "VarExpPair.h"

namespace carl
{
	template<bool collectCoeff, typename CoeffType>
	struct VariableInformation
	{
		
	};

	template<typename CoeffType>
	struct VariableInformation<false, CoeffType>
	{
		VariableInformation(unsigned degreeOfOccurence)
		:	maxDegree(degreeOfOccurence), 
			minDegree(degreeOfOccurence), 
			occurence(1)
		{
			
		}
		/// Maximal degree variable occurs with.
		unsigned maxDegree;
		/// Minimal non-zero degree variable occurs with.
		unsigned minDegree;
		/// Number of terms a variable occurs in.
		unsigned occurence;
		
		template<typename Term>
		void updateCoeff(unsigned, const Term&)
		{
			// Empty function, we do not save the coefficient here.
			// TODO there might be a better solution to this.
		}
	};
	
	template<typename CoeffType>
	struct VariableInformation<true, CoeffType> : public VariableInformation<false, CoeffType>
	{
		VariableInformation(unsigned degreeOfOccurence) :  VariableInformation<false, CoeffType>(degreeOfOccurence)
		{
		}
		
	
		std::map<unsigned, CoeffType> coeffs;
		
		template<typename Term>
		void updateCoeff(unsigned exponent, const Term& t)
		{
			typename std::map<unsigned, CoeffType>::iterator it = coeffs.find(exponent);
			if(it == coeffs.end())
			{
				// TODO no support for map emplace in GCC 4.7. 
				// coeffs.emplace(exponent, CoeffType(t));
				coeffs.insert(std::make_pair(exponent, CoeffType(t)));
				
			}
			else
			{
				it->second += t;
			}
		}
	};

}
	

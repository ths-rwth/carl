/** 
 * @file:   VariableInformation.h
 * @author: Sebastian Junges
 *
 * @since September 3, 2013
 */

#pragma once
#include <map>

namespace carl
{
	template<bool collectCoeff, typename CoeffType>
	struct VariableInformation
	{
		VariableInformation() 
		{
			std::cout << "MG" << std::endl;
		}
		
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
		
		void updateCoeff()
		{
			// Empty function, we do not save the coefficient here.
		}
	};
	
	template<typename CoeffType>
	struct VariableInformation<true, CoeffType> : public VariableInformation<false, CoeffType>
	{
		VariableInformation(unsigned degreeOfOccurence) :  VariableInformation<false, CoeffType>(degreeOfOccurence)
		{
		}
		
	
		std::map<unsigned, CoeffType> coeff;
		
		void updateCoeff()
		{
			
		}
	};

}
	

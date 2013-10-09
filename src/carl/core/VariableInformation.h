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
	class VariableInformation<false, CoeffType>
	{
		
		/// Maximal degree variable occurs with.
		unsigned mMaxDegree;
		/// Minimal non-zero degree variable occurs with.
		unsigned mMinDegree;
		/// Number of terms a variable occurs in.
		unsigned mOccurence;
	public:
		VariableInformation(unsigned degreeOfOccurence)
		:	mMaxDegree(degreeOfOccurence), 
			mMinDegree(degreeOfOccurence), 
			mOccurence(1)
		{
			
		}
		
		unsigned maxDegree() const
		{
			return mMaxDegree;
		}
		
		unsigned minDegree() const
		{
			return mMinDegree;
		}
		
		unsigned occurence() const
		{
			return mOccurence;
		}
		
		/**
		 * If degree is larger than maxDegree, we set the maxDegree to degree.
         * @param degree
		 * @return true if degree was larger.
         */
		bool raiseMaxDegree(unsigned degree)
		{
			if(mMaxDegree < degree)
			{
				mMaxDegree = degree;
				return true;
			}
			return false;
		}
		
		/**
		 * If degree is smaller than minDegree, we set the minDegree to degree.
         * @param degree
		 * @return true if degree was smaller.
         */
		bool lowerMinDegree(unsigned degree)
		{
			if(mMinDegree > degree)
			{
				mMinDegree = degree;
				return true;
			}
			return false;
		}
		
		
		void increaseOccurence()
		{
			++mOccurence;
		}
		
		template<typename Term>
		void updateCoeff(unsigned, const Term&)
		{
			// Empty function, we do not save the coefficient here.
			// TODO there might be a better solution to this.
		}
	};
	
	template<typename CoeffType>
	class VariableInformation<true, CoeffType> : public VariableInformation<false, CoeffType>
	{
		std::map<unsigned, CoeffType> mCoeffs;
	public:
		VariableInformation(unsigned degreeOfOccurence) :  VariableInformation<false, CoeffType>(degreeOfOccurence)
		{
		}
		
	
		const std::map<unsigned, CoeffType>& coeffs() const
		{
			return mCoeffs;
		}
		
		
		template<typename Term>
		void updateCoeff(unsigned exponent, const Term& t)
		{
			typename std::map<unsigned, CoeffType>::iterator it = mCoeffs.find(exponent);
			if(it == mCoeffs.end())
			{
				// TODO no support for map emplace in GCC 4.7. 
				// coeffs.emplace(exponent, CoeffType(t));
				mCoeffs.insert(std::make_pair(exponent, CoeffType(t)));
				
			}
			else
			{
				it->second += t;
			}
		}
	};

}
	

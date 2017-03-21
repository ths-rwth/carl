/** 
 * @file:   VariablesInformation.h
 * @author: Sebastian Junges
 *
 * @since September 3, 2013
 */

#pragma once

#include "Variable.h"
#include "VariableInformation.h"

namespace carl
{

class VariablesInformationInterface
{
public:
	virtual ~VariablesInformationInterface() = default;
	virtual bool hasCoeff() const = 0;
};

template<bool collectCoeff, typename CoeffType>
class VariablesInformation : public VariablesInformationInterface
{
	std::map<Variable, VariableInformation<collectCoeff, CoeffType>> mVariableInfo;
	
public:
    VariablesInformation(): mVariableInfo() {}
    VariablesInformation(std::map<Variable, VariableInformation<collectCoeff, CoeffType>>&& _varInfos): mVariableInfo(_varInfos) {}
	~VariablesInformation() override = default;

	bool hasCoeff() const override
	{
		return collectCoeff;
	}
    
    typename std::map<Variable, VariableInformation<collectCoeff, CoeffType>>::const_iterator cbegin()
    {
        return mVariableInfo.begin();
    }
    
    typename std::map<Variable, VariableInformation<collectCoeff, CoeffType>>::const_iterator cend()
    {
        return mVariableInfo.end();
    }
    
    typename std::map<Variable, VariableInformation<collectCoeff, CoeffType>>::iterator begin()
    {
        return mVariableInfo.begin();
    }
    
    typename std::map<Variable, VariableInformation<collectCoeff, CoeffType>>::iterator end()
    {
        return mVariableInfo.end();
    }
    
	/**
	 * Updates the Variable informations based on the assumption that this method is called with according parameters
     * @param ve A variable-exponent pair occuring in a term t.
     * @param termCoeff The coefficient of t.
	 * @param monomial The monomial part of t.
     */
	template<typename TermCoeff>
	void variableInTerm(const std::pair<Variable, exponent>& ve, const TermCoeff& termCoeff, const typename CoeffType::MonomType& monomial)
	{
		auto it = mVariableInfo.find(ve.first);
		// Variable did not occur before.
		if(it == mVariableInfo.end())
		{
			// TODO GCC 4.7 does not support the (faster) emplace on maps
			//it = mVariableInfo.emplace(ve.first, VariableInformation<collectCoeff, CoeffType>(ve.second)).first;
			it = mVariableInfo.insert(std::make_pair(ve.first, VariableInformation<collectCoeff, CoeffType>(ve.second))).first;
		}
		else
		{
			// One more term in which the variable occurs.
			it->second.increaseOccurence();
			// Update minimal/maximal degree.
			if(!it->second.raiseMaxDegree(ve.second))
			{
				// Only if raising failed, lowering can be successful.
				it->second.lowerMinDegree(ve.second);
			}
		}
		if(collectCoeff)
		{
			typename CoeffType::MonomType::Arg m = monomial.dropVariable(ve.first);
			if(m == nullptr)
			{
				it->second.updateCoeff(ve.second, typename CoeffType::TermType(termCoeff));
			}
			else
			{
				it->second.updateCoeff(ve.second, typename CoeffType::TermType(termCoeff, m));
			}
		}
	}
	
	const VariableInformation<collectCoeff, CoeffType>* getVarInfo(Variable::Arg v) const
	{
		auto it = mVariableInfo.find(v);
		// Variable did not occur before.
		if(it == mVariableInfo.end())
		{
			return nullptr;
		}
		else
		{
			return &(it->second);
		}
	}
	
	bool occurs(Variable::Arg v) const
	{
		return mVariableInfo.count(v) > 0;
	}
};
}

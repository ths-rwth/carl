 /**
 * @file	MultivariateHorner.h
 * @author	Lukas Netz
 *
 */

#pragma once
#include <vector>
#include "Variable.h"
#include <set>
#include "carl/core/MultivariatePolynomial.h"
#include "carl/interval/Interval.h"

namespace carl{

enum Strategy {
	GREEDY_I = 0, 
	GREEDY_Is = 1,
	GREEDY_II = 2,
	GREEDY_IIs = 3
};

template<typename PolynomialType, Strategy Strat>
class MultivariateHorner { 

/**
* Datastructure to save Polynomes once they are transformed into a horner scheme:
*
* h = Variable * h_dependent + h_independent || Variable * const_dependent + const_independent || Variable * h_dependent + const_independent
*
*/


private:
	typedef typename MultivariatePolynomial<PolynomialType>::CoeffType CoeffType;

	CoeffType mConst_dependent = constant_zero<CoeffType>::get();
	CoeffType mConst_independent = constant_zero<CoeffType>::get();
	Variable mVariable = Variable::NO_VARIABLE;
	unsigned mExponent = 1;
	MultivariateHorner* mH_dependent = NULL;
	MultivariateHorner* mH_independent = NULL;

	
public:
		
	//Constuctor
	MultivariateHorner (const PolynomialType& inPut);
	MultivariateHorner (const PolynomialType& inPut, Strategy s);

	
	//TODO Why does Destructor not work ??
	//~MultivariateHorner ();
	
	MultivariateHorner& operator=(const MultivariateHorner& mh)
	{
		mConst_dependent = mh.mConst_dependent;
		mConst_independent = mh.mConst_independent;

	 	mH_dependent = mh.mH_dependent;
	 	mH_independent = mh.mH_independent;	

	 	mVariable = mh.mVariable;
	 	mExponent = mh.mExponent;		
		return *this;
	}


/*
	MultivariateHorner& operator=(MultivariateHorner&& mh) 
	{
		mConst_dependent = std::move(mh.mConst_dependent);
		mConst_independent = std::move(mh.mConst_independent);	 	
	 
	 	mH_dependent = std::move(mh.mH_dependent);
	 	mH_independent = std::move(mh.mH_independent);		
	 
	 	mVariable = mh.mVariable;		
	 	mExponent = mh.mExponent;
		return *this;
	}
*/

	//getter and setters
	Variable getVariable() const
	{
		return mVariable;
	}

	void setVariable(const Variable::Arg& var) 
	{
		mVariable = var;
	}

	MultivariateHorner* getDependent() const
	{
		return mH_dependent;
	}

	void removeDependent()
	{
		mH_dependent = NULL;
	}

	void removeIndepenent() 
	{
		mH_independent = NULL;
	}

	void setDependent(MultivariateHorner* dependent)
	{
		mH_dependent = dependent;
	}

	MultivariateHorner* getIndependent() const
	{
		return mH_independent;
	}

	void setIndependent(MultivariateHorner* independent)
	{
		mH_independent = independent;
	}

	const CoeffType& getDepConstant() const
	{
		return mConst_dependent;
	}

	void setDepConstant(const CoeffType& constant)
	{
		mConst_dependent = constant;
	}	

	const CoeffType& getIndepConstant() const
	{
		return mConst_independent;
	}

	void setIndepConstant(const CoeffType& constant){
		mConst_independent = constant;
	}

	unsigned getExponent() const
	{
		return mExponent;
	}

	void setExponent(const unsigned& exp){
		mExponent = exp;
	}

}; //Class MultivarHorner

}//namespace carl
#include "MultivariateHorner.tpp"



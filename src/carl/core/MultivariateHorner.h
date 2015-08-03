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

template<typename PolynomialType >
class MultivariateHorner { 

/**
* Datastructure to save Polynomes once they are transformed into a horner scheme:
*
* h = Variable * h_dependent + h_independent || Variable * const_dependent + const_independent || Variable * h_dependent + const_independent
*
*/
public:
	/// Type of the coefficients. 	
	
	typedef typename MultivariatePolynomial<PolynomialType>::CoeffType CoeffType;

	CoeffType mConst_dependent = constant_zero<CoeffType>::get();
	CoeffType mConst_independent = constant_zero<CoeffType>::get();
	Variable mVariable = Variable::NO_VARIABLE;
	unsigned mExponent = 1;
	MultivariateHorner* mH_dependent = NULL;
	MultivariateHorner* mH_independent = NULL;
	
	//Constuctor
	MultivariateHorner (const PolynomialType& inPut);

	MultivariateHorner& operator=(const MultivariateHorner& mh) {
		mConst_dependent = mh.mConst_dependent;
		mConst_independent = mh.mConst_independent;
	 	mH_dependent = mh.mH_dependent;
	 	mH_independent = mh.mH_independent;		
	 	mVariable = mh.mVariable;
	 	mExponent = mh.mExponent;		
		return *this;
	}


/*
	MultivariateHorner& operator=(MultivariateHorner&& mh) {
		mConst_dependent = std::move(mh.mConst_dependent);
		mConst_independent = std::move(mh.mConst_independent);
	 	mH_dependent = std::move(mh.mH_dependent);
	 	mH_independent = std::move(mh.mH_independent);		
	 	mVariable = std::move(mh.mVariable);		
	 	mExponent = std::move(mh.mExponent);
		return *this;
	}
*/

	//getter and setters
	Variable getVariable(){
		return mVariable;
	}

	void setVariable(const Variable::Arg& var){
		mVariable = var;
	}

	MultivariateHorner* getDependent(){
		return mH_dependent;
	}

	void setDependent( MultivariateHorner* dependent){
		mH_dependent = dependent;
	}

	MultivariateHorner* getIndependent(){
		return mH_independent;
	}

	void setIndependent( MultivariateHorner* independent){
		mH_independent = independent;
	}

	CoeffType& getDepConstant(){
		return mConst_dependent;
	}

	void setDepConstant(CoeffType constant){
		mConst_dependent = constant;
	}	

	CoeffType& getIndepConstant(){
		return mConst_independent;
	}

	void setIndepConstant(const CoeffType& constant){
		mConst_independent = constant;
	}	

//
//	template<typename Number>
//	static Interval<Number> evaluate(const MultivariateHorner<PolynomialType> mvH, std::map<Variable, Interval<Number>>& map);

}; //Class MultivarHorner

}//namespace carl
#include "MultivariateHorner.tpp"



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

namespace carl{

template< typename PolynomialType >
class MultivariateHorner { 

/**
* Datastructure to save Polynomes once they are transformed into a horner scheme:
*
* h = Variable^(Exponent) * SUM(h_dependent) + SUM(h_independent) + constant
*
*/
public:
	/// Type of the coefficients. 	
	
	typedef typename PolynomialType::CoeffType CoeffType;
	CoeffType mCoeff = constant_zero<CoeffType>::get();
	Variable mVariable;
	MultivariateHorner *mDependent;
	MultivariateHorner *mIndependent;
	
	//Constuctor
	MultivariateHorner (const PolynomialType& inPut);

	//getter and setters
	Variable getVariable(){
		return mVariable;
	}

	void setVariable(Variable::Arg var){
		mVariable = var;
	}

	const MultivariateHorner& getDependent(){
		return *mDependent;
	}

	void setDependent(const MultivariateHorner& dependent){
		mDependent = *dependent;
	}

	const MultivariateHorner& getIndependent(){
		return *mIndependent;
	}

	void setIndependent(const MultivariateHorner& independent){
		*mIndependent = independent;
	}

	CoeffType& getConstant(){
		return mCoeff;
	}

	void setConstant(const CoeffType& constant){
		mCoeff = constant;
	}	

	MultivariateHorner& operator=(const MultivariateHorner& mh) {

		mCoeff = mh.mCoeff;
		std::cout << "A" << std::endl;
	 	mDependent = mh.mDependent;
	 	std::cout << "B" << std::endl;
	 	mIndependent = mh.mIndependent;		
	 	std::cout << "C" << std::endl;
	 	mVariable = mh.mVariable;
		
		return *this;
	}
};

}//namespace carl

#include "MultivariateHorner.tpp"

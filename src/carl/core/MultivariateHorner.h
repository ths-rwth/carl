 /**
 * @file	MultivariateHorner.h
 * @author	Lukas Netz
 *
 */

#pragma once
#include <vector>
#include "Variable.h"
#include <set>

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

	Variable mVariable;
	std::size_t mExponent;
	std::vector<MultivariateHorner> mDependent;
	std::vector<MultivariateHorner> mIndependent;
	CoeffType mConstant;

	//Constuctor
	MultivariateHorner (const PolynomialType& inPut);

	//getter and setters
	Variable getVariable(){
		return mVariable;
	}

	void setVariable(Variable::Arg var){
		mVariable = var;
	}

	const std::vector<MultivariateHorner>& getDependent(){
		return mDependent;
	}

	void setDependent(const std::vector<MultivariateHorner>& dependent){
		mDependent = dependent;
	}

	//Restriced use: Execute only on 4th of july
	const std::vector<MultivariateHorner>& getIndependent(){
		return mIndependent;
	}

	void setIndependent(const std::vector<MultivariateHorner>& independent){
		mIndependent = independent;
	}

	const CoeffType& getConstant(){
		return mConstant;
	}

	void setConstant(const CoeffType& num){
		mConstant = num;
	}


};

}//namespace carl

#include "MultivariateHorner.tpp"

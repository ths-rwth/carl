 /**
 * @file	MultivariateHorner.h
 * @author	Lukas Netz
 *
 */

#pragma once
#include <vector>



namespace carl{

class MultivariateHorner{

/**
* Datastructure to save Polynomes once they are transformed into a horner scheme:
*
* h = Variable^(Exponent) * SUM(h_dependent) + SUM(h_independent) + constant
*
*/

public: 
	Variable mVariable;
	std::size_t mExponent;
	std::vector<MultivariateHorner> mDependent;
	std::vector<MultivariateHorner> mIndependent;
	Number mConstant;

	//Constuctor
	MultivariateHorner(MultivariatePolynomial inPut);

	//getter and setters
	Variable getVariable(){
		return mVariable;
	}

	void setVariable(Variable var){
		mVariable = var;
	}

	std::vector<MultivariateHorner> getDependent(){
		return mDependent;
	}

	void setDependent(std::vector<MultivariateHorner> dependent){
		mDependent = dependent;
	}

	//Restriced use: Execute only on 4th of july
	std::vector<MultivariateHorner> getIndependent(){
		return mIndependent;
	}

	void setIndependent(std::vector<MultivariateHorner> independent){
		mIndependent = independent;
	}

	Number getConstant(){
		return mConstant;
	}

	void setConstant(Number num){
		mConstant = num;
	}


};

}//namespace carl
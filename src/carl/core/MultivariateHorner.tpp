 /**
 * @file	MultivariateHorner.tpp
 * @author	Lukas Netz
 *
 */

#pragma once
#include "carl/core/Term.h"

 #ifndef HORNER_Minimize_arithmetic_operations
 #define HORNER_Minimize_arithmetic_operations 1
 #endif

namespace carl
{
	template< typename PolynomialType > 
	MultivariateHorner< PolynomialType >::MultivariateHorner (const PolynomialType& inPut) {

		std::set<Variable> allVariablesinPolynome;
		inPut.gatherVariables(allVariablesinPolynome);
		std::cout << "## " << allVariablesinPolynome.size() << std::endl;

		if (HORNER_Minimize_arithmetic_operations)
		{
			//Detecting amounts of Variables in Monomials
			std::set<Variable>::iterator variableIt;
			std::set<Variable>::iterator bestVariable;
			unsigned int bestCounter = 0;

			if (allVariablesinPolynome.size() != 0)
			{
				for (variableIt = allVariablesinPolynome.begin(); variableIt != allVariablesinPolynome.end(); variableIt++)
				{	
					unsigned int counter = 0;
					typename PolynomialType::TermsType::const_iterator polynomialIt;
					for (polynomialIt = inPut.begin(); polynomialIt != inPut.end(); polynomialIt++)
					{
						if (polynomialIt->has(*variableIt))
						{
							counter++;
						}
					}
					
					//saving most promising Variable for later 
					if ( counter >= bestCounter ) {
						bestCounter = counter;
						bestVariable = variableIt;					
					}
				}
				this->setVariable(*bestVariable);				

				std::cout << "Polynome: " << inPut << std::endl;
				std::cout << "  Choosen Var: " << *bestVariable << std::endl;

				typename PolynomialType::TermsType::const_iterator polynomialIt;
				typename PolynomialType::TermType tempTerm;

				typename PolynomialType::PolyType indepPolynome;
				typename PolynomialType::PolyType depPolynome;

				//Choose Terms from Polynome denpending on dependency on choosen Variable
				for (polynomialIt = inPut.begin(); polynomialIt != inPut.end(); polynomialIt++)
				{
					if (polynomialIt->has(*bestVariable))
					{
						//divide dependent terms by choosen Variable
						polynomialIt->divide(*bestVariable, tempTerm);
						depPolynome.addTerm( tempTerm );
					}
					else 
					{
						indepPolynome.addTerm( *polynomialIt );
					}
				}

				std::cout << "  INDEP:" << indepPolynome << std::endl;
				std::cout << "  DEP:" << depPolynome << std::endl;

				if (depPolynome.nrTerms() != 0)
				{
					std::cout << ">>>> DEP: Start" << std::endl;
					MultivariateHorner< PolynomialType > depHorner (depPolynome);	
					this->mDependent = &depHorner;
					//this->setDependent( depHorner );					
					std::cout << "<<<< DEP: end" << std::endl;
				}
				else
				{
					std::cout << "EMPTY  dep Polynome" << std::endl;
					//this->setConstant (depPolynome.coefficient() );
					this->mCoeff = depPolynome.coefficient();
				}
				
				if (indepPolynome.nrTerms() != 0 )
				{
					std::cout << ">>>> INDEP: Start" << std::endl;
					MultivariateHorner< PolynomialType > indepHorner (indepPolynome);
					this->mIndependent = &indepHorner;
					//this->setIndependent (indepHorner);
 					std::cout << "<<<< INDEP: end" << std::endl;
				}
				else
				{
					std::cout << "EMPTY  indep Polynome" << std::endl;
					this->mCoeff = indepPolynome.coefficient();
					//this->setConstant (indepPolynome.coefficient() );
				}				
				
			} //if there are no Variables in the polynome
			
			else 
			{		
				this->setVariable( Variable::NO_VARIABLE );
			}
			

		}//minimize arithmatic operations
	};


template< typename PolynomialType > 
std::ostream& operator<<(std::ostream& os, MultivariateHorner<PolynomialType>& mvH)
{
	if (mvH.mDependent == NULL)
	{
		return (os << mvH.getConstant().toString());
	}
	else
	{
		return (os << mvH.getVariable().toString() << " * (" << mvH->mDependent << ") + (" << mvH->mIndependent << ")" );	
	}
	
}
	

}//namespace carl
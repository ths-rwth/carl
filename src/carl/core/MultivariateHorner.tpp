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

		if (HORNER_Minimize_arithmetic_operations)
		{
			
			std::set<Variable>::iterator variableIt;
			std::set<Variable>::iterator selectedVariable;
			std::set<Variable> allVariablesinPolynome;
			inPut.gatherVariables(allVariablesinPolynome);

			unsigned int monomials_containingChoosenVar = 0;

			if (allVariablesinPolynome.size() != 0)
			{
				//Detecting amounts of Variables in Monomials
				for (variableIt = allVariablesinPolynome.begin(); variableIt != allVariablesinPolynome.end(); variableIt++)
				{	
					unsigned int monomialCounter = 0;
					typename PolynomialType::TermsType::const_iterator polynomialIt;
					for (polynomialIt = inPut.begin(); polynomialIt != inPut.end(); polynomialIt++)
					{
						if (polynomialIt->has(*variableIt))
						{
							monomialCounter++;
						}
					}
					
					//saving most promising Variable for later 
					if ( monomialCounter >= monomials_containingChoosenVar ) {
						monomials_containingChoosenVar = monomialCounter;
						selectedVariable = variableIt;					
					}
				}

				//Setting the choosen Variable for the current Hornerscheme iterartion
				this->setVariable(*selectedVariable);			

				#ifdef DEBUG_HORNER
				std::cout << "Polynome: " << inPut << std::endl;
				std::cout << "  Choosen Var: " << *selectedVariable << std::endl;
 				#endif
				
				typename PolynomialType::TermsType::const_iterator polynomialIt;
				typename PolynomialType::TermType tempTerm;

				typename PolynomialType::PolyType h_independentPart;
				typename PolynomialType::PolyType h_dependentPart;

				//Choose Terms from Polynome denpending on dependency on choosen Variable
				for (polynomialIt = inPut.begin(); polynomialIt != inPut.end(); polynomialIt++)
				{
					if (polynomialIt->has(*selectedVariable))
					{
						//divide dependent terms by choosen Variable
						polynomialIt->divide(*selectedVariable, tempTerm);
						h_dependentPart.addTerm( tempTerm );
					}
					else 
					{
						h_independentPart.addTerm( *polynomialIt );
					}
				}

				//If Dependent Polynome contains Variables - continue with recursive Horner
				if ( !h_dependentPart.isNumber() )
				{
					mH_dependent = new MultivariateHorner< PolynomialType >(h_dependentPart);
					mConst_dependent = constant_zero<CoeffType>::get();			
				}
				
				//Dependent Polynome is a Constant (Number) - save to memberVar
				else 
				{
					mH_dependent = NULL;
					mConst_dependent = h_dependentPart.constantPart();
				}			

				//If independent Polynome contains Variables - continue with recursive Horner
				if ( !h_independentPart.isNumber() )
				{
					mH_independent = new MultivariateHorner< PolynomialType >(h_independentPart);
					mConst_independent = constant_zero<CoeffType>::get();
				}
				//Independent Polynome is a Constant (Number) - save to memberVar
				else
				{
					mH_independent = NULL;
					mConst_independent = h_independentPart.constantPart();
				}					
			} 
						
			//If there are no Variables in the polynome
			else 
			{		
				mH_independent = NULL;
				mH_dependent = NULL;
				mConst_independent = inPut.constantPart();
				this->setVariable( Variable::NO_VARIABLE );
			}
			

		}//minimize arithmatic operations
	};


/**
	 * Streaming operator for multivariate HornerSchemes.
	 * @param os Output stream.
	 * @param rhs HornerScheme.
	 * @return `os`.
	 */
template< typename PolynomialType > 
std::ostream& operator<<(std::ostream& os, MultivariateHorner<PolynomialType>& mvH)
{
	if (mvH.mH_dependent != NULL && mvH.mH_independent != NULL)
	{
		if (mvH.mExponent != 1)
		{
			return (os << mvH.mVariable <<"^"<< mvH.mExponent << " * (" << *mvH.mH_dependent << ") + " << *mvH.mH_independent);		
		}
		else
		{
			return (os << mvH.mVariable << " * (" << *mvH.mH_dependent << ") + " << *mvH.mH_independent);			
		}
		
	}
	else if (mvH.mH_dependent != NULL && mvH.mH_independent == NULL)
	{
		if (mvH.mConst_independent == 0)
		{
			if (mvH.mExponent != 1)
			{
				return (os << mvH.mVariable <<"^"<< mvH.mExponent << " * (" << *mvH.mH_dependent << ")" );			
			}
			else
			{
				return (os << mvH.mVariable << " * (" << *mvH.mH_dependent << ")" );				
			}
		}
		else
		{
			if (mvH.mExponent != 1)
			{
				return (os << mvH.mVariable <<"^"<< mvH.mExponent << " * (" << *mvH.mH_dependent << ") + " << mvH.mConst_independent);	
			}
			else
			{
				return (os << mvH.mVariable << " * (" << *mvH.mH_dependent << ") + " << mvH.mConst_independent);		
			}
		}
		
	}
	else if (mvH.mH_dependent == NULL && mvH.mH_independent != NULL)
	{
		if (mvH.mConst_dependent == 1)
		{
			if (mvH.mExponent != 1)
			{
				return (os << mvH.mVariable <<"^"<< mvH.mExponent << " + " << *mvH.mH_independent );			
			}
			else
			{
				return (os << mvH.mVariable << " + " << *mvH.mH_independent );		
			}
			
		}
		else
		{
			if (mvH.mExponent != 1)
			{
				return (os << mvH.mConst_dependent << mvH.mVariable <<"^"<< mvH.mExponent << " + " << *mvH.mH_independent );		
			}
			else
			{
				return (os << mvH.mConst_dependent << mvH.mVariable <<" + " << *mvH.mH_independent );			
			}
		}
	}	
	else //if (mvH.mH_dependent == NULL && mvH.mH_independent == NULL)
	{
		if (mvH.mConst_independent == 0)
		{
			if (mvH.mConst_dependent == 1)
			{
				if (mvH.mExponent != 1)
				{
					return (os << mvH.mVariable <<"^"<< mvH.mExponent );	
				}
				else
				{
					return (os << mvH.mVariable);	
				}
				
			}
			else
			{
				if (mvH.mExponent != 1)
				{
					return (os << mvH.mConst_dependent << mvH.mVariable <<"^"<< mvH.mExponent);	
				}
				else
				{
					return (os << mvH.mConst_dependent << mvH.mVariable);	
				}
			}
		}
		else
		{
			if (mvH.mExponent != 1)
			{
				return (os << mvH.mConst_dependent << mvH.mVariable <<"^"<< mvH.mExponent << " + " << mvH.mConst_independent);	
			}
			else
			{
				return (os << mvH.mConst_dependent << mvH.mVariable <<" + " << mvH.mConst_independent);	
			}

		}
	}
}

template<typename PolynomialType>
MultivariateHorner<PolynomialType> simplify( MultivariateHorner<PolynomialType>& mvH)
{
	#ifdef DEBUG_HORNER
	std::cout << mvH << std::endl;	
	#endif

	if (mvH.mH_dependent != NULL && (mvH.mH_dependent->mH_dependent != NULL || mvH.mH_dependent->mConst_dependent != 0) && mvH.mH_dependent->mH_independent == NULL && mvH.mH_dependent->mConst_independent == 0 )
	{
		if (mvH.mVariable == mvH.mH_dependent->mVariable)
		{
			mvH.mExponent += mvH.mH_dependent->mExponent;

			if (mvH.mH_dependent->mH_dependent != NULL)
			{
				*mvH.mH_dependent = simplify(*mvH.mH_dependent->mH_dependent);	
			} 
			else if (mvH.mH_dependent->mConst_dependent != 0)
			{
				mvH.mConst_dependent = mvH.mH_dependent->mConst_dependent;
				mvH.mH_dependent = NULL;
			}

			if (mvH.mH_independent != NULL)
			{	
				*mvH.mH_independent = simplify( *mvH.mH_independent );
			}
	
			return ( simplify(mvH) );	
		}
	}

	else if (mvH.mH_dependent == NULL && mvH.mH_independent != NULL)
	{
		*mvH.mH_independent = simplify (*mvH.mH_independent);
		return(mvH);
	}

	else if (mvH.mH_dependent != NULL && mvH.mH_independent == NULL)
	{
		*mvH.mH_dependent = simplify (*mvH.mH_dependent);
		return(mvH);
	}
	
	else if (mvH.mH_dependent != NULL && mvH.mH_independent != NULL)
	{
		*mvH.mH_dependent   = simplify(*mvH.mH_dependent);
		*mvH.mH_independent = simplify (*mvH.mH_independent);
		return(mvH);
	}
	

	
	return(mvH);
}



template<typename PolynomialType>
Interval<PolynomialType> evaluate( MultivariateHorner<PolynomialType>& mvH, const std::map<Variable, Interval<PolynomialType>>&, Variable* previousVariable)
{
	unsigned exponentOfpreviousVariable;
	Interval<PolynomialType> result(1);
	


	return result;
}


}//namespace carl
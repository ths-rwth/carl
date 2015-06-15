 /**
 * @file	MultivariateHorner.tpp
 * @author	Lukas Netz
 *
 */


 #ifndef HORNER_Minimize_arithmetic_operations
 #define HORNER_Minimize_arithmetic_operations 1
 #endif

namespace carl
{
	template< typename PolynomialType > 
	MultivariateHorner< PolynomialType >::MultivariateHorner (const PolynomialType& inPut){

		std::set<Variable> allVariablesinPolynome;
		inPut.gatherVariables(allVariablesinPolynome);

		if (HORNER_Minimize_arithmetic_operations)
		{
			//Detecting amounts of Variables in Monomials
			std::set<Variable>::iterator it;
			std::set<Variable>::iterator bestVariable;
			unsigned int bestCounter = 0;

			for (it = allVariablesinPolynome.begin(); it != allVariablesinPolynome.end(); it++)
			{	
				unsigned int counter = 0;
				typename PolynomialType::TermsType::const_iterator pIt;
				for (pIt = inPut.begin(); pIt != inPut.end(); pIt++)
				{
					//checking if piT has it...
					if (pIt->has(*it))
					{
						counter++;
					}
				}
				
				//saving most promising Variable for later 
				if ( counter >= bestCounter ) {
					bestCounter = counter;
					bestVariable = it;
				}
			}			

		}//minimize arithmatic operations
	}
	

}//namespace carl
 /**
 * @file	MultivariateHorner.tpp
 * @author	Lukas Netz
 *
 */

//#define DEBUG_HORNER

namespace carl
{
	//Constructor
	template< typename PolynomialType, class strategy >
	MultivariateHorner< PolynomialType, strategy>::MultivariateHorner (const PolynomialType& inPut) {
	#ifdef DEBUG_HORNER
		std::cout << __func__ << " (GreedyI constr) P: " << inPut << std::endl;
	#endif

	size_t arithmeticOperationsCounter = 0;
	if (strategy::use_arithmeticOperationsCounter)
 	{
 		//Sum of monomials
 		arithmeticOperationsCounter = inPut.nrTerms() - 1;

 		typename PolynomialType::TermsType::const_iterator polynomialIt;
		for (polynomialIt = inPut.begin(); polynomialIt != inPut.end(); polynomialIt++)
		{
			arithmeticOperationsCounter += polynomialIt->getNrVariables() - 1;
			arithmeticOperationsCounter += polynomialIt->tdeg() - polynomialIt->getNrVariables();
			if (polynomialIt->coeff() > 1) arithmeticOperationsCounter++;
			if (polynomialIt->isConstant()) arithmeticOperationsCounter++;

		}
 	}


	//static_assert(!(strategy::variableSelectionHeurisics == variableSelectionHeurisics::GREEDY_II)&&!(strategy::variableSelectionHeurisics == variableSelectionHeurisics::GREEDY_IIs), "Strategy requires Interval map");

	if (strategy::selectionType == variableSelectionHeurisics::GREEDY_II || strategy::selectionType == variableSelectionHeurisics::GREEDY_IIs){
		auto allVariablesinPolynome = carl::variables(inPut);
		carl::carlVariables::iterator variableIt;

		for (variableIt = allVariablesinPolynome.begin(); variableIt != allVariablesinPolynome.end(); variableIt++)
		{
			typename std::map<Variable, Interval<double>>::const_iterator const_iterator_map = mMap.find(*variableIt);
			if ( const_iterator_map == mMap.end() )
			{
				const_iterator_map = mMap.emplace(*variableIt, Interval<double>((-1) * strategy::targetDiameter, strategy::targetDiameter)).first;
			}
		}
	}

	int arithmeticOperationsReductionCounter = 0;

	//Create Horner Scheme Recursivly
	MultivariateHorner< PolynomialType, strategy > root ( std::move(inPut), mMap, arithmeticOperationsReductionCounter );

 	//Part after recursion
 	if (strategy::selectionType == variableSelectionHeurisics::GREEDY_Is || strategy::selectionType == variableSelectionHeurisics::GREEDY_IIs)
 	{
 		auto root_ptr =std::make_shared<MultivariateHorner< PolynomialType, strategy > >(root);
 		root_ptr = simplify( root_ptr );
 		root = *root_ptr;
 	}

 	//Apply all changes
 	*this = root;

 	if (strategy::use_arithmeticOperationsCounter)
 	{
 		std::cout <<"Total AO: "<< arithmeticOperationsCounter << " rAO: " << arithmeticOperationsReductionCounter <<  " inPut: " << inPut << "  Output: " << root << std::endl;

 		if (arithmeticOperationsReductionCounter > 0)
 		{
 			std::cout << "\n\n\n\n\n                                           IT WORKED !!!  \n\n\n\n\n" << std::endl;
 		}
 	}
	}


	//Constructor for Greedy II and Greedy I
	template< typename PolynomialType, typename strategy >
	MultivariateHorner< PolynomialType, strategy>::MultivariateHorner (const PolynomialType& inPut, const std::map<Variable, Interval<double>>& map) {
	#ifdef DEBUG_HORNER
		std::cout << __func__ << " (GreedyII constr) P: " << inPut << std::endl;
	#endif

	//Create Horner Scheme Recursivly
	MultivariateHorner< PolynomialType, strategy > root (std::move(inPut), true, map);

 	//Part after recursion
 	if (strategy::selectionType == variableSelectionHeurisics::GREEDY_Is || strategy::selectionType == variableSelectionHeurisics::GREEDY_IIs)
 	{
 		auto root_ptr =std::make_shared<MultivariateHorner< PolynomialType, strategy > >(root);
 		root_ptr = simplify( root_ptr );
 		root = *root_ptr;
 	}

 	//Apply all changes
 	*this = root;
	}


	//Constructor for Greedy I/II creates recursive Datastruckture
	template< typename PolynomialType, typename strategy >
	MultivariateHorner< PolynomialType, strategy>::MultivariateHorner (const PolynomialType& inPut, const std::map<Variable, Interval<double>>& map, int& counter)
	{
	int s = strategy::selectionType;

		carl::carlVariables::iterator variableIt;
		carl::carlVariables::iterator selectedVariable;
		auto allVariablesinPolynome = carl::variables(inPut);

		Interval<double> currentInterval(0);
		CoeffType delta = constant_zero<CoeffType>::get();
		CoeffType bestDelta = constant_zero<CoeffType>::get();

		unsigned int monomials_containingChoosenVar = 0;

		if (!allVariablesinPolynome.empty())
		{
			//Detecting amounts of Variables in Monomials
			for (variableIt = allVariablesinPolynome.begin(); variableIt != allVariablesinPolynome.end(); variableIt++)
			{
				if (s == variableSelectionHeurisics::GREEDY_I || s == variableSelectionHeurisics::GREEDY_Is)
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

				if (s == variableSelectionHeurisics::GREEDY_II || s == variableSelectionHeurisics::GREEDY_IIs)
				{
					typename PolynomialType::TermsType::const_iterator polynomialIt;
					CoeffType accMonomEval = constant_zero<CoeffType>::get();
					CoeffType accMonomDivEval = constant_zero<CoeffType>::get();

					for (polynomialIt = inPut.begin(); polynomialIt != inPut.end(); polynomialIt++)
					{
						if (polynomialIt->has(*variableIt))
						{
							Term< typename PolynomialType::CoeffType > currentTerm = *polynomialIt;
							Term< typename PolynomialType::CoeffType > currentTerm_div = *polynomialIt;

							currentTerm_div.divide(*variableIt, currentTerm_div);

							currentInterval = IntervalEvaluation::evaluate( currentTerm_div, map );

							accMonomEval += carl::rationalize<CoeffType>(IntervalEvaluation::evaluate( currentTerm, map ).diameter());
							accMonomDivEval += carl::rationalize<CoeffType>(currentInterval.diameter());

						}
					}
					accMonomDivEval *= carl::rationalize<CoeffType>(map.find(*variableIt)->second.diameter());
					delta = accMonomDivEval - accMonomEval;

					if (delta > bestDelta )
					{
						#ifdef DEBUG_HORNER
							std::cout << "update Delta...";
						#endif

						bestDelta = delta;
						selectedVariable = variableIt;
					}
					//std::cout << *variableIt << " D: "<< delta  << " BD: "<< bestDelta << "\n" << std::endl;
	 			}
			}

			//Setting the choosen Variable for the current Hornerscheme iterartion

			if ((*selectedVariable == NULL && (strategy::selectionType == variableSelectionHeurisics::GREEDY_Is || strategy::selectionType == variableSelectionHeurisics::GREEDY_I))
				|| (bestDelta == constant_zero<CoeffType>::get() && (strategy::selectionType == variableSelectionHeurisics::GREEDY_IIs || strategy::selectionType == variableSelectionHeurisics::GREEDY_II)) )
			{
				selectedVariable = allVariablesinPolynome.begin();
			}
			this->setVariable(*selectedVariable);

			#ifdef DEBUG_HORNER
			std::cout << __func__ << "    Polynome: " << inPut << std::endl;
			std::cout << "Choosen Var: " << *selectedVariable << std::endl;
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
					counter++;
					h_dependentPart.addTerm( tempTerm );
				}
				else
				{
					h_independentPart.addTerm( *polynomialIt );
				}
			}

			counter = counter - 1;

			//If Dependent Polynome contains Variables - continue with recursive Horner
			if ( !h_dependentPart.isNumber() )
			{
				#ifdef DEBUG_HORNER
					std::cout << __func__ << "    DEP->new Horner " << h_dependentPart << std::endl;
				#endif
				std::shared_ptr<MultivariateHorner<PolynomialType, strategy>> new_dependent (new MultivariateHorner< PolynomialType, strategy >(std::move(h_dependentPart), map, counter));
				setDependent(new_dependent);
				mConst_dependent = constant_zero<CoeffType>::get();
			}

			//Dependent Polynome is a Constant (Number) - save to memberVar
			else
			{
				removeDependent();
				mConst_dependent = h_dependentPart.constantPart();
			}

			//If independent Polynome contains Variables - continue with recursive Horner
			if ( !h_independentPart.isNumber() )
			{
				#ifdef DEBUG_HORNER
					std::cout << __func__ << "    INDEP->new Horner " << h_independentPart << std::endl;
				#endif
				std::shared_ptr<MultivariateHorner<PolynomialType, strategy>> new_independent ( new MultivariateHorner< PolynomialType, strategy >(std::move(h_independentPart) ,map, counter));
				setIndependent(new_independent);
				mConst_independent = constant_zero<CoeffType>::get();
			}
			//Independent Polynome is a Constant (Number) - save to memberVar
			else
			{
				removeIndepenent();
				mConst_independent =  h_independentPart.constantPart();
			}
		}

		//If there are no Variables in the polynome
		else
		{
			#ifdef DEBUG_HORNER
				std::cout << __func__ << " [CONSTANT TERM w/o Variables]  " << inPut << std::endl;
			#endif
			removeIndepenent();
			removeDependent();
			mConst_independent = inPut.constantPart();
			this->setVariable( Variable::NO_VARIABLE );
		}
	}



/**
	 * Streaming operator for multivariate HornerSchemes.
	 * @param os Output stream.
	 * @param rhs HornerScheme.
	 * @return `os`.
	 */
template< typename PolynomialType, typename strategy >
std::ostream& operator<<(std::ostream& os,const MultivariateHorner<PolynomialType, strategy>& mvH)
{
	if (mvH.getDependent() && mvH.getIndependent())
	{
		if (mvH.getExponent() != 1)
		{
			return (os << mvH.getVariable() <<"^"<< mvH.getExponent() << " * (" << *mvH.getDependent() << ") + " << *mvH.getIndependent());
		}
		else
		{
			return (os << mvH.getVariable() << " * (" << *mvH.getDependent() << ") + " << *mvH.getIndependent());
		}

	}
	else if (mvH.getDependent() && !mvH.getIndependent())
	{
		if (mvH.getIndepConstant() == 0)
		{
			if (mvH.getExponent() != 1)
			{
				return (os << mvH.getVariable() <<"^"<< mvH.getExponent() << " * (" << *mvH.getDependent() << ")" );
			}
			else
			{
				return (os << mvH.getVariable() << " * (" << *mvH.getDependent() << ")" );
			}
		}
		else
		{
			if (mvH.getExponent() != 1)
			{
				return (os << mvH.getVariable() <<"^"<< mvH.getExponent() << " * (" << *mvH.getDependent() << ") + " << mvH.getIndepConstant());
			}
			else
			{
				return (os << mvH.getVariable() << " * (" << *mvH.getDependent() << ") + " << mvH.getIndepConstant());
			}
		}
	}
	else if (!mvH.getDependent() && mvH.getIndependent())
	{
		if (mvH.getDepConstant() == 1)
		{
			if (mvH.getExponent() != 1)
			{
				return (os << mvH.getVariable() <<"^"<< mvH.getExponent() << " + " << *mvH.getIndependent() );
			}
			else
			{
				return (os << mvH.getVariable() << " + " << *mvH.getIndependent() );
			}
		}
		else
		{
			if (mvH.getExponent() != 1)
			{
				return (os << mvH.getDepConstant() << mvH.getVariable() <<"^"<< mvH.getExponent() << " + " << *mvH.getIndependent() );
			}
			else
			{
				return (os << mvH.getDepConstant() << mvH.getVariable() <<" + " << *mvH.getIndependent() );
			}
		}
	}
	else //if (mvH.getDependent() == NULL && mvH.getIndependent() == NULL)
	{
		if (mvH.getVariable() == Variable::NO_VARIABLE)
		{
			return (os << mvH.getIndepConstant());
		}

		else if (mvH.getIndepConstant() == 0)
		{
			if (mvH.getDepConstant() == 1)
			{
				if (mvH.getExponent() != 1)
				{
					return (os << mvH.getVariable() <<"^"<< mvH.getExponent() );
				}
				else
				{
					return (os << mvH.getVariable());
				}

			}
			else
			{
				if (mvH.getExponent() != 1)
				{
					return (os << mvH.getDepConstant() << mvH.getVariable() <<"^"<< mvH.getExponent());
				}
				else
				{
					return (os << mvH.getDepConstant() << mvH.getVariable());
				}
			}
		}
		else
		{
			if (mvH.getExponent() != 1)
			{
				return (os << mvH.getDepConstant() << mvH.getVariable() <<"^"<< mvH.getExponent() << " + " << mvH.getIndepConstant());
			}
			else
			{
				return (os << mvH.getDepConstant() << mvH.getVariable() <<" + " << mvH.getIndepConstant());
			}

		}
	}
}

template<typename PolynomialType, typename strategy>
std::shared_ptr<MultivariateHorner<PolynomialType, strategy>> simplify( std::shared_ptr<MultivariateHorner<PolynomialType, strategy>> mvH)
{

	#ifdef DEBUG_HORNER
		std::cout << __func__ << " Polynome: " << *mvH << std::endl;
	#endif

	if (mvH->getDependent() && (mvH->getDependent()->getDependent() || mvH->getDependent()->getDepConstant() != 0) && !mvH->getDependent()->getIndependent()  && mvH->getDependent()->getIndepConstant() == 0 )
	{

		if (mvH->getVariable() == mvH->getDependent()->getVariable())
		{
			mvH->setExponent (mvH->getExponent() + mvH->getDependent()->getExponent()) ;

			if (mvH->getDependent()->getDependent())
			{
				mvH->setDependent( simplify( mvH->getDependent()->getDependent()) );
			}
			else if (mvH->getDependent()->getDepConstant() != 0)
			{
				mvH->setDepConstant(mvH->getDependent()->getDepConstant() );
				mvH->removeDependent();
			}

			if (mvH->getIndependent())
			{
				mvH->setIndependent( simplify( mvH->getIndependent() ));
			}

			return ( simplify(mvH));
		}
	}

	else if (!mvH->getDependent() && mvH->getIndependent())
	{
		mvH->setIndependent(simplify ( mvH->getIndependent() ));
		mvH->removeDependent();
		return mvH;
	}

	else if (mvH->getDependent()  && !mvH->getIndependent() )
	{
		mvH->setDependent( simplify ( mvH->getDependent()));
		mvH->removeIndepenent();
		return mvH;
	}

	else if (mvH->getDependent() && mvH->getIndependent() )
	{
		mvH->setDependent( simplify( mvH->getDependent()));
		mvH->setIndependent( simplify ( mvH->getIndependent()));
		return mvH;
	}

	return(mvH);
}

}//namespace carl

/** 
 * @file:   stringparser.h
 * @author: Sebastian Junges
 *
 * @since March 17, 2014
 */


#pragma once

#include <cassert>
#include <exception>
#include <iostream>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "../core/VariablePool.h"
#include "../core/Term.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/logging.h"


namespace carl
{
	
	
	//enum class CoeffType { Integer, Rational };
	//enum class NumbLib { CLN, GMPPlusPlus };
	class InvalidInputStringException : public std::runtime_error
	{
		typedef const char* cstring;
		
		/// Substring where the problem is.
		const std::string mSubstring;
		/// Inputstring
		std::string mInputString;
	public:
		InvalidInputStringException(const std::string& msg, const std::string& substring, const std::string& inputString = "") : std::runtime_error(msg),
		mSubstring(substring), mInputString(inputString)
		{
			
		}
		
		void setInputString(const std::string& inputString) 
		{
			mInputString = inputString;
		}
		
		virtual cstring what() const throw() override
		{
			std::stringstream strstr;
			strstr << std::runtime_error::what() << " at " << mSubstring << " in " << mInputString;
			return strstr.str().c_str();
		}		
	};
	
	
	
	
	
	class StringParser
	{	
	private:
		VariablePool& mVPool = VariablePool::getInstance();		
	protected:
		bool mSingleSymbVariables;
		bool mImplicitMultiplicationMode; 
		bool mSumOfTermsForm;
		std::map<std::string, Variable> mVars;
		
	public:
		void setVariables(std::list<std::string> variables)
		{
			mSingleSymbVariables = true;
			variables.sort();
			variables.unique();
			for(const std::string& v : variables)
			{
				if(v.length() > 1)
				{
					mSingleSymbVariables = false;
					mImplicitMultiplicationMode = false;
				}
				mVars.emplace(v, mVPool.getFreshVariable(v));
			}
		}
			
		bool setImplicitMultiplicationMode(bool to)
		{
			if(!mSingleSymbVariables)
			{
				mImplicitMultiplicationMode = to;
				return true;
			}
			else
			{
				return false;
			}
		}
		
		/**
		 * In SumOfTermsForm, input strings are expected to be of the form "c_1 * m_1 + ... + c_n * m_n",
		 * where c_i are coefficients and m_i are monomials.
         * @param to value to set
         * @return 
         */
		void setSumOfTermsForm(bool to)
		{
			LOG_ASSERT(to, "Extended parser not supported");
			mSumOfTermsForm = to;
		}
		
		template<typename C>
		MultivariatePolynomial<C, typename MultivariatePolynomial<C>::OrderedBy, typename MultivariatePolynomial<C>::Policy> parseMultivariatePolynomial(const std::string& inputString) const
		{
			MultivariatePolynomial<C, typename MultivariatePolynomial<C>::OrderedBy, typename MultivariatePolynomial<C>::Policy> result;
			std::vector<std::string> termStrings;
			if(mSumOfTermsForm)
			{
				boost::split(termStrings,inputString,boost::is_any_of("+"));
				
				for(std::string& tStr : termStrings)
				{
					boost::trim(tStr);
					try 
					{
						result += parseTerm<C>(tStr);
					}
					catch(InvalidInputStringException& e) 
					{
						e.setInputString(inputString);
						throw e;
					}
				}
			}
			else
			{
				LOG_NOTIMPLEMENTED();
			}
			return result;
		}
		
		template<typename C>
		Term<C> parseTerm(const std::string& inputStr) const
		{
			C coeff = 1;
			std::vector<VarExpPair> varExpPairs;
			if(!mImplicitMultiplicationMode)
			{
				std::vector<std::string> varExpPairStrings;
				boost::split(varExpPairStrings, inputStr, boost::is_any_of("*"));
				
				
				for(const std::string& veStr : varExpPairStrings)
				{
					std::vector<std::string> varAndExp;
					boost::split(varAndExp, veStr, boost::is_any_of("^"));
					if(varAndExp.size() > 2)
					{
						throw InvalidInputStringException("Two carats in one variable-exponent pair", veStr, "");
					}
					
					if(varAndExp.size() == 1)
					{
						auto it = mVars.find(veStr);
						if(it != mVars.end())
						{
							varExpPairs.emplace_back(it->second, 1);
						}
						else
						{
							coeff *= 1;//constructCoefficient(veStr);
						}
					}
					else
					{
						assert(varAndExp.size() == 2);
						auto it = mVars.find(varAndExp.front());
						if(it != mVars.end())
						{
							try
							{
								unsigned exp = boost::lexical_cast<unsigned>(varAndExp.back());
								varExpPairs.emplace_back(it->second, exp);
							}
							catch(const boost::bad_lexical_cast& e)
							{
								throw InvalidInputStringException("Exponent is not a number", veStr);
							}
						}
						else
						{
							throw InvalidInputStringException("Unknown variable", varAndExp.front());
						}
					}
				}
			}
			else
			{
				LOG_ASSERT(mSingleSymbVariables, "The implicit mode can only be set with single symbol variables");				
			}
			
			std::sort(varExpPairs.begin(), varExpPairs.end(), CompareByVariable());
			size_t nrVariables = varExpPairs.size();
			std::unique(varExpPairs.begin(), varExpPairs.end(), CompareByVariable());
			if(nrVariables != varExpPairs.size())
			{
				throw InvalidInputStringException("Variable occurs twice", inputStr);
			}
			Term<C> result(coeff, Monomial(varExpPairs));
			return result;
		}
		
	};
}
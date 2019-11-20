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
#include "../core/logging.h"

#include "../core/VariablePool.h"
#include "../core/Term.h"
#include "../core/MultivariatePolynomial.h"

#include <carl-extpolys/RationalFunction.h>


namespace carl
{
	
	
	//enum class CoeffType { Integer, Rational };
	//enum class NumbLib { CLN, GMPPlusPlus };
	class InvalidInputStringException : public std::runtime_error
	{
		using cstring = const char*;
		
		/// Substring where the problem is.
		const std::string mSubstring;
		/// Error message
		std::string mErrorString;
	public:
		InvalidInputStringException(const std::string& msg, std::string substring, const std::string& inputString = ""):
			std::runtime_error(msg), mSubstring(std::move(substring))
		{
			setInputString(inputString);
		}
		
		void setInputString(const std::string& inputString) 
		{
			std::stringstream strstr;
			strstr << std::runtime_error::what() << " at " << mSubstring << " in " << inputString;
			mErrorString = strstr.str();
		}
		
#ifdef __VS
		virtual cstring what() const override
#else
		virtual cstring what() const noexcept override
#endif
		{
			return mErrorString.c_str();
		}		
	};
	
	
	
	
	
	class StringParser
	{
	protected:
		bool mSingleSymbVariables;
		bool mImplicitMultiplicationMode = false; 
		bool mSumOfTermsForm = true;
		std::map<std::string, Variable> mVars;
		
	public:
		StringParser():
			mSingleSymbVariables(false),
			mImplicitMultiplicationMode(false),
			mSumOfTermsForm(true),
			mVars()
		{
		}

		const std::map<std::string, Variable>& variables() const {
			return mVars;
		}
		
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
				mVars.emplace(v, freshRealVariable(v));
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
			CARL_LOG_ASSERT("carl.stringparser", to, "Extended parser not supported");
			mSumOfTermsForm = to;
		}
		
		template<typename C, typename O = typename MultivariatePolynomial<C>::OrderedBy, typename P = typename MultivariatePolynomial<C>::Policy>
		RationalFunction<MultivariatePolynomial<C,O,P>> parseRationalFunction(const std::string& inputString) const
		{
			std::vector<std::string> nomAndDenom;
			boost::split(nomAndDenom, inputString, boost::is_any_of("/"));
			assert(!nomAndDenom.empty());
			if(nomAndDenom.size() > 2)
			{
				throw InvalidInputStringException("Multiple divisions, unclear which is division", inputString , inputString);
			}
			else if(nomAndDenom.size() == 2)
			{
				auto nom = parseMultivariatePolynomial<C>(nomAndDenom.front());
				auto denom = parseMultivariatePolynomial<C>(nomAndDenom.back());
				if(denom.isZero())
				{
					throw InvalidInputStringException("Denominator is zero", nomAndDenom.back() , inputString);
				}
				return {nom, denom};
			}
			else
			{
				assert(nomAndDenom.size() == 1);
				auto pol = parseMultivariatePolynomial<C>(nomAndDenom.front());
				return {pol};
			}
		}
		
		template<typename C, typename O = typename MultivariatePolynomial<C>::OrderedBy, typename P = typename MultivariatePolynomial<C>::Policy>
		MultivariatePolynomial<C, O, P> parseMultivariatePolynomial(const std::string& inputString) const
		{
			MultivariatePolynomial<C, O, P> result;
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
						throw;
					}
				}
			}
			else
			{
				CARL_LOG_NOTIMPLEMENTED();
			}
			return result;
		}
		
		template<typename C>
		Term<C> parseTerm(const std::string& inputStr) const
		{
			C coeff = 1;
			std::vector<std::pair<Variable, exponent>> varExpPairs;
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
							coeff *= constructCoefficient<C>(veStr);
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
							catch(const boost::bad_lexical_cast&)
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
				CARL_LOG_ASSERT("carl.stringparser", mSingleSymbVariables, "The implicit mode can only be set with single symbol variables");
			}
			
			std::sort(varExpPairs.begin(), varExpPairs.end(), [](const std::pair<Variable, exponent>& p1, const std::pair<Variable, exponent>& p2){ return p1.first < p2.first; });
			size_t nrVariables = varExpPairs.size();
			std::unique(varExpPairs.begin(), varExpPairs.end());
			if(nrVariables != varExpPairs.size())
			{
				throw InvalidInputStringException("Variable occurs twice", inputStr);
			}
			if(varExpPairs.empty())
			{
				return Term<C>(coeff);
			}
			else
			{
                std::shared_ptr<const Monomial> result = createMonomial( std::move(varExpPairs) );
				return Term<C>(coeff, result);
			}
		
		}
		
	protected:
		template<typename C>
		C constructCoefficient(const std::string& inputString) const
		{
            std::string inputStringCopy = inputString;
            assert(!inputStringCopy.empty());
            if( inputStringCopy[0] == '(' )
            {
                assert(inputStringCopy.back() == ')');
                inputStringCopy = inputStringCopy.substr( 1, inputStringCopy.size()-2 );
            }
			try
			{
				return parse<C>(inputStringCopy);
			}
			catch(std::exception&)
			{
				throw InvalidInputStringException("Could not build coefficient", inputStringCopy);
			}
			
		}
		
	};
	
}

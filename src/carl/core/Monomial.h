/**
 * @file Monomial.h 
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once
#include <algorithm>
#include <list>
#include <set>
#include <sstream>

#include "Variable.h"
#include "CompareResult.h"
#include "VarExpPair.h"
#include "VariablesInformation.h"
#include "logging.h"
#include "../numbers/numbers.h"

namespace carl
{   

	template<typename Coefficient>
	class Term;

	/**
	 * The general-purpose monomials. Notice that we aim to keep this object as small as possbible,
	 * while also limiting the use of expensive language features such as RTTI, exceptions and even
	 * polymorphism.
	 * @ingroup multirp
	 */
	class Monomial
	{
	private:
		/**
		 * Set if we want to use binary search instead of linear search.
		 */
		static const bool PREFERBINARYSEARCH = false;

	protected:
		/// A vector of variable-exponent vars (v_i^e_i) with nonzero exponents. 
		std::vector<VarExpPair> mExponents;
		/// Some applications performance depends on getting the degree of monomials very fast
		exponent mTotalDegree = 0;

		typedef std::vector<VarExpPair>::iterator exponents_it;
		typedef std::vector<VarExpPair>::const_iterator exponents_cIt;

		/**
		 */
		Monomial() = default;
	public:
		/**
		 * Generate a monomial from a variable and an exponent.
		 * @param v The variable.
		 * @param e The exponent.
		 */
		Monomial(Variable::Arg v, exponent e = 1) :
			mExponents(1, VarExpPair(v,e)),
			mTotalDegree(e)
		{
			assert(isConsistent());
		}

		/**
		 * Copy constructor.
		 * @param rhs Monomial to copy.
		 */
		Monomial(const Monomial& rhs) :
			mExponents(rhs.mExponents),
			mTotalDegree(rhs.mTotalDegree)
		{
			assert(isConsistent());
		}

		/**
		 * Generate a monomial from a vector of variable-exponent pairs and a total degree.
		 * @param exponents The variables and their exponents.
		 * @param totalDegree The total degree of the monomial to generate.
		 */
		Monomial(std::vector<VarExpPair>&& exponents, exponent totalDegree) :
			mExponents(exponents),
			mTotalDegree(totalDegree)
		{
			assert(isConsistent());
		}

		Monomial& operator=(const Monomial& rhs)
		{
			// Check for self-assignment.
			if(this == &rhs) return *this;
			mExponents = rhs.mExponents;
			mTotalDegree = rhs.mTotalDegree;
			return *this;
		}
		/**
		 * Gives the total degree, i.e. the sum of all exponents.
		 * @return Total degree.
		 */
		exponent tdeg() const
		{
			return mTotalDegree;
		}
		/**
		 * Checks whether the monomial is a constant.
		 * @return If monomial is constant.
		 */
		bool isConstant() const
		{
			return mTotalDegree == 0;
		}
		/**
		 * Checks whether the monomial has exactly degree one.
		 * @return If monomial is linear.
		 */
		bool isLinear() const
		{
			return mTotalDegree == 1;
		}
		
		/**
		 * Checks whether the monomial has at most degree one.
		 * @return If monomial is linear or constant.
		 */
		bool isAtMostLinear() const
		{
			return mTotalDegree <= 1;
		}
		
		/**
		 * Checks whether the monomial is a square, i.e. whether all exponents are even.
		 * @return If monomial is a square.
		 */
		bool isSquare() const
		{
			if(mTotalDegree % 2 == 1) return false;
			for(const VarExpPair& ve : mExponents)
			{
				if(ve.exp % 2 == 1) return false;
			}
			return true;
		}
		
		/**
		 * Returns the number of variables that occur in the monomial.
		 * @return Number of variables.
		 */
		size_t nrVariables() const
		{
			return mExponents.size();
		}

		/**
		 * Retrieves the single variable of the monomial.
		 * Asserts that there is in fact only a single variable.
		 * @return Variable.
		 */
		Variable::Arg getSingleVariable() const
		{
			assert(mExponents.size() == 1);
			return mExponents.front().var;
		}
		
		/**
		 * Checks that there is no other variable than the given one.
		 * @param v Variable.
		 * @return If there is only v.
		 */
		bool hasNoOtherVariable(Variable::Arg v) const
		{
			if(mExponents.size() == 1)
			{
				if(mExponents.front().var == v) return true;
				return false;
			}
			else if(mExponents.size() == 0) 
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		
		/**
		 * Retrieves the given VarExpPair.
		 * @param index Index.
		 * @return VarExpPair.
		 */
		const VarExpPair& operator[](unsigned index) const
		{
			assert(index < mExponents.size());
			return mExponents[index];
		}
		/**
		 * Retrieves the exponent of the given variable.
		 * @param v Variable.
		 * @return Exponent of v.
		 */
		exponent exponentOfVariable(Variable::Arg v) const
		{
			exponents_cIt it = std::find(mExponents.cbegin(), mExponents.cend(), v);
			if(it == mExponents.cend()) {
				return 0;
			} else {
				return it->exp;
			}
		}
		
		/**
		 * TODO: write code if binary search is preferred.
		 * @param v The variable to check for its occurrence.
		 * @return true, if the variable occurs in this term.
		 */
		bool has(Variable::Arg v) const
		{
			return (std::find(mExponents.cbegin(), mExponents.cend(), v) != mExponents.cend());
		}

		exponent highestExponent(Variable::Arg) const
		{
			LOG_NOTIMPLEMENTED();
			return 0;
		}
		
		/**
		 * For a monomial m = Prod( x_i^e_i ) * v^e, divides m by v^e
		 * @return nullptr if result is 1, otherwise m/v^e.
		 */
		Monomial* dropVariable(Variable::Arg v) const
		{
			if (!this->has(v)) {
				return new Monomial(*this);
			}
			Monomial* m = new Monomial();
			m->mExponents.reserve(mExponents.size()-1);
			m->mTotalDegree = mTotalDegree;
			
			for(const VarExpPair& ve : mExponents)
			{
				if(ve.var == v)
				{
					m->mTotalDegree -= ve.exp;
				}
				else
				{
					m->mExponents.emplace_back(ve);
				}
			}
			if(m->mTotalDegree == 0) 
			{
				delete m;
				return nullptr;
			}
			return m;
		}

		Monomial* dividedBy(Variable::Arg v) const
		{
			 // Linear implementation, as we expect very small monomials.
			exponents_cIt it;
			if((it = std::find(mExponents.cbegin(), mExponents.cend(), v)) == mExponents.cend())
			{
				return nullptr;
			}
			else
			{
				Monomial* m = new Monomial();
				// If the exponent is one, the variable does not occur in the new monomial.
				if(it->exp == 1)
				{
					if(it != mExponents.begin())
					{
						m->mExponents.assign(mExponents.begin(), it);
					}
					m->mExponents.insert(m->mExponents.end(), it+1,mExponents.end());
				}
				// We have to decrease the exponent of the variable by one.
				else
				{
					m->mExponents.assign(mExponents.begin(), mExponents.end());
					m->mExponents[(unsigned)(it - mExponents.begin())].exp -= 1;
				}
				m->mTotalDegree = mTotalDegree - 1;
				return m;
			}
		}

		
		bool dividableBy(const Monomial& m) const
		{
			assert(isConsistent());
			if(m.mTotalDegree > mTotalDegree) return false;
			if(m.nrVariables() > nrVariables()) return false;
			// Linear, as we expect small monomials.
			exponents_cIt itright = m.mExponents.begin();
			for(exponents_cIt itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
			{
				// Done with division
				if(itright == m.mExponents.end())
				{
					return true;
				}
				// Variable is present in both monomials.
				if(itleft->var == itright->var)
				{
					if(itright->exp > itleft->exp)
					{
						// Underflow, itright->exp was larger than itleft->exp.
						return false;
					}
					itright++;
				}
				// Variable is not present in lhs, division fails.
				else if(itleft->var > itright->var) 
				{
					return false;
				}
				else
				{
					assert(itright->var > itleft->var);
				}
			}
			// If there remain variables in the m, it fails.
			if(itright != m.mExponents.end()) 
			{
				return false;
			}
			return true;
			
			
		}
		/**
		 * 
		 * @param m Recommended to be non-constant as this yields unnecessary copying.
		 * @return 
		 */
		Monomial* dividedBy(const Monomial& m) const
		{
			LOG_FUNC("carl.core.monomial", *this << ", " << m);
			if(m.mTotalDegree > mTotalDegree || m.mExponents.size() > mExponents.size())
			{
				// Division will fail.
				LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
				return nullptr;
			}
			Monomial* result = new Monomial();
			result->mTotalDegree =  mTotalDegree - m.mTotalDegree;

			// Linear, as we expect small monomials.
			exponents_cIt itright = m.mExponents.begin();
			for(exponents_cIt itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
			{
				// Done with division
				if(itright == m.mExponents.end())
				{
					// Insert remaining part
					result->mExponents.insert(result->mExponents.end(), itleft, mExponents.end());
					assert(result->isConsistent());
					LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
					return result;
				}
				// Variable is present in both monomials.
				if(itleft->var == itright->var)
				{
					exponent newExp = itleft->exp - itright->exp;
					if(newExp > itleft->exp)
					{
						// Underflow, itright->exp was larger than itleft->exp.
						delete result;
						LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
						return nullptr;
					}
					else if(newExp > 0)
					{
						result->mExponents.push_back(VarExpPair(itleft->var, newExp));
					}
					itright++;
				}
				// Variable is not present in lhs, division fails.
				else if(itleft->var > itright->var) 
				{
					delete result;
					LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
					return nullptr;
				}
				else
				{
					assert(itright->var > itleft->var);
					result->mExponents.push_back(*itleft);
				}
			}
			// If there remain variables in the m, it fails.
			if(itright != m.mExponents.end())
			{
				delete result;
				LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
				return nullptr;
			}
			assert(result->isConsistent());
			LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
			return result;
			
		}
		
		/**
		 * 
		 * @param m
		 * @return 
		 */
		Monomial* calcLcmAndDivideBy(const Monomial& m) const
		{
			Monomial* result = new Monomial();
			result->mTotalDegree = mTotalDegree;
			// Linear, as we expect small monomials.
			exponents_cIt itright = m.mExponents.begin();
			for(exponents_cIt itleft = mExponents.begin(); itleft != mExponents.end();)
			{
				// Done with division
				if(itright == m.mExponents.end())
				{
					// Insert remaining part
					result->mExponents.insert(result->mExponents.end(), itleft, mExponents.end());
					return result;
				}
				// Variable is present in both monomials.
				if(itleft->var == itright->var)
				{
					exponent newExp = std::max(itleft->exp, itright->exp) - itright->exp;
					if(newExp != 0)
					{
						result->mExponents.push_back(VarExpPair(itleft->var, newExp));
						result->mTotalDegree -= itright->exp;
					}
					else
					{
						result->mTotalDegree -= itleft->exp;
					}
					++itright;
					++itleft;
				}
				// Variable is not present in lhs, dividing lcm yields variable will not occur in result
				
				else if(itleft->var > itright->var) 
				{
					++itright;
				}
				else
				{
					assert(itright->var > itleft->var);
					result->mExponents.push_back(*itleft);
					++itleft;
				}
			}
			assert(result->isConsistent());
			return result;
		}
		
		template<typename Coeff, bool gatherCoeff, typename CoeffType>
		void gatherVarInfo(const Variable& var, VariableInformation<gatherCoeff, CoeffType>& varinfo, const Coeff& coeffFromTerm) const
		{
			varinfo.collect(var, coeffFromTerm, *this);
		}
		
		template<typename Coeff, bool gatherCoeff, typename CoeffType>
		void gatherVarInfo(VariablesInformation<gatherCoeff, CoeffType>& varinfo, const Coeff& coeffFromTerm) const
		{
			for( const VarExpPair& ve : mExponents )
			{
				varinfo.variableInTerm(ve, coeffFromTerm, *this);
			}
		}
		
		/**
		 * For a monomial \\prod_i x_i^e_i with e_i != 0, returns \\prod_i x_i^1.
		 * @return 
		 */
		Monomial* seperablePart() const
		{
			Monomial* m = new Monomial();
			m->mExponents.reserve(mExponents.size());
			m->mTotalDegree = (unsigned)mExponents.size();
			for(const VarExpPair& ve : mExponents)
			{
				m->mExponents.emplace_back(ve.var, 1);
			}
			return m;
		}

		Monomial* pow(unsigned exp) const {
			if (exp == 0) {
				return nullptr;
			}
			Monomial* res = new Monomial(*this);
			unsigned expsum = 0;
			for (auto& it: res->mExponents) {
				it.exp = (exponent)(it.exp * exp);
				expsum += it.exp;
			}
			res->mTotalDegree = expsum;
			assert(res->isConsistent());
			return res;
		}
		
		/**
		 * Fill the set of variables with the variables from this monomial.
		 * @param variables
		 */
		void gatherVariables(std::set<Variable>& variables) const
		{
			for(const VarExpPair& ve : mExponents)
			{
				variables.insert(ve.var);
			}
		}
		
		template<typename Coefficient>
		Term<Coefficient>* derivative(Variable::Arg v) const;
		
		template<typename Coefficient, typename SubstitutionType>
		Term<Coefficient>* substitute(const std::map<Variable, SubstitutionType>& substitutions, Coefficient factor) const;
		template<typename Coefficient>
		Term<Coefficient>* substitute(const std::map<Variable, Term<Coefficient>>& substitutions, const Coefficient& factor) const;
		///////////////////////////
		// Orderings
		///////////////////////////

		static CompareResult compareLexical(const Monomial& lhs, const Monomial& rhs)
		{
			return lexicalCompare(lhs,rhs);
		}
		
		static CompareResult compareLexical(const Monomial& lhs, Variable::Arg rhs)
		{
			if(lhs.mExponents.front().var < rhs) return CompareResult::LESS;
			if(lhs.mExponents.front().var > rhs) return CompareResult::GREATER;
			if(lhs.mExponents.front().exp > 1) return CompareResult::GREATER;
			else return CompareResult::LESS;
		}


		static CompareResult compareGradedLexical(const Monomial& lhs, const Monomial& rhs)
		{
			
			if(lhs.mTotalDegree < rhs.mTotalDegree) return CompareResult::LESS;
			if(lhs.mTotalDegree > rhs.mTotalDegree) return CompareResult::GREATER;
			return lexicalCompare(lhs, rhs);
		}
		
		static CompareResult compareGradedLexical(const Monomial& lhs, Variable::Arg rhs)
		{
			if(lhs.mTotalDegree < 1) return CompareResult::LESS;
			if(lhs.mTotalDegree > 1) return CompareResult::GREATER;
			if(lhs.mExponents.front().var < rhs) return CompareResult::GREATER;
			if(lhs.mExponents.front().var > rhs) return CompareResult::LESS;
			else return CompareResult::EQUAL;
			
		}

		// 
		// Operators
		//
		
		friend bool operator==(const Monomial& lhs, const Monomial& rhs)
		{
			if (lhs.mTotalDegree != rhs.mTotalDegree) return false;
			return lhs.mExponents == rhs.mExponents;
		}

		friend bool operator==(const Monomial& lhs, Variable::Arg rhs)
		{
			if (lhs.mTotalDegree != 1) return false;
			if (lhs.mExponents[0].var == rhs) return true;
			return false;
		}

		friend bool operator==(const Variable& lhs, const Monomial& rhs)
		{
			return rhs == lhs;
		}

		friend bool operator!=(const Monomial& lhs, const Monomial& rhs)
		{
			return !(lhs == rhs);
		}

		friend bool operator!=(const Monomial& lhs, const Variable& rhs)
		{
			return !(lhs == rhs);
		}
		friend bool operator!=(const Variable& lhs, const Monomial& rhs)
		{
			return !(rhs == lhs);
		}

		friend bool operator<(const Monomial& lhs, const Monomial& rhs)
		{
			CompareResult cr = compareGradedLexical(lhs, rhs);
			return cr == CompareResult::LESS;
		}

		Monomial& operator*=(Variable::Arg v)
		{
			++mTotalDegree;
			// Linear, as we expect small monomials.
			for(exponents_it it = mExponents.begin(); it != mExponents.end(); ++it)
			{
				// Variable is present
				if(*it == v)
				{
					++(it->exp);
					return *this;
				}
				// Variable is not present, we have to insert v.
				if(*it > v)
				{
					mExponents.emplace(it,v);
					return *this;
				}
			}
			// Variable was not inserted, insert at end.
			mExponents.emplace_back(v);
			return *this;
		}

		Monomial& operator*=(const Monomial& rhs)
		{
			LOG_FUNC("carl.core.monomial", *this << ", " << rhs);
			if(rhs.mTotalDegree == 0) return *this;
			mTotalDegree += rhs.mTotalDegree;

			// Linear, as we expect small monomials.
			exponents_cIt itright = rhs.mExponents.begin();
			assert(itright != rhs.mExponents.end());
			for(exponents_it itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
			{
				// Everything is inserted.
				if(itright == rhs.mExponents.end())
				{
					LOGMSG_TRACE("carl.core.monomial", "Result: " << *this);
					return *this;
				}
				// Variable is present in both monomials.
				if(itleft->var == itright->var)
				{
					itleft->exp += itright->exp;
					++itright;
				}
				// Variable is not present in lhs, we have to insert var-exp pair from rhs.
				else if(itleft->var > itright->var) 
				{
					assert(itright != rhs.mExponents.end());
					itleft = mExponents.insert(itleft,*itright);
					++itright;
				
				}		
				else 
				{
					assert(itleft->var < itright->var);
				}

			}
			// Insert remainder of rhs.
			mExponents.insert(mExponents.end(), itright, rhs.mExponents.end());
			assert(isConsistent());
			LOGMSG_TRACE("carl.core.monomial", "Result: " << *this);
			return *this;
		}

		friend Monomial operator*(const Monomial& lhs, Variable::Arg rhs)
		{
			// Note that thisope implementation is not optimized yet!
			Monomial result(lhs);
			result *= rhs;
			return result;
		}

		friend Monomial operator*(Variable::Arg lhs, const Monomial& rhs)
		{
			return rhs * lhs;
		}
		
		friend Monomial operator*(Variable::Arg lhs, Variable::Arg rhs);

		friend Monomial operator*(const Monomial& lhs, const Monomial& rhs )
		{
			// Note that this implementation is not optimized yet!
			Monomial result(lhs);
			result *= rhs;
			return result;
		}
		
		std::string toString(bool infix = true, bool friendlyVarNames = true) const
		{
			if(mExponents.empty()) return "1";
			if(infix)
			{
				std::string result = "";
				for(auto vp = mExponents.begin(); vp != mExponents.end(); ++vp)
				{
					std::stringstream s;
					s << vp->exp;
					if(vp != mExponents.begin())
						result += "*";
					result += varToString(vp->var, friendlyVarNames) + (vp->exp > 1 ? ("^" + s.str()) : "");
				}
				return result;
			}
			else
			{
				std::string result = (mExponents.size() > 1 ? "(* " : "");
				for(auto vp = mExponents.begin(); vp != mExponents.end(); ++vp)
				{
					if(vp != mExponents.begin()) result += " ";
					std::stringstream s;
					s << vp->exp;
					std::string varName = varToString(vp->var, friendlyVarNames);
					if(vp->exp == 1) result += varName;
					else if(vp->exp > 1) // Is it necessary to check vp->exp > 1?
					{
						result += "(*";
						for(unsigned i = 0; i<vp->exp; ++i)
							result += " " + varName;
						result += ")";
					}
				}
				result += (mExponents.size() > 1 ? ")" : "");
				return result;
			}
		}

		friend std::ostream& operator<<( std::ostream& os, const Monomial& rhs )
		{
			return (os << rhs.toString(true, true));
		}
		
		static Monomial lcm(std::shared_ptr<const Monomial> lhs, std::shared_ptr<const Monomial> rhs)
		{
			if(!lhs)
			{
				if(!rhs)
				{
					return Monomial();
				}
				else
				{
					return *rhs;
				}
			}
			else
			{
				if(!rhs)
				{
					return *lhs;
				}
				else
				{
					return lcm(*lhs, *rhs);
				}
			}
				
		}
		
		static Monomial lcm(const Monomial& lhs, const Monomial& rhs)
		{
			LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
			assert(lhs.isConsistent());
			assert(rhs.isConsistent());
			Monomial result;
			result.mTotalDegree = lhs.tdeg() + rhs.tdeg();
			// Linear, as we expect small monomials.
			exponents_cIt itright = rhs.mExponents.begin();
			for(exponents_cIt itleft = lhs.mExponents.begin(); itleft != lhs.mExponents.end();)
			{
				// Done on right
				if(itright == rhs.mExponents.end())
				{
					// Insert remaining part
					result.mExponents.insert(result.mExponents.end(), itleft, lhs.mExponents.end());
					LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
					return result;
				}
				// Variable is present in both monomials.
				if(itleft->var == itright->var)
				{
					exponent newExp = std::max(itleft->exp, itright->exp);
					result.mExponents.push_back(VarExpPair(itleft->var, newExp));
					result.mTotalDegree -= std::min(itleft->exp, itright->exp);
					++itright;
					++itleft;
				}
				// Variable is not present in lhs, dividing lcm yields variable will not occur in result
				
				else if(itleft->var > itright->var) 
				{
					result.mExponents.push_back(*itright);
					++itright;
				}
				else
				{
					assert(itright->var > itleft->var);
					result.mExponents.push_back(*itleft);
					++itleft;
				}
			}
			 // Insert remaining part
			result.mExponents.insert(result.mExponents.end(), itright, rhs.mExponents.end());
			assert(result.isConsistent());
			LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
			return result;
			
		}

	private:
		
		/**
		 * Asserts that the data is valid.
		 * @return 
		 */
		bool isConsistent() const {
			unsigned tdeg = 0;
			unsigned lastVarIndex = 0;
			for(VarExpPair ve : mExponents)
			{
				if (ve.exp <= 0) return false;
				if (ve.var.getId() < lastVarIndex) return false;
				tdeg += ve.exp;
				lastVarIndex = ve.var.getId();
			}
			if (tdeg != mTotalDegree) return false;
			return true;
		}
		
		static CompareResult lexicalCompare(const Monomial& lhs, const Monomial& rhs)
		{
			exponents_cIt lhsit = lhs.mExponents.begin( );
			exponents_cIt rhsit = rhs.mExponents.begin( );
			exponents_cIt lhsend = lhs.mExponents.end( );
			exponents_cIt rhsend = rhs.mExponents.end( );

			while( lhsit != lhsend )
			{
				if( rhsit == rhsend )
					return CompareResult::GREATER;
				//which variable occurs first
				if( lhsit->var == rhsit->var )
				{
					//equal variables
					if( lhsit->exp < rhsit->exp )
						return CompareResult::LESS;
					if( lhsit->exp > rhsit->exp )
						return CompareResult::GREATER;
				}
				else
				{
					return (lhsit->var < rhsit->var ) ? CompareResult::GREATER : CompareResult::LESS;
				}
				++lhsit;
				++rhsit;
			}
			if( rhsit == rhsend )
				return CompareResult::EQUAL;
			return CompareResult::LESS;
		}
	};
	
	inline Monomial operator*(Variable::Arg lhs, Variable::Arg rhs)
		{
			// Note that this implementation is not optimized yet!
			Monomial result(lhs);
			result *= rhs;
			return result;
		}
} // namespace carl

namespace std
{
	template<>
	struct hash<carl::Monomial>
	{
		size_t operator()(const carl::Monomial& monomial) const 
		{
			std::hash<carl::Variable> h;
			size_t result = 0;
			for(unsigned i = 0; i < monomial.nrVariables(); ++i)
			{
				// perform a circular shift by 5 bits.
				result = (result << 5) | (result >> (sizeof(size_t)*8 - 5));
				result ^= h( monomial[i].var );
				result = (result << 5) | (result >> (sizeof(size_t)*8 - 5));
				result ^= monomial[i].exp;
			}
			return result;
		}
	};
} // namespace std


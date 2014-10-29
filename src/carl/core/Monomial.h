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
#include "VariablesInformation.h"
#include "logging.h"
#include "carlLoggingHelper.h"
#include "../numbers/numbers.h"
#include "VariablePool.h"

namespace carl
{   

	template<typename Coefficient>
	class Term;
	
	/// Type of an exponent.
	typedef unsigned exponent;
	
	/**
	 * Compare a pair of variable and exponent with a variable.
	 * Returns true, if both variables are the same.
	 * @param p Pair of variable and exponent.
	 * @param v Variable.
	 * @return `p.first == v`
	 */
	inline bool operator==(const std::pair<Variable, exponent>& p, Variable::Arg v) {
		return p.first == v;
	}

	/**
	 * The general-purpose monomials. Notice that we aim to keep this object as small as possbible,
	 * while also limiting the use of expensive language features such as RTTI, exceptions and even
	 * polymorphism.
	 * 
	 * Although a Monomial can conceptually be seen as a map from variables to exponents,
	 * this implementation uses a vector of pairs of variables and exponents.
	 * Due to the fact that monomials usually contain only a small number of variables,
	 * the overhead introduced by `std::map` makes up for the asymptotically slower `std::find` on 
	 * the `std::vector` that is used.
	 * 
	 * Besides, many operations like multiplication, division or substitution do not rely
	 * on finding some variable, but must iterate over all entries anyway.
	 * 
	 * @ingroup multirp
	 */
	class Monomial
	{
	protected:
		/// A vector of variable exponent pairs (v_i^e_i) with nonzero exponents. 
		std::vector<std::pair<Variable, exponent>> mExponents;
		/// Some applications performance depends on getting the degree of monomials very fast
		exponent mTotalDegree = 0;

		typedef std::vector<std::pair<Variable, exponent>>::iterator exponents_it;
		typedef std::vector<std::pair<Variable, exponent>>::const_iterator exponents_cIt;

		/**
		 * Default constructor.
		 */
		Monomial() = default;
	public:
		/**
		 * Generate a monomial from a variable and an exponent.
		 * @param v The variable.
		 * @param e The exponent.
		 */
		Monomial(Variable::Arg v, exponent e = 1) :
			mExponents(1, std::make_pair(v,e)),
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
		Monomial(std::vector<std::pair<Variable, exponent>>&& exponents, exponent totalDegree) :
			mExponents(exponents),
			mTotalDegree(totalDegree)
		{
			assert(isConsistent());
		}
		
		/**
		 * Generate a monomial from a vector of variable-exponent pairs and a total degree.
		 * @param exponents The variables and their exponents.
		 */
		explicit Monomial(std::vector<std::pair<Variable, exponent>>&& exponents) :
			mExponents(exponents),
			mTotalDegree(0)
		{
			
			for(auto const& ve : mExponents)
			{
				mTotalDegree += ve.second;
			}
			assert(isConsistent());
		}
		/**
		 * Assignment operator.
 * @param rhs Other monomial.
 * @return this.
 */
		Monomial& operator=(const Monomial& rhs)
		{
			// Check for self-assignment.
			if(this == &rhs) return *this;
			mExponents = rhs.mExponents;
			mTotalDegree = rhs.mTotalDegree;
			return *this;
		}
		
		/**
		 * Returns iterator on first pair of variable and exponent.
		 * @return Iterator on begin.
		 */
		exponents_it begin() {
			return mExponents.begin();
		}
		/**
		 * Returns constant iterator on first pair of variable and exponent.
		 * @return Iterator on begin.
		 */
		exponents_cIt begin() const {
			return mExponents.begin();
		}
		/**
		 * Returns past-the-end iterator.
		 * @return Iterator on end.
		 */
		exponents_it end() {
			return mExponents.end();
		}
		/**
		 * Returns past-the-end iterator.
		 * @return Iterator on end.
		 */
		exponents_cIt end() const {
			return mExponents.end();
		}
		
		/**
		 * Gives the total degree, i.e. the sum of all exponents.
		 * @return Total degree.
		 */
		exponent tdeg() const
		{
			return mTotalDegree;
		}
		
		const std::vector<std::pair<Variable, exponent>>& exponents() const {
			return mExponents;
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
			if (mTotalDegree % 2 == 1) return false;
			for (auto it: mExponents)
			{
				if (it.second % 2 == 1) return false;
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
			return mExponents.front().first;
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
				if(mExponents.front().first == v) return true;
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
		const std::pair<Variable, exponent>& operator[](unsigned index) const
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
			auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);
			if(it == mExponents.cend()) {
				return 0;
			} else {
				return it->second;
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
		
		/**
		 * For a monomial m = Prod( x_i^e_i ) * v^e, divides m by v^e
		 * @return nullptr if result is 1, otherwise m/v^e.
		 */
		Monomial* dropVariable(Variable::Arg v) const
		{
			LOG_FUNC("carl.core.monomial", mExponents << ", " << v);
			auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);
			
			if (it == mExponents.cend()) return new Monomial(*this);
			if (mExponents.size() == 1) return nullptr;
			
			exponent tDeg = mTotalDegree - it->second;
			std::vector<std::pair<Variable, exponent>> newExps(mExponents.begin(), it);
			it++;
			newExps.insert(newExps.end(), it, mExponents.end());
			return new Monomial(std::move(newExps), tDeg);
		}

		/**
		 * Divides the monomial by a variable v.
		 * If the division is impossible (because v does not occur in the monomial), nullptr is returned.
		 * @param v Variable
		 * @return This divided by v.
		 */
		Monomial* divide(Variable::Arg v) const
		{
			auto it = std::find(mExponents.cbegin(), mExponents.cend(), v);
			if(it == mExponents.cend())	return nullptr;
			else {
				Monomial* m = new Monomial();
				// If the exponent is one, the variable does not occur in the new monomial.
				if(it->second == 1) {
					if(it != mExponents.begin()) {
						m->mExponents.assign(mExponents.begin(), it);
					}
					m->mExponents.insert(m->mExponents.end(), it+1,mExponents.end());
				} else {
					// We have to decrease the exponent of the variable by one.
					m->mExponents.assign(mExponents.begin(), mExponents.end());
					m->mExponents[(unsigned)(it - mExponents.begin())].second -= 1;
				}
				m->mTotalDegree = mTotalDegree - 1;
				return m;
			}
		}

		
		/**
		 * Checks if this monomial is divisible by the given monomial m.
		 * @param m Monomial.
		 * @return If this is divisible by m.
		 */
		bool divisible(const Monomial& m) const
		{
			assert(isConsistent());
			if(m.mTotalDegree > mTotalDegree) return false;
			if(m.nrVariables() > nrVariables()) return false;
			// Linear, as we expect small monomials.
			auto itright = m.mExponents.begin();
			for(auto itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
			{
				// Done with division
				if(itright == m.mExponents.end())
				{
					return true;
				}
				// Variable is present in both monomials.
				if(itleft->first == itright->first)
				{
					if(itright->second > itleft->second)
					{
						// Underflow, itright->exp was larger than itleft->exp.
						return false;
					}
					itright++;
				}
				// Variable is not present in lhs, division fails.
				else if(itleft->first > itright->first) 
				{
					return false;
				}
				else
				{
					assert(itright->first > itleft->first);
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
		 * Returns a new monomial that is this monomial divided by m.
		 * If this is not divisible by m, nullptr is returned.
		 * @param m Monomial.
		 * @return this divided by m.
		 */
		Monomial* divide(const Monomial& m) const
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
			auto itright = m.mExponents.begin();
			for(auto itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
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
				if(itleft->first == itright->first)
				{
					if (itleft->second < itright->second)
					{
						// Underflow, itright->exp was larger than itleft->exp.
						delete result;
						LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
						return nullptr;
					}
					exponent newExp = itleft->second - itright->second;
					if(newExp > 0)
					{
						result->mExponents.push_back(std::make_pair(itleft->first, newExp));
					}
					itright++;
				}
				// Variable is not present in lhs, division fails.
				else if(itleft->first > itright->first) 
				{
					delete result;
					LOGMSG_TRACE("carl.core.monomial", "Result: nullptr");
					return nullptr;
				}
				else
				{
					assert(itright->first > itleft->first);
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
			if (result->mExponents.empty())
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
			auto itright = m.mExponents.begin();
			for(auto itleft = mExponents.begin(); itleft != mExponents.end();)
			{
				// Done with division
				if(itright == m.mExponents.end())
				{
					// Insert remaining part
					result->mExponents.insert(result->mExponents.end(), itleft, mExponents.end());
					return result;
				}
				// Variable is present in both monomials.
				if(itleft->first == itright->first)
				{
					exponent newExp = std::max(itleft->second, itright->second) - itright->second;
					if(newExp != 0)
					{
						result->mExponents.push_back(std::make_pair(itleft->first, newExp));
						result->mTotalDegree -= itright->second;
					}
					else
					{
						result->mTotalDegree -= itleft->second;
					}
					++itright;
					++itleft;
				}
				// Variable is not present in lhs, dividing lcm yields variable will not occur in result
				
				else if(itleft->first > itright->first) 
				{
					++itright;
				}
				else
				{
					assert(itright->first > itleft->first);
					result->mExponents.push_back(*itleft);
					++itleft;
				}
			}
			assert(result->isConsistent());
			return result;
		}
		
		template<typename Coeff, bool gatherCoeff, typename CoeffType>
		void gatherVarInfo(VariablesInformation<gatherCoeff, CoeffType>& varinfo, const Coeff& coeffFromTerm) const
		{
			for (auto ve : mExponents )
			{
				varinfo.variableInTerm(ve, coeffFromTerm, *this);
			}
		}
		
		/**
		 * Calculates the separable part of this monomial.
		 * For a monomial \f$ \\prod_i x_i^e_i with e_i \neq 0 \f$, this is \f$ \\prod_i x_i^1 \f$.
		 * @return Separable part.
		 */
		Monomial* separablePart() const
		{
			Monomial* m = new Monomial(*this);
			m->mTotalDegree = (exponent)mExponents.size();
			for (auto& it: m->mExponents) it.second = 1;
			return m;
		}

		/**
		 * Calculates the given power of this monomial.
		 * @param exp Exponent.
		 * @return this to the power of exp.
		 */
		Monomial* pow(unsigned exp) const {
			if (exp == 0) {
				return nullptr;
			}
			Monomial* res = new Monomial(*this);
			exponent expsum = 0;
			for (auto& it: res->mExponents) {
				it.second = (exponent)(it.second * exp);
				expsum += it.second;
			}
			res->mTotalDegree = expsum;
			assert(res->isConsistent());
			return res;
		}
		
		/**
		 * Fill the set of variables with the variables from this monomial.
		 * @param variables Variables.
		 */
		void gatherVariables(std::set<Variable>& variables) const
		{
			for (auto it: mExponents) {
				variables.insert(it.first);
			}
		}
		
		/**
		 * Computes the (partial) derivative of this monomial with respect to the given variable.
		 * @param v Variable.
		 * @return Partial derivative.
		 */
		template<typename Coefficient>
		Term<Coefficient>* derivative(Variable::Arg v) const;
		
		/**
		 * Applies the given substitutions to this monomial.
		 * Every variable may be substituted by some number. Additionally, a constant factor may be given that is multiplied with the result.
		 * @param substitutions Maps variables to numbers.
		 * @param factor A constant factor.
		 * @return \f$ factor \cdot this[<substitutions>] \f$
		 */
		template<typename Coefficient>
		Term<Coefficient>* substitute(const std::map<Variable, Coefficient>& substitutions, Coefficient factor) const;
		/**
		 * Applies the given substitutions to this monomial.
		 * Every variable may be substituted by some term. Additionally, a constant factor may be given that is multiplied with the result.
		 * @param substitutions Maps variables to terms.
		 * @param factor A constant factor.
		 * @return \f$ factor \cdot this[<substitutions>] \f$
		 */
		template<typename Coefficient>
		Term<Coefficient>* substitute(const std::map<Variable, Term<Coefficient>>& substitutions, const Coefficient& factor) const;
		///////////////////////////
		// Orderings
		///////////////////////////

		static CompareResult compareLexical(const Monomial& lhs, const Monomial& rhs)
		{
			return lexicalCompare(lhs,rhs);
		}
		
		/*static CompareResult compareLexical(const Monomial& lhs, Variable::Arg rhs)
		{
			if(lhs.mExponents.front().first < rhs) return CompareResult::LESS;
			if(lhs.mExponents.front().first > rhs) return CompareResult::GREATER;
			if(lhs.mExponents.front().second > 1) return CompareResult::GREATER;
			else return CompareResult::LESS;
		}*/


		static CompareResult compareGradedLexical(const Monomial& lhs, const Monomial& rhs)
		{
			
			if(lhs.mTotalDegree < rhs.mTotalDegree) return CompareResult::LESS;
			if(lhs.mTotalDegree > rhs.mTotalDegree) return CompareResult::GREATER;
			return lexicalCompare(lhs, rhs);
		}
		
		/*static CompareResult compareGradedLexical(const Monomial& lhs, Variable::Arg rhs)
		{
			if(lhs.mTotalDegree < 1) return CompareResult::LESS;
			if(lhs.mTotalDegree > 1) return CompareResult::GREATER;
			if(lhs.mExponents.front().first < rhs) return CompareResult::GREATER;
			if(lhs.mExponents.front().first > rhs) return CompareResult::LESS;
			else return CompareResult::EQUAL;
			
		}*/

		/**
		 * Multiplies this monomial with a variable.
		 * @param v Variable.
		 * @return This multiplied with v.
		 */
		Monomial& operator*=(Variable::Arg v)
		{
			++mTotalDegree;
			// Linear, as we expect small monomials.
			for(auto it = mExponents.begin(); it != mExponents.end(); ++it)
			{
				// Variable is present
				if(it->first == v)
				{
					++(it->second);
					return *this;
				}
				// Variable is not present, we have to insert v.
				if(it->first > v)
				{
					mExponents.emplace(it,v,1);
					return *this;
				}
			}
			// Variable was not inserted, insert at end.
			mExponents.emplace_back(v,1);
			return *this;
		}

		/**
		 * Multiplies this monomial with another monomial.
		 * @param rhs Monomial.
		 * @return This multiplied with rhs.
		 */
		Monomial& operator*=(const Monomial& rhs)
		{
			LOG_FUNC("carl.core.monomial", *this << ", " << rhs);
			if(rhs.mTotalDegree == 0) return *this;
			mTotalDegree += rhs.mTotalDegree;

			// Linear, as we expect small monomials.
			auto itright = rhs.mExponents.begin();
			assert(itright != rhs.mExponents.end());
			for(auto itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
			{
				// Everything is inserted.
				if(itright == rhs.mExponents.end())
				{
					LOGMSG_TRACE("carl.core.monomial", "Result: " << *this);
					return *this;
				}
				// Variable is present in both monomials.
				if(itleft->first == itright->first)
				{
					itleft->second += itright->second;
					++itright;
				}
				// Variable is not present in lhs, we have to insert var-exp pair from rhs.
				else if(itleft->first > itright->first) 
				{
					assert(itright != rhs.mExponents.end());
					itleft = mExponents.insert(itleft,*itright);
					++itright;
				
				}		
				else 
				{
					assert(itleft->first < itright->first);
				}

			}
			// Insert remainder of rhs.
			mExponents.insert(mExponents.end(), itright, rhs.mExponents.end());
			assert(isConsistent());
			LOGMSG_TRACE("carl.core.monomial", "Result: " << *this);
			return *this;
		}

		/**
		 * Returns the string representation of this monomial.
		 * @param infix Flag if prefix or infix notation should be used.
		 * @param friendlyVarNames Flag if friendly variable names should be used.
		 * @return String representation.
		 */
		std::string toString(bool infix = true, bool friendlyVarNames = true) const
		{
			if(mExponents.empty()) return "1";
			std::stringstream ss;
			if (infix) {
				for (auto vp = mExponents.begin(); vp != mExponents.end(); ++vp) {
					if (vp != mExponents.begin()) ss << "*";
					ss << vp->first;
					if (vp->second > 1) ss << "^" << vp->second;
				}
			} else {
				if (mExponents.size() > 1) ss << "(* ";
				for (auto vp = mExponents.begin(); vp != mExponents.end(); ++vp) {
					if (vp != mExponents.begin()) ss << " ";
					if (vp->second == 1) ss << vp->first;
					else {
						std::string varName = VariablePool::getInstance().getName(vp->first, friendlyVarNames);
						ss << "(*";
						for (unsigned i = 0; i < vp->second; i++) ss << " " << varName;
						ss << ")";
					}
				}
				if (mExponents.size() > 1) ss << ")";
			}
			return ss.str();
		}

		/**
		 * Streaming operator for Monomial.
		 * @param os Output stream. 
		 * @param rhs Monomial.
		 * @return `os`
		 */
		friend std::ostream& operator<<( std::ostream& os, const Monomial& rhs )
		{
			return (os << rhs.toString(true, true));
		}
		
		
		/**
		 * Calculates the least common multiple of two monomial pointers.
		 * If both are valid objects, the gcd of both is calculated.
		 * If only one is a valid object, this one is returned.
		 * If both are invalid objects, an empty monomial is returned.
		 * @param lhs First monomial.
		 * @param rhs Second monomial.
		 * @return gcd of lhs and rhs.
		 */
		static Monomial gcd(std::shared_ptr<const Monomial> lhs, std::shared_ptr<const Monomial> rhs)
		{
			if(!lhs && !rhs) return Monomial();
			if(!lhs) return *rhs;
			if(!rhs) return *lhs;
			return gcd(*lhs, *rhs);
		}
		
		/**
		 * Calculates the greatest common divisor of two monomials.
		 * @param lhs First monomial.
		 * @param rhs Second monomial.
		 * @return lcm of lhs and rhs.
		 */
		static Monomial gcd(const Monomial& rhs, const Monomial& lhs)
		{
			LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
			assert(lhs.isConsistent());
			assert(rhs.isConsistent());
			
			Monomial result;
			// Linear, as we expect small monomials.
			auto itright = rhs.mExponents.cbegin();
			auto leftEnd = lhs.mExponents.cend();
			auto rightEnd = rhs.mExponents.cend();
			for(auto itleft = lhs.mExponents.cbegin(); (itleft != leftEnd && itright != rightEnd);)
			{
				// Variable is present in both monomials.
				if(itleft->first == itright->first)
				{
					exponent newExp = std::min(itleft->second, itright->second);
					result.mExponents.push_back(std::make_pair(itleft->first, newExp));
					result.mTotalDegree += newExp;
					++itright;
					++itleft;
				}
				
				else if(itleft->first > itright->first) 
				{
					++itright;
				}
				else
				{
					assert(itright->first > itleft->first);
					++itleft;
				}
			}
			 // Insert remaining part
			assert(result.isConsistent());
			LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
			return result;
			
		}
		
		/**
		 * Calculates the least common multiple of two monomial pointers.
		 * If both are valid objects, the lcm of both is calculated.
		 * If only one is a valid object, this one is returned.
		 * If both are invalid objects, an empty monomial is returned.
		 * @param lhs First monomial.
		 * @param rhs Second monomial.
		 * @return lcm of lhs and rhs.
		 */
		static Monomial lcm(std::shared_ptr<const Monomial> lhs, std::shared_ptr<const Monomial> rhs)
		{
			if (!lhs && !rhs) return Monomial();
			if (!lhs) return *rhs;
			if (!rhs) return *lhs;
			return lcm(*lhs, *rhs);
				
		}

		/**
		 * Calculates the least common multiple of two monomials.
		 * @param lhs First monomial.
		 * @param rhs Second monomial.
		 * @return lcm of lhs and rhs.
		 */
		static Monomial lcm(const Monomial& lhs, const Monomial& rhs)
		{
			LOG_FUNC("carl.core.monomial", lhs << ", " << rhs);
			assert(lhs.isConsistent());
			assert(rhs.isConsistent());
			Monomial result;
			result.mTotalDegree = lhs.tdeg() + rhs.tdeg();
			// Linear, as we expect small monomials.
			auto itright = rhs.mExponents.cbegin();
			auto leftEnd = lhs.mExponents.cend();
			auto rightEnd = rhs.mExponents.cend();
			for(auto itleft = lhs.mExponents.cbegin(); itleft != leftEnd;)
			{
				// Done on right
				if(itright == rightEnd)
				{
					// Insert remaining part
					result.mExponents.insert(result.mExponents.end(), itleft, lhs.mExponents.end());
					LOGMSG_TRACE("carl.core.monomial", "Result: " << result);
					return result;
				}
				// Variable is present in both monomials.
				if(itleft->first == itright->first)
				{
					exponent newExp = std::max(itleft->second, itright->second);
					result.mExponents.push_back(std::make_pair(itleft->first, newExp));
					result.mTotalDegree -= std::min(itleft->second, itright->second);
					++itright;
					++itleft;
				}
				// Variable is not present in lhs, dividing lcm yields variable will not occur in result
				
				else if(itleft->first > itright->first) 
				{
					result.mExponents.push_back(*itright);
					++itright;
				}
				else
				{
					assert(itright->first > itleft->first);
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
		 * Checks if the monomial is consistent.
		 * @return If this is consistent.
		 */
		bool isConsistent() const {
			LOG_FUNC("carl.core.monomial", mExponents << ", " << mTotalDegree);
			if (mTotalDegree < 1) return false;
			unsigned tdeg = 0;
			unsigned lastVarIndex = 0;
			for(auto ve : mExponents)
			{
				if (ve.second <= 0) return false;
				if (ve.first.getId() < lastVarIndex) return false;
				tdeg += ve.second;
				lastVarIndex = ve.first.getId();
			}
			if (tdeg != mTotalDegree) return false;
			return true;
		}
		
		/**
		 * This method performs a lexical comparison as defined in @cite GCL92, page 47.
		 * We define the exponent vectors to be in decreasing order, i.e. the exponents of the larger variables first.
		 * @param lhs First monomial.
		 * @param rhs Second monomial.
		 * @return Comparison result.
		 * @see @cite GCL92, page 47.
		 */
		static CompareResult lexicalCompare(const Monomial& lhs, const Monomial& rhs)
		{
			auto lhsit = lhs.mExponents.rbegin( );
			auto rhsit = rhs.mExponents.rbegin( );
			auto lhsend = lhs.mExponents.rend( );
			auto rhsend = rhs.mExponents.rend( );

			while( lhsit != lhsend )
			{
				if( rhsit == rhsend )
					return CompareResult::GREATER;
				//which variable occurs first
				if( lhsit->first == rhsit->first )
				{
					//equal variables
					if( lhsit->second < rhsit->second )
						return CompareResult::LESS;
					if( lhsit->second > rhsit->second )
						return CompareResult::GREATER;
				}
				else
				{
					return (lhsit->first < rhsit->first ) ? CompareResult::LESS : CompareResult::GREATER;
				}
				++lhsit;
				++rhsit;
			}
			if( rhsit == rhsend )
				return CompareResult::EQUAL;
			return CompareResult::LESS;
		}
	};	
	
	/// @name Comparison operators
	/// @{
	/**
	 * Compares two arguments where one is a Monomial and the other is either a monomial or a variable.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs ~ rhs`, `~` being the relation that is checked.
	 */
	inline bool operator==(const Monomial& lhs, const Monomial& rhs) {
		if (lhs.tdeg() != rhs.tdeg()) return false;
		return lhs.exponents() == rhs.exponents();
	}
	inline bool operator==(const Monomial& lhs, Variable::Arg rhs) {
		if (lhs.tdeg() != 1) return false;
		if (lhs.begin()->first == rhs) return true;
		return false;
	}
	inline bool operator==(Variable::Arg lhs, const Monomial& rhs) {
		return rhs == lhs;
	}
	inline bool operator!=(const Monomial& lhs, const Monomial& rhs) {
		return !(lhs == rhs);
	}
	inline bool operator!=(const Monomial& lhs, Variable::Arg rhs) {
		return !(lhs == rhs);
	}
	inline bool operator!=(Variable::Arg lhs, const Monomial& rhs) {
		return !(rhs == lhs);
	}
	inline bool operator<(const Monomial& lhs, const Monomial& rhs) {
		CompareResult cr = Monomial::compareGradedLexical(lhs, rhs);
		return cr == CompareResult::LESS;
	}
	inline bool operator<(const Monomial& lhs, Variable::Arg rhs) {
		if (lhs.tdeg() == 0) return true;
		if (lhs.tdeg() > 1) return false;
		return lhs.begin()->first < rhs;
	}
	inline bool operator<(Variable::Arg lhs, const Monomial& rhs) {
		if (rhs.tdeg() == 0) return false;
		if (rhs.tdeg() > 1) return true;
		return lhs < rhs.begin()->first;
	}
	inline bool operator<=(const Monomial& lhs, const Monomial& rhs) {
		return !(rhs < lhs);
	}
	inline bool operator<=(const Monomial& lhs, Variable::Arg rhs) {
		return !(rhs < lhs);
	}
	inline bool operator<=(Variable::Arg lhs, const Monomial& rhs) {
		return !(rhs < lhs);
	}
	inline bool operator>(const Monomial& lhs, const Monomial& rhs) {
		return rhs < lhs;
	}
	inline bool operator>(const Monomial& lhs, Variable::Arg rhs) {
		return rhs < lhs;
	}
	inline bool operator>(Variable::Arg lhs, const Monomial& rhs) {
		return rhs < lhs;
	}
	inline bool operator>=(const Monomial& lhs, const Monomial& rhs) {
		return rhs <= lhs;
	}
	inline bool operator>=(const Monomial& lhs, Variable::Arg rhs) {
		return rhs <= lhs;
	}
	inline bool operator>=(Variable::Arg lhs, const Monomial& rhs) {
		return rhs <= lhs;
	}
	/// @}

	/// @name Multiplication operators
	/// @{
	/**
	 * Perform a multiplication involving a monomial.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs * rhs`
	 */
	inline Monomial operator*(const Monomial& lhs, const Monomial& rhs) {
		Monomial result(lhs);
		result *= rhs;
		return result;
	}
	inline Monomial operator*(const Monomial& lhs, Variable::Arg rhs) {
		Monomial result(lhs);
		result *= rhs;
		return result;
	}
	inline Monomial operator*(Variable::Arg lhs, const Monomial& rhs) {
		return rhs * lhs;
	}
	inline Monomial operator*(Variable::Arg lhs, Variable::Arg rhs) {
		Monomial result(lhs);
		result *= rhs;
		return result;
	}
	/// @}

} // namespace carl

namespace std
{
	template<>
	struct less<std::shared_ptr<const carl::Monomial>> {
		bool operator()(const std::shared_ptr<const carl::Monomial>& lhs, const std::shared_ptr<const carl::Monomial>& rhs) const {
			if (lhs && rhs) return *lhs < *rhs;
			if (lhs) return false;
			return true;
		}
	};
	/**
	 * The template specialization of `std::hash` for `carl::Monomial`.
	 * @param monomial Monomial.
	 * @return Hash of monomial.
	 */
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
				result ^= h( monomial[i].first );
				result = (result << 5) | (result >> (sizeof(size_t)*8 - 5));
				result ^= monomial[i].second;
			}
			return result;
		}
	};
} // namespace std


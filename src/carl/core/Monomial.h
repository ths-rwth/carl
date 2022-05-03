/**
 * @file Monomial.h 
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

#include <carl-common/util/hash.h>
#include "../numbers/numbers.h"
#include "CompareResult.h"
#include "Variable.h"
#include "Variables.h"
#include "VariablePool.h"

#include <algorithm>
#include <list>
#include <numeric>
#include <set>
#include <sstream>

#include <boost/intrusive/unordered_set.hpp>


namespace carl
{
	/// Type of an exponent.
	using exponent = std::size_t;
	
	/**
	 * Compare a pair of variable and exponent with a variable.
	 * Returns true, if both variables are the same.
	 * @param p Pair of variable and exponent.
	 * @param v Variable.
	 * @return `p.first == v`
	 */
	inline bool operator==(const std::pair<Variable, std::size_t>& p, Variable v) {
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
	class Monomial final : public boost::intrusive::unordered_set_base_hook<>
	{
		friend class MonomialPool;
	public:
		using Arg = std::shared_ptr<const Monomial>;
		using Content = std::vector<std::pair<Variable, std::size_t>>;
		~Monomial();

		/**
		 * Default constructor.
		 */
		Monomial() = delete;
		Monomial(const Monomial& rhs) = delete;
		Monomial(Monomial&& rhs) = delete;
	private:
		/// A vector of variable exponent pairs (v_i^e_i) with nonzero exponents.
		Content mExponents;
		/// Some applications performance depends on getting the degree of monomials very fast
		std::size_t mTotalDegree = 0;
		/// Monomial id.
		mutable std::size_t mId = 0;
		/// Cached hash.
		mutable std::size_t mHash = 0;

		using exponents_it = Content::iterator ;
		using exponents_cIt = Content::const_iterator;

		mutable std::weak_ptr<const Monomial> mWeakPtr;

		/**
		 * Calculates the hash and stores it to mHash.
		 */
		void calc_hash() {
			mHash = Monomial::hashContent(mExponents);
		}
		/**
		 * Calculates the total degree and stores it to mTotalDegree.
		 */
		void calc_total_degree() {
			mTotalDegree = std::accumulate(
				mExponents.begin(), mExponents.end(), std::size_t(0),
				[](std::size_t d, const auto& p) { return d + p.second; }
			);
		}

		/**
		 * Generate a monomial from a vector of variable-exponent pairs and a total degree.
		 * If the totalDegree is zero, it is recalculated.
		 * The content is not expected to be sorted.
		 * @param content The variables and their exponents.
		 * @param totalDegree The total degree of the monomial to generate, or zero.
		 */
		Monomial(Content&& content, std::size_t totalDegree = 0) :
			mExponents(std::move(content)),
			mTotalDegree(totalDegree)
		{
			std::sort(mExponents.begin(), mExponents.end(),
				[](const auto& p1, const auto& p2){ return p1.first < p2.first; }
			);
			if (mTotalDegree == 0) {
				calc_total_degree();
			}
			calc_hash();
			assert(isConsistent());
		}

		/**
		 * Generate a monomial from a vector of variable-exponent pairs and a total degree.
		 * If the totalDegree is zero, it is recalculated.
		 * The content is not expected to be sorted.
		 * @param content The variables and their exponents
		 * @param totalDegree The total degree of the monomial to generate, or zero.
		 */
		Monomial(const Content& content, std::size_t totalDegree = 0) :
			Monomial(Content(content), totalDegree)
		{}

	public:
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
		 * Returns the hash of this monomial
		 * @return Hash.
		 */
		std::size_t hash() const {
			return mHash;
		}

		/**
		 * Return the id of this monomial.
		 * @return Id.
		 */
		std::size_t id() const {
			return mId;
		}
		
		/**
		 * Gives the total degree, i.e. the sum of all exponents.
		 * @return Total degree.
		 */
		exponent tdeg() const {
			return mTotalDegree;
		}
		
		const Content& exponents() const {
			return mExponents;
		}
		
		/**
		 * Checks whether the monomial is a constant.
		 * @return If monomial is constant.
		 */
		bool isConstant() const {
			return mTotalDegree == 0;
		}
        
        /**
         * @return true, if the image of this monomial is integer-valued.
         */
        bool integerValued() const {
			auto res = std::find_if(
				mExponents.begin(), mExponents.end(),
				[](const auto& e){ return e.first.type() != VariableType::VT_INT; }
			);
			return res == mExponents.end();
        }
        
		/**
		 * Checks whether the monomial has exactly degree one.
		 * @return If monomial is linear.
		 */
		bool isLinear() const {
			return mTotalDegree == 1;
		}
		
		/**
		 * Checks whether the monomial has at most degree one.
		 * @return If monomial is linear or constant.
		 */
		bool isAtMostLinear() const {
			return mTotalDegree <= 1;
		}
		
		/**
		 * Checks whether the monomial is a square, i.e. whether all exponents are even.
		 * @return If monomial is a square.
		 */
		bool isSquare() const {
			if (mTotalDegree % 2 == 1) return false;
			auto res = std::find_if(
				mExponents.begin(), mExponents.end(),
				[](const auto& e){ return e.second % 2 == 1; }
			);
			return res == mExponents.end();
		}
		
		/**
		 * Returns the number of variables that occur in the monomial.
		 * @return Number of variables.
		 */
		std::size_t nrVariables() const {
			return mExponents.size();
		}

		/**
		 * Retrieves the single variable of the monomial.
		 * Asserts that there is in fact only a single variable.
		 * @return Variable.
		 */
		Variable getSingleVariable() const {
			assert(mExponents.size() == 1);
			return mExponents.front().first;
		}
		
		/**
		 * Checks that there is no other variable than the given one.
		 * @param v Variable.
		 * @return If there is only v.
		 */
		bool hasNoOtherVariable(Variable v) const {
			if(mExponents.size() == 1) {
				return mExponents.front().first == v;
			}
			return mExponents.empty();
		}
		
		/**
		 * Retrieves the given VarExpPair.
		 * @param index Index.
		 * @return VarExpPair.
		 */
		const std::pair<Variable, std::size_t>& operator[](std::size_t index) const {
			assert(index < mExponents.size());
			return mExponents[index];
		}
		/**
		 * Retrieves the exponent of the given variable.
		 * @param v Variable.
		 * @return Exponent of v.
		 */
		exponent exponentOfVariable(Variable v) const {
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
		bool has(Variable v) const {
			return (std::find(mExponents.cbegin(), mExponents.cend(), v) != mExponents.cend());
		}
		
		/**
		 * For a monomial m = Prod( x_i^{e_i} ) * v^e, divides m by v^e
		 * @return nullptr if result is 1, otherwise m/v^e.
		 */
		Monomial::Arg dropVariable(Variable v) const;

		/**
		 * Divides the monomial by a variable v.
		 * If the division is impossible (because v does not occur in the monomial), nullptr is returned.
		 * @param v Variable
		 * @param res Resulting monomial
		 * @return This divided by v.
		 */
		bool divide(Variable v, Monomial::Arg& res) const;

		
		/**
		 * Checks if this monomial is divisible by the given monomial m.
		 * @param m Monomial.
		 * @return If this is divisible by m.
		 */
		bool divisible(const Monomial::Arg& m) const
		{
			if(!m) return true;
			assert(isConsistent());
			if(m->mTotalDegree > mTotalDegree) return false;
			if(m->nrVariables() > nrVariables()) return false;
			// Linear, as we expect small monomials.
			auto itright = m->mExponents.begin();
			for (const auto& itleft: mExponents) {
				// Done with division
				if(itright == m->mExponents.end())
				{
					return true;
				}
				// Variable is present in both monomials.
				if(itleft.first == itright->first)
				{
					if(itright->second > itleft.second)
					{
						// Underflow, itright->exp was larger than itleft->exp.
						return false;
					}
					itright++;
				}
				// Variable is not present in lhs, division fails.
				else if(itleft.first > itright->first) 
				{
					return false;
				}
				else
				{
					assert(itright->first > itleft.first);
				}
			}
			// If there remain variables in the m, it fails.
			return itright == m->mExponents.end();
		}
		/**
		 * Returns a new monomial that is this monomial divided by m.
		 * Returns a pair of a monomial pointer and a bool.
		 * The bool indicates if the division was possible.
		 * The monomial pointer holds the result of the division.
		 * If the division resulted in an empty monomial (i.e. the two monomials were equal), the pointer is nullptr.
		 * @param m Monomial.
		 * @param res Resulting monomial.
		 * @return this divided by m.
		 */
		bool divide(const Monomial::Arg& m, Monomial::Arg& res) const;
		
		/**
		 * Calculates and returns the square root of this monomial, iff the monomial is a square as checked by isSquare().
		 * Otherwise, nullptr is returned.
		 * @return The square root of this monomial, iff the monomial is a square as checked by isSquare().
		 */
		Monomial::Arg sqrt() const;
		
		template<typename Coeff, typename VarInfo>
		void gatherVarInfo(VarInfo& varinfo, const Coeff& coeffFromTerm) const
		{
			for (auto ve : mExponents )
			{
				varinfo.variableInTerm(ve, coeffFromTerm, *this);
			}
		}

		///////////////////////////
		// Orderings
		///////////////////////////

		static CompareResult compareLexical(const Monomial::Arg& lhs, const Monomial::Arg& rhs)
		{
			if( !lhs && !rhs )
				return CompareResult::EQUAL;
			if( !lhs )
				return CompareResult::LESS;
			if( !rhs )
				return CompareResult::GREATER;
			return lexicalCompare(*lhs, *rhs);
		}
		
		static CompareResult compareLexical(const Monomial::Arg& lhs, Variable rhs)
		{
			if(!lhs) return CompareResult::LESS;
			if(lhs->mExponents.front().first < rhs) return CompareResult::GREATER;
			if(lhs->mExponents.front().first > rhs) return CompareResult::LESS;
			if(lhs->mExponents.front().second > 1) return CompareResult::GREATER;
			return CompareResult::EQUAL;
		}


		static CompareResult compareGradedLexical(const Monomial::Arg& lhs, const Monomial::Arg& rhs)
		{
			if( !lhs && !rhs )
				return CompareResult::EQUAL;
			if( !lhs )
				return CompareResult::LESS;
			if( !rhs )
				return CompareResult::GREATER;
			if(lhs->mTotalDegree < rhs->mTotalDegree) return CompareResult::LESS;
			if(lhs->mTotalDegree > rhs->mTotalDegree) return CompareResult::GREATER;
			return lexicalCompare(*lhs, *rhs);
		}
		
		static CompareResult compareGradedLexical(const Monomial::Arg& lhs, Variable rhs)
		{
			if(!lhs) return CompareResult::LESS;
			if(lhs->mTotalDegree > 1) return CompareResult::GREATER;
			if(lhs->mExponents.front().first < rhs) return CompareResult::GREATER;
			if(lhs->mExponents.front().first > rhs) return CompareResult::LESS;
			if(lhs->mExponents.front().second > 1) return CompareResult::GREATER;
			return CompareResult::EQUAL;
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
		static Monomial::Arg lcm(const Monomial::Arg& lhs, const Monomial::Arg& rhs);
		
		
		/**
		 * Returns lcm(lhs, rhs) / rhs
		 */
		static Monomial::Arg calcLcmAndDivideBy(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
			Monomial::Arg res;
			bool works = lcm(lhs, rhs)->divide(rhs, res);
			assert(works);
			return res;
		}
		
		/**
		 * This method performs a lexical comparison as defined in @cite GCL92, page 47.
		 * We define the exponent vectors to be in decreasing order, i.e. the exponents of the larger variables first.
		 * @param lhs First monomial.
		 * @param rhs Second monomial.
		 * @return Comparison result.
		 * @see @cite GCL92, page 47.
		 */
		static CompareResult lexicalCompare(const Monomial& lhs, const Monomial& rhs);

		/**
		 * Calculate the hash of a monomial based on its content.
		 * @param c Content of a monomial.
		 * @return Hash of the monomial.
		 */
		static std::size_t hashContent(const Monomial::Content& c) {
			return carl::hash_all(c);
		}

	public:
		
		/**
		 * Checks if the monomial is consistent.
		 * @return If this is consistent.
		 */
		bool isConsistent() const;
        
        /*
         * TODO: cannot link Monomial::evaluate
        template<typename SubstitutionType>
		SubstitutionType evaluate(const std::map<Variable, SubstitutionType>& map) const;
         */
	};
	
	/// @name Comparison operators
	/// @{


	inline bool operator==(const Monomial& lhs, const Monomial& rhs) {
		if (&lhs == &rhs) return true;
		if ((lhs.id() != 0) && (rhs.id() != 0)) return lhs.id() == rhs.id();
		if (lhs.hash() != rhs.hash()) return false;
		if (lhs.tdeg() != rhs.tdeg()) return false;
		return lhs.exponents() == rhs.exponents();
	}

	/**
	 * Compares two arguments where one is a Monomial and the other is either a monomial or a variable.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs ~ rhs`, `~` being the relation that is checked.
	 */
	inline bool operator==(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		if (lhs.get() == rhs.get()) return true;
		if (lhs == nullptr || rhs == nullptr) return false;
		if ((lhs->id() != 0) && (rhs->id() != 0)) return lhs->id() == rhs->id();
		if (lhs->hash() != rhs->hash()) return false;
		if (lhs->tdeg() != rhs->tdeg()) return false;
		return lhs->exponents() == rhs->exponents();
	}
	
	inline bool operator==(const Monomial::Arg& lhs, Variable rhs) {
		if (lhs == nullptr) return false;
		if (lhs->tdeg() != 1) return false;
		return lhs->begin()->first == rhs;
	}
	
	inline bool operator==(Variable lhs, const Monomial::Arg& rhs) {
		return rhs == lhs;
	}
	
	inline bool operator!=(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		return !(lhs == rhs);
	}
	
	inline bool operator!=(const Monomial::Arg& lhs, Variable rhs) {
		return !(lhs == rhs);
	}
	
	inline bool operator!=(Variable lhs, const Monomial::Arg& rhs) {
		return !(rhs == lhs);
	}
	
	inline bool operator<(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		if (lhs.get() == rhs.get()) return false;
		if (lhs == nullptr) return true;
		if (rhs == nullptr) return false;
		if ((lhs->id() != 0) && (rhs->id() != 0)) {
			if (lhs->id() == rhs->id()) return false;
			// TODO sort by id?
			//return (lhs->id() < rhs->id());
		}
		if(lhs->tdeg() < rhs->tdeg()) return true;
		if(lhs->tdeg() > rhs->tdeg()) return false;
		CompareResult cr = Monomial::lexicalCompare(*lhs, *rhs);
		return cr == CompareResult::LESS;
	}
	
	inline bool operator<(const Monomial::Arg& lhs, Variable rhs) {
		if (lhs == nullptr) return true;
		if (lhs->tdeg() > 1) return false;
		return lhs->begin()->first < rhs;
	}
	
	inline bool operator<(Variable lhs, const Monomial::Arg& rhs) {
		if (rhs == nullptr) return false;
		if (rhs->tdeg() > 1) return true;
		return lhs < rhs->begin()->first;
	}
	
	inline bool operator<=(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		return !(rhs < lhs);
	}
	
	inline bool operator<=(const Monomial::Arg& lhs, Variable rhs) {
		return !(rhs < lhs);
	}
	
	inline bool operator<=(Variable lhs, const Monomial::Arg& rhs) {
		return !(rhs < lhs);
	}
	
	inline bool operator>(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		return rhs < lhs;
	}
	
	inline bool operator>(const Monomial::Arg& lhs, Variable rhs) {
		return rhs < lhs;
	}
	
	inline bool operator>(Variable lhs, const Monomial::Arg& rhs) {
		return rhs < lhs;
	}
	
	inline bool operator>=(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		return rhs <= lhs;
	}
	
	inline bool operator>=(const Monomial::Arg& lhs, Variable rhs) {
		return rhs <= lhs;
	}
	
	inline bool operator>=(Variable lhs, const Monomial::Arg& rhs) {
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
	Monomial::Arg operator*(const Monomial::Arg& lhs, const Monomial::Arg& rhs);
	
	Monomial::Arg operator*(const Monomial::Arg& lhs, Variable rhs);
	
	Monomial::Arg operator*(Variable lhs, const Monomial::Arg& rhs);
	
	Monomial::Arg operator*(Variable lhs, Variable rhs);
	/// @}


	/**
	 * Streaming operator for Monomial.
	 * @param os Output stream. 
	 * @param rhs Monomial.
	 * @return `os`
	 */
	inline std::ostream& operator<<(std::ostream& os, const Monomial& rhs) {
		if (rhs.exponents().empty()) return os << "1";
		bool first = true;
		for (const auto& vp: rhs) {
			if (first) first = false;
			else os << "*";
			os << vp.first;
			if (vp.second > 1) os << "^" << vp.second;
		}
		return os;
	}
	/**
	 * Streaming operator for std::shared_ptr<Monomial>.
	 * @param os Output stream.
	 * @param rhs Monomial.
	 * @return `os`
	 */
	inline std::ostream& operator<<(std::ostream& os, const Monomial::Arg& rhs) {
		if (rhs) return os << *rhs;
		return os << "1";
	}
	
	Monomial::Arg pow(Variable v, std::size_t exp);

	/// Add the variables of the given monomial to the variables.
	inline void variables(const Monomial& m, carlVariables& vars) {
		for (const auto& e : m) {
			vars.add(e.first);
		}
	}

	struct hashLess {
		bool operator()(const Monomial& lhs, const Monomial& rhs) const {
			return lhs.hash() < rhs.hash();
		}
		bool operator()(const Monomial::Arg& lhs, const Monomial::Arg& rhs) const {
			if (lhs == rhs) return false;
			if (lhs && rhs) return (*this)(*lhs, *rhs);
			return bool(rhs);
		}
	};

	struct hashEqual {
		bool operator()(const Monomial& lhs, const Monomial& rhs) const {
			return lhs.hash() == rhs.hash();
		}
		bool operator()(const Monomial::Arg& lhs, const Monomial::Arg& rhs) const {
			if (lhs == rhs) return true;
			if (lhs && rhs) return (*this)(*lhs, *rhs);
			return false;
		}
	};

} // namespace carl

namespace std
{
	template<>
	struct equal_to<carl::Monomial::Arg> {
		bool operator()(const carl::Monomial::Arg& lhs, const carl::Monomial::Arg& rhs) const {
			return lhs == rhs;
		}
	};
	template<>
	struct less<carl::Monomial::Arg> {
		bool operator()(const carl::Monomial::Arg& lhs, const carl::Monomial::Arg& rhs) const {
			if (lhs && rhs) return lhs < rhs;
			return !lhs;
		}
	};
	
	/**
	 * The template specialization of `std::hash` for `carl::Monomial`.
	 * @param monomial Monomial.
	 * @return Hash of monomial.
	 */
	template<>
	struct hash<carl::Monomial> {
		std::size_t operator()(const carl::Monomial& monomial) const {
			return monomial.hash();
		}
	};
	
	/**
	 * The template specialization of `std::hash` for a shared pointer of a `carl::Monomial`.
	 * @param monomial The shared pointer to a monomial.
	 * @return Hash of monomial.
	 */
	template<>
	struct hash<carl::Monomial::Arg>
	{
		size_t operator()(const carl::Monomial::Arg& monomial) const 
		{
			if (!monomial) return 0;
			return monomial->hash();
		}
	};
} // namespace std

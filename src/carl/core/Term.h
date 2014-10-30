/*
 * @file Term.h
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

#include <memory>
#include "Monomial.h"
#include "VariablesInformation.h"
#include "Definiteness.h"
#include "../interval/Interval.h"
#include "pointerOperations.h"

namespace carl
{

/**
 * Represents a single term, that is a numeric coefficient and a monomial.
 * @ingroup multirp
 */
template<typename Coefficient>
class Term
{
	private:
		Coefficient mCoeff = Coefficient(0);
		std::shared_ptr<const Monomial> mMonomial;

	public:
		/**
		 * Default constructor. Constructs a term of value zero.
		 */
		Term();
		/**
		 * Constructs a term of value \f$ c \f$.
		 * @param c Coefficient.
		 */
		explicit Term(const Coefficient& c);
		/**
		 * Constructs a term of value \f$ v \f$.
		 * @param v Variable.
		 */
		explicit Term(Variable::Arg v);
		/**
		 * Constructs a term of value \f$ m \f$.
		 * @param m Monomial.
		 */
		explicit Term(const Monomial& m);
		/**
		 * Constructs a term of value \f$ m \f$.
		 * @param m Monomial pointer.
		 */
		explicit Term(const std::shared_ptr<const Monomial>& m);
		/**
		 * Constructs a term of value \f$ c \cdot m \f$.
		 * @param c Coefficient.
		 * @param m Monomial.
		 */
		Term(const Coefficient& c, const Monomial& m);
		/**
		 * Constructs a term of value \f$ c \cdot m \f$.
		 * @param c Coefficient.
		 * @param m Monomial pointer.
		 */
		Term(const Coefficient& c, const Monomial* m);
		/**
		 * Constructs a term of value \f$ c \cdot m \f$.
		 * @param c Coefficient.
		 * @param m Monomial pointer.
		 */
		Term(const Coefficient& c, const std::shared_ptr<const Monomial>& m);
		/**
		 * Constructs a term of value \f$ c \cdot v^e \f$.
		 * @param c Coefficient.
		 * @param v Variable.
		 * @param e Exponent.
		 */
		Term(const Coefficient& c, Variable::Arg v, exponent e);
		
		/**
		 * Get the coefficient.
		 * @return Coefficient.
		 */
		const Coefficient& coeff() const
		{
			return mCoeff;
		}
		
		
		
		/**
		 * Get the monomial.
		 * @return Monomial.
		 */
		std::shared_ptr<const Monomial> monomial() const
		{
			return mMonomial;
		}
		/**
		 * Gives the total degree, i.e. the sum of all exponents.
		 * @return Total degree.
		 */
		exponent tdeg() const
		{
			if(!mMonomial) return 0;
			return mMonomial->tdeg();
		}
		
		/**
		 * Checks whether the term is zero.
		 * @return 
		 */
		bool isZero() const
		{
			return mCoeff == Coefficient(0);
		}
		
		/**
		 * Checks whether the term equals one.
         * @return 
         */
		bool isOne() const
		{
			return (isConstant() && mCoeff == Coefficient(1));
		}
		/**
		 * Checks whether the monomial is a constant.
		 * @return 
		 */
		bool isConstant() const
		{
			return !mMonomial;
		}
		/**
		 * Checks whether the monomial has exactly the degree one.
		 * @return 
		 */
		bool isLinear() const
		{
			if(!mMonomial) return true;
			return mMonomial->isLinear();
		}
		/**
		 * 
		 * @return 
		 */
		size_t getNrVariables() const
		{
			if(!mMonomial) return 0;
			return mMonomial->nrVariables();
		}
		
		/**
		 * @param v The variable to check for its occurrence.
		 * @return true, if the variable occurs in this term.
		 */
		bool has(Variable::Arg v) const
		{
			if (!mMonomial) return false;
			return mMonomial->has(v);
		}
		
		/**
		 * Checks if the monomial is either a constant or the only variable occuring is the variable v.
		 * @param v The variable which may occur.
		 * @return true if no variable occurs, or just v occurs. 
		 */
		bool hasNoOtherVariable(Variable::Arg v) const
		{
			if(!mMonomial) return true;
			return mMonomial->hasNoOtherVariable(v);
		}
		
		/**
		 * For terms with exactly one variable, get this variable.
		 * @return The only variable occuring in the term.
		 */
		Variable::Arg getSingleVariable() const
		{
			assert(getNrVariables() == 1);
			return mMonomial->getSingleVariable();
		}
		
		/**
		 * Checks if the term is a square.
		 * @return If this is square.
		 */
		bool isSquare() const
		{
			return (mCoeff >= Coefficient(0)) && ((!mMonomial) || mMonomial->isSquare());
		}
		
		/**
		 * Set the term to zero with the canonical representation.
		 */
		void clear()
		{
			mCoeff = Coefficient(0);
			mMonomial.reset();
		}

		/**
		 * Negates the term by negating the coefficient.
		 */
		void negate()
		{
			mCoeff = -mCoeff;
		}

		/**
		 * 
		 * @param c a non-zero coefficient.
		 * @return 
		 */
		Term* divideBy(const Coefficient& c) const;
		/**
		 * 
		 * @param v
		 * @return 
		 */
		Term* divideBy(Variable::Arg v) const;
		/**
		 * 
		 * @param m
		 * @return 
		 */
		Term* divideBy(const Monomial& m) const;
		
		Term* divideBy(const Term&) const;
		
		Term calcLcmAndDivideBy( const Monomial&) const;
		
		template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
		bool divisible(const Term&) const;
		template<typename C = Coefficient, DisableIf<is_field<C>> = dummy>
		bool divisible(const Term&) const;
		
		Term* derivative(Variable::Arg) const;
		
		Definiteness definiteness() const;
		
		template<typename SubstitutionType=Coefficient>
		Term* substitute(const std::map<Variable, SubstitutionType>& substitutions) const;
		Term* substitute(const std::map<Variable, Term<Coefficient>>& substitutions) const;
		
		
		template<bool gatherCoeff, typename CoeffType>
		void gatherVarInfo(Variable::Arg var, VariableInformation<gatherCoeff, CoeffType>& varinfo) const;
		
		template<bool gatherCoeff, typename CoeffType>
		void gatherVarInfo(VariablesInformation<gatherCoeff, CoeffType>& varinfo) const;
		
		void gatherVariables(std::set<Variable>& variables) const
		{
			if(mMonomial)
			{
				mMonomial->gatherVariables(variables);
			}
		}
		
		Term* pow(unsigned exp) const
		{
			if(mMonomial)
			{
				return new Term(carl::pow(coeff(),exp), mMonomial->pow(exp));
			}
			else
			{
				return new Term(carl::pow(coeff(),exp), mMonomial);
			}
			
		}

	    template<typename C = Coefficient, DisableIf<is_interval<C>> = dummy>
	    std::string toString(bool infix=true, bool friendlyVarNames=true) const;
        
	    template<typename C = Coefficient, EnableIf<is_interval<C>> = dummy>
	    std::string toString(bool infix=true, bool friendlyVarNames=true) const;

		bool isConsistent() const;
		
		const Term<Coefficient> operator-() const;
		
		/// @name In-place multiplication operators
		/// @{
		/**
		 * Multiply this term with something and return the changed term.
		 * @param rhs Right hand side.
		 * @return Changed term.
		 */
		Term& operator *=(const Coefficient& rhs);
		Term& operator *=(Variable::Arg rhs);
		Term& operator *=(const Monomial& rhs);
		Term& operator *=(const Term& rhs);
		/// @}

		/// @name Division operators
		/// @{
		/**
		 * Perform a division involving a term.
		 * @param lhs Left hand side.
		 * @param rhs Right hand side.
		 * @return `lhs / rhs`
		 */
		template<typename Coeff>
		friend const Term<Coeff> operator/(const Term<Coeff>& lhs, unsigned long rhs);
		/// @}
		
		/**
		 * Streaming operator for Term.
		 * @param os Output stream.
		 * @param rhs Term.
		 * @return `os`
		 */
		template<typename Coeff>
		friend std::ostream& operator<<(std::ostream& os, const Term<Coeff>& rhs);
		
		/**
		 * Checks if two terms have the same monomial.
         * @param lhs First term.
         * @param rhs Second term.
         * @return If both terms have the same monomial.
         */
		static bool monomialEqual(const Term& lhs, const Term& rhs)
		{
			return std::equal_to<std::shared_ptr<Monomial>>()(lhs.mMonomial, rhs.mMonomial);
		}
		static bool monomialLess(const Term& lhs, const Term& rhs)
		{
			return std::less<std::shared_ptr<const Monomial>>()(lhs.mMonomial, rhs.mMonomial);
		}
		static bool monomialLessPtr(const std::shared_ptr<const Term>& lhs, const std::shared_ptr<const Term>& rhs)
		{
			if (lhs && rhs) return monomialLess(*lhs, *rhs);
			if (lhs) return false;
			return true;
		}
		/**
		 * Calculates the gcd of (t1, t2).
		 * If t1 or t2 are not set, undefined.
		 * If t1 or t2 is zero, undefined.
         * @param t1 first term
         * @param t2 second term
         * @return gcd(t1,t2);
         */
		static Term gcd(std::shared_ptr<const Term> t1, std::shared_ptr<const Term> t2);
		/**
		 * Calculates the gcd of (t1, t2).
		 * If t1 or t2 is zero, undefined.
         * @param t1 first term
         * @param t2 second term
         * @return gcd(t1,t2)
         */
		static Term gcd(const Term& t1, const Term& t2);
};

	/// @name Comparison operators
	/// @{
	/**
	 * Compares two arguments where one is a term and the other is either a term, a monomial or a variable.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs ~ rhs`, `~` being the relation that is checked.
	 */
	template<typename Coeff>
	inline bool operator==(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
	template<typename Coeff>
	inline bool operator==(const Term<Coeff>& lhs, const Monomial& rhs);
	template<typename Coeff>
	inline bool operator==(const Term<Coeff>& lhs, Variable::Arg rhs);
	template<typename Coeff>
	inline bool operator==(const Term<Coeff>& lhs, const Coeff& rhs);
	template<typename Coeff>
	inline bool operator==(const Monomial& lhs, const Term<Coeff>& rhs) {
		return rhs == lhs;
	}
	template<typename Coeff>
	inline bool operator==(Variable::Arg lhs, const Term<Coeff>& rhs) {
		return rhs == lhs;
	}
	template<typename Coeff>
	inline bool operator==(const Coeff& lhs, const Term<Coeff>& rhs) {
		return rhs == lhs;
	}

	template<typename Coeff>
	inline bool operator!=(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Term<Coeff>& lhs, const Monomial& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Term<Coeff>& lhs, Variable::Arg rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Term<Coeff>& lhs, const Coeff& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Monomial& lhs, const Term<Coeff>& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(Variable::Arg lhs, const Term<Coeff>& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Coeff& lhs, const Term<Coeff>& rhs) {
		return !(lhs == rhs);
	}

	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, const Monomial& rhs);
	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, Variable::Arg rhs);
	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, const Coeff& rhs);
	template<typename Coeff>
	bool operator<(const Monomial& lhs, const Term<Coeff>& rhs);
	template<typename Coeff>
	bool operator<(Variable::Arg lhs, const Term<Coeff>& rhs);
	template<typename Coeff>
	bool operator<(const Coeff& lhs, const Term<Coeff>& rhs);

	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, const Monomial& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, Variable::Arg rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, const Coeff& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Monomial& lhs, const Term<Coeff>& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(Variable::Arg lhs, const Term<Coeff>& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Coeff& lhs, const Term<Coeff>& rhs) {
		return !(rhs < lhs);
	}

	template<typename Coeff>
	inline bool operator>(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Term<Coeff>& lhs, const Monomial& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Term<Coeff>& lhs, Variable::Arg rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Term<Coeff>& lhs, const Coeff& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Monomial& lhs, const Term<Coeff>& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(Variable::Arg lhs, const Term<Coeff>& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Coeff& lhs, const Term<Coeff>& rhs) {
		return rhs < lhs;
	}

	template<typename Coeff>
	inline bool operator>=(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Term<Coeff>& lhs, const Monomial& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Term<Coeff>& lhs, Variable::Arg rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Term<Coeff>& lhs, const Coeff& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Monomial& lhs, const Term<Coeff>& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(Variable::Arg lhs, const Term<Coeff>& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Coeff& lhs, const Term<Coeff>& rhs) {
		return rhs <= lhs;
	}
	/// @}
	
	/// @name Multiplication operators
	/// @{
	/**
	 * Perform a multiplication involving a term.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs * rhs`
	 */
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
		Term<Coeff> res(lhs);
		return res *= rhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Term<Coeff>& lhs, const Monomial& rhs) {
		Term<Coeff> res(lhs);
		return res *= rhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Term<Coeff>& lhs, Variable::Arg rhs) {
		Term<Coeff> res(lhs);
		return res *= rhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Term<Coeff>& lhs, const Coeff& rhs) {
		Term<Coeff> res(lhs);
		return res *= rhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Monomial& lhs, const Term<Coeff>& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Monomial& lhs, const Coeff& rhs) {
		return Term<Coeff>(rhs, lhs);
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(Variable::Arg lhs, const Term<Coeff>& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(Variable::Arg lhs, const Coeff& rhs) {
		return Term<Coeff>(rhs, lhs);
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Coeff& lhs, const Term<Coeff>& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Coeff& lhs, const Monomial& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff>
	inline const Term<Coeff> operator*(const Coeff& lhs, Variable::Arg rhs) {
		return rhs * lhs;
	}
	/// @}

} // namespace carl

namespace std {

	template<typename Coefficient>
	struct less<std::shared_ptr<const carl::Term<Coefficient>>> {
		bool operator()(const std::shared_ptr<const carl::Term<Coefficient>>& lhs, const std::shared_ptr<const carl::Term<Coefficient>>& rhs) {
			if (lhs && rhs) return *lhs < *rhs;
			if (lhs) return true;
			return false;
		}
	};

	/**
	 * Specialization of `std::hash` for a Term.
	 */
	template<typename Coefficient>
	struct hash<carl::Term<Coefficient>> {
		/**
		 * Calculates the hash of a Term.
		 * @param term Term.
		 * @return Hash of term.
		 */
		size_t operator()(const carl::Term<Coefficient>& term) const {
			if(term.isConstant())
				return hash<Coefficient>()(term.coeff()) | (size_t)1;
			else
				return hash<Coefficient>()(term.coeff()) ^ hash<carl::Monomial>()(*term.monomial());
		}
	};
} // namespace std

#include "Term.tpp"

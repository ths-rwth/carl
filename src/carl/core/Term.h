/*
 * @file Term.h
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

#include "../interval/Interval.h"
#include "../numbers/number/NumberMpq.h"
#include "../numbers/number/NumberClRA.h"
#include "../numbers/number/NumberInterval.h"
#include "../numbers/number/NumberInt.h"
#include "../numbers/numbers.h"
#include "../util/pointerOperations.h"
#include "Definiteness.h"
#include "Monomial.h"
#include "VariablesInformation.h"

#include <memory>


namespace carl
{
    
/**
 * Represents a single term, that is a numeric coefficient and a monomial.
 * @ingroup multirp
 */
template<typename Coefficient>
class Term {
	private:
		using CoefficientType = Coefficient;
		CoefficientType mCoeff = CoefficientType(constant_zero<Coefficient>::get());
		Monomial::Arg mMonomial;

	public:
		/**
		 * Default constructor. Constructs a term of value zero.
		 */
		Term();
		
		Term(const Term& t): mCoeff(t.mCoeff), mMonomial(t.mMonomial) {
			assert(isConsistent());
		}
		Term(Term&& t): mCoeff(std::move(t.mCoeff)), mMonomial(std::move(t.mMonomial)) {
			assert(isConsistent());
		}
		Term& operator=(const Term& t) = default;
		Term& operator=(Term&& t) = default;
		/**
		 * Constructs a term of value \f$ c \f$.
		 * @param c Coefficient.
		 */
		explicit Term(const Coefficient& c);
		/**
		 * Constructs a term of value \f$ v \f$.
		 * @param v Variable.
		 */
		explicit Term(Variable v);
		/**
		 * Constructs a term of value \f$ m \f$.
		 * @param m Monomial pointer.
		 */
		explicit Term(Monomial::Arg m);
		/**
		 * Constructs a term of value \f$ m \f$.
		 * @param m Monomial pointer.
		 */
		explicit Term(Monomial::Arg&& m);

		/**
		 * Constructs a term of value \f$ c \cdot m \f$.
		 * @param c Coefficient.
		 * @param m Monomial pointer.
		 */
		Term(const Coefficient& c, Monomial::Arg m);
        
		/**
		 * Constructs a term of value \f$ c \cdot m \f$.
		 * @param c Coefficient.
		 * @param m Monomial pointer.
		 */
		Term(Coefficient&& c, Monomial::Arg&& m);
		/**
		 * Constructs a term of value \f$ c \cdot v^e \f$.
		 * @param c Coefficient.
		 * @param v Variable.
		 * @param e Exponent.
		 */
		Term(const Coefficient& c, Variable v, uint e);
		
		/**
		 * Get the coefficient.
		 * @return Coefficient.
		 */
		Coefficient& coeff() {
			return mCoeff;
		}
		const Coefficient& coeff() const {
			return mCoeff;
		}
		
		/**
		 * Get the monomial.
		 * @return Monomial.
		 */
		const Monomial::Arg& monomial() const {
			return mMonomial;
		}
		/**
		 * Gives the total degree, i.e. the sum of all exponents.
		 * @return Total degree.
		 */
		uint tdeg() const {
			if(!mMonomial) return 0;
			return mMonomial->tdeg();
		}
		
		/**
		 * Checks whether the term is zero.
		 * @return 
		 */
		bool isZero() const {
			return carl::isZero(mCoeff); //change this to mCoeff.isZero() at some point
		}
		
		/**
		 * Checks whether the term equals one.
         * @return 
         */
		bool isOne() const {
			return isConstant() && carl::isOne(mCoeff); //change this to mCoeff.isOne() at some point
		}
		/**
		 * Checks whether the monomial is a constant.
		 * @return 
		 */
		bool isConstant() const {
			return !mMonomial;
		}
        
        /**
         * @return true, if the image of this term is integer-valued.
         */
        bool integerValued() const {
            if (!carl::isInteger(mCoeff)) return false;
            return !mMonomial || mMonomial->integerValued();
        }
        
		/**
		 * Checks whether the monomial has exactly the degree one.
		 * @return 
		 */
		bool isLinear() const {
			if (!mMonomial) return true;
			return mMonomial->isLinear();
		}
		/**
		 * 
		 * @return 
		 */
		std::size_t getNrVariables() const {
			if (!mMonomial) return 0;
			return mMonomial->nrVariables();
		}
        
        /**
         * @return An approximation of the complexity of this monomial.
         */
        std::size_t complexity() const {
            if (!mMonomial) return 1;
            return mMonomial->complexity();
        }
		
		/**
		 * @param v The variable to check for its occurrence.
		 * @return true, if the variable occurs in this term.
		 */
		bool has(Variable v) const {
			if (!mMonomial) return false;
			return mMonomial->has(v);
		}
		
		/**
		 * Checks if the monomial is either a constant or the only variable occuring is the variable v.
		 * @param v The variable which may occur.
		 * @return true if no variable occurs, or just v occurs. 
		 */
		bool hasNoOtherVariable(Variable v) const {
			if (!mMonomial) return true;
			return mMonomial->hasNoOtherVariable(v);
		}
		
        bool isSingleVariable() const {
            if (!mMonomial) return false;
            return mMonomial->isLinear();
        }
		/**
		 * For terms with exactly one variable, get this variable.
		 * @return The only variable occuring in the term.
		 */
		Variable getSingleVariable() const {
			assert(getNrVariables() == 1);
			return mMonomial->getSingleVariable();
		}
		
		/**
		 * Checks if the term is a square.
		 * @return If this is square.
		 */
		bool isSquare() const {
			return (mCoeff >= CoefficientType(0)) && ((!mMonomial) || mMonomial->isSquare());
		}
		
		/**
		 * Set the term to zero with the canonical representation.
		 */
		void clear()
		{
			mCoeff = CoefficientType(carl::constant_zero<Coefficient>().get());
			mMonomial = nullptr;
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
		Term divide(const Coefficient& c) const;
		bool divide(const Coefficient& c, Term& res) const;
		
		bool divide(Variable v, Term& res) const;
		
		bool divide(const Monomial::Arg& m, Term& res) const;
		
		bool divide(const Term& t, Term& res) const;
		
		Term calcLcmAndDivideBy(const Monomial::Arg& m) const;
        
        /**
		 * Calculates the square root of this term.
		 * Returns true, iff the term is a square as checked by isSquare().
		 * In that case, res will changed to be the square root.
		 * Otherwise, res is undefined.
		 * @param res Square root of this term.
		 * @return If square root could be calculated.
		 */
		bool sqrt(Term& res) const;
		
		template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
		bool divisible(const Term& t) const;
		template<typename C = Coefficient, DisableIf<is_field<C>> = dummy>
		bool divisible(const Term& t) const;
		
		Term derivative(Variable v) const;
		
		Definiteness definiteness() const;
		
		Term substitute(const std::map<Variable, Coefficient>& substitutions) const;
		Term substitute(const std::map<Variable, Term<Coefficient>>& substitutions) const;
        Coefficient evaluate(const std::map<Variable, Coefficient>& map) const;
		
		
		template<bool gatherCoeff, typename CoeffType>
		void gatherVarInfo(Variable var, VariableInformation<gatherCoeff, CoeffType>& varinfo) const;
		
		template<bool gatherCoeff, typename CoeffType>
		void gatherVarInfo(VariablesInformation<gatherCoeff, CoeffType>& varinfo) const;
		
		void gatherVariables(std::set<Variable>& variables) const
		{
			if(mMonomial)
			{
				mMonomial->gatherVariables(variables);
			}
		}
		
		Term pow(uint exp) const
		{
			if(mMonomial)
			{
				return Term(carl::pow(coeff(),exp), mMonomial->pow(exp));
			}
			else
			{
				return Term(carl::pow(coeff(),exp), mMonomial);
			}
			
		}
        
	    template<typename C = Coefficient, DisableIf<is_interval<C>> = dummy>
	    std::string toString(bool infix=true, bool friendlyVarNames=true) const;
        
	    template<typename C = Coefficient, EnableIf<is_interval<C>> = dummy>
	    std::string toString(bool infix=true, bool friendlyVarNames=true) const;

		bool isConsistent() const;
		
		Term<Coefficient> operator-() const;
		
		/// @name In-place multiplication operators
		/// @{
		/**
		 * Multiply this term with something and return the changed term.
		 * @param rhs Right hand side.
		 * @return Changed term.
		 */
		Term& operator*=(const Coefficient& rhs);
		Term& operator*=(Variable rhs);
		Term& operator*=(const Monomial::Arg& rhs);
		Term& operator*=(const Term& rhs);
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
		friend const Term<Coeff> operator/(const Term<Coeff>& lhs, uint rhs);
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
			return lhs.mMonomial == rhs.mMonomial;
		}
		static bool monomialEqual(const std::shared_ptr<const Term>& lhs, const std::shared_ptr<const Term>& rhs)
		{
			if (lhs == rhs) return true;
			if (lhs && rhs) return monomialEqual(*lhs, *rhs);
			return false;
		}
		static bool monomialLess(const Term& lhs, const Term& rhs)
		{
			return lhs.mMonomial < rhs.mMonomial;
		}
		static bool monomialLess(const std::shared_ptr<const Term>& lhs, const std::shared_ptr<const Term>& rhs)
		{
			if (lhs && rhs) return monomialLess(*lhs, *rhs);
			return !lhs;
		}
		/**
		 * Calculates the gcd of (t1, t2).
		 * If t1 or t2 are not set, undefined.
		 * If t1 or t2 is zero, undefined.
         * @param t1 first term
         * @param t2 second term
         * @return gcd(t1,t2);
         */
		static Term gcd(const std::shared_ptr<const Term>& t1, const std::shared_ptr<const Term>& t2);
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
	inline bool operator==(const Term<Coeff>& lhs, Variable rhs);
	template<typename Coeff>
	inline bool operator==(const Term<Coeff>& lhs, const Coeff& rhs);
	template<typename Coeff>
	inline bool operator==(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
		return rhs == lhs;
	}
	template<typename Coeff>
	inline bool operator==(Variable lhs, const Term<Coeff>& rhs) {
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
	inline bool operator!=(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Term<Coeff>& lhs, Variable rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Term<Coeff>& lhs, const Coeff& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(Variable lhs, const Term<Coeff>& rhs) {
		return !(lhs == rhs);
	}
	template<typename Coeff>
	inline bool operator!=(const Coeff& lhs, const Term<Coeff>& rhs) {
		return !(lhs == rhs);
	}

	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, const Monomial::Arg& rhs);
	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, Variable rhs);
	template<typename Coeff>
	bool operator<(const Term<Coeff>& lhs, const Coeff& rhs);
	template<typename Coeff>
	bool operator<(const Monomial::Arg& lhs, const Term<Coeff>& rhs);
	template<typename Coeff>
	bool operator<(Variable lhs, const Term<Coeff>& rhs);
	template<typename Coeff>
	bool operator<(const Coeff& lhs, const Term<Coeff>& rhs);

	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, Variable rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Term<Coeff>& lhs, const Coeff& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
		return !(rhs < lhs);
	}
	template<typename Coeff>
	inline bool operator<=(Variable lhs, const Term<Coeff>& rhs) {
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
	inline bool operator>(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Term<Coeff>& lhs, Variable rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Term<Coeff>& lhs, const Coeff& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
		return rhs < lhs;
	}
	template<typename Coeff>
	inline bool operator>(Variable lhs, const Term<Coeff>& rhs) {
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
	inline bool operator>=(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Term<Coeff>& lhs, Variable rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Term<Coeff>& lhs, const Coeff& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
		return rhs <= lhs;
	}
	template<typename Coeff>
	inline bool operator>=(Variable lhs, const Term<Coeff>& rhs) {
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
	inline Term<Coeff> operator*(const Term<Coeff>& lhs, const Term<Coeff>& rhs) {
		return Term<Coeff>(lhs) *= rhs;
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(const Term<Coeff>& lhs, const Monomial::Arg& rhs) {
		return Term<Coeff>(lhs) *= rhs;
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(const Term<Coeff>& lhs, Variable rhs) {
		return Term<Coeff>(lhs) *= rhs;
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(const Term<Coeff>& lhs, const Coeff& rhs) {
		return Term<Coeff>(lhs) *= rhs;
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(const Monomial::Arg& lhs, const Term<Coeff>& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff, EnableIf<carl::is_number<Coeff>> = dummy>
	inline Term<Coeff> operator*(const Monomial::Arg& lhs, const Coeff& rhs) {
		return Term<Coeff>(rhs, lhs);
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(Variable lhs, const Term<Coeff>& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(Variable lhs, const Coeff& rhs) {
		return Term<Coeff>(rhs, lhs, 1);
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(const Coeff& lhs, const Term<Coeff>& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff, EnableIf<carl::is_number<Coeff>> = dummy>
	inline Term<Coeff> operator*(const Coeff& lhs, const Monomial::Arg& rhs) {
		return rhs * lhs;
	}
	template<typename Coeff>
	inline Term<Coeff> operator*(const Coeff& lhs, Variable rhs) {
		return rhs * lhs;
	}
    template<typename Coeff, EnableIf<carl::is_subset_of_rationals<Coeff>> = dummy>
    inline Term<Coeff> operator/(const Term<Coeff>& lhs, const Coeff& rhs) {
        return lhs * reciprocal(rhs);
    }
    template<typename Coeff, EnableIf<carl::is_subset_of_rationals<Coeff>> = dummy>
    inline Term<Coeff> operator/(const Monomial::Arg& lhs, const Coeff& rhs) {
        return lhs * reciprocal(rhs);
    }
    template<typename Coeff, EnableIf<carl::is_subset_of_rationals<Coeff>> = dummy>
    inline Term<Coeff> operator/(Variable& lhs, const Coeff& rhs) {
        return lhs * reciprocal(rhs);
    }
	/// @}

} // namespace carl

namespace std {

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
		std::size_t operator()(const carl::Term<Coefficient>& term) const {
            if (term.isConstant()) {
                return carl::hash_all(term.coeff());
            } else {
                return carl::hash_all(term.coeff(), *term.monomial());
            }
		}
	};
} // namespace std

#include "Term.tpp"

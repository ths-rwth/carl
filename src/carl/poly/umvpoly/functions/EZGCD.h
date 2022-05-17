/** 
 * @file   EZGCD.h
 * @ingroup gcd
 * @ingroup multirp
 * @author Sebastian Junges
 */

#pragma once
#include "../MultivariatePolynomial.h"
#include "../../numbers/PrimeFactory.h"
#include "GCD.h"

namespace carl
{
/**
 * Extended Zassenhaus algorithm for multivariate GCD calculation.
 * @ingroup gcd
 * @ingroup multirp
 */
template<typename Coeff, typename Ordering= GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class EZGCD
{
	
	typedef MultivariatePolynomial<Coeff,Ordering,Policies> Polynomial;
	typedef GCDResult<Coeff,Ordering,Policies> Result;
	typedef	typename Polynomial::TermType Term; 
	typedef typename IntegralType<Coeff>::type Integer;
	typedef UnivariatePolynomial<MultivariatePolynomial<Coeff,Ordering,Policies>> UnivReprPol;
	typedef UnivariatePolynomial<Coeff> UnivPol;
	
	const Polynomial& mp1;
	const Polynomial& mp2;
	PrimeFactory<Integer> mPrimeFactory;
	
	
	public:
	EZGCD(const MultivariatePolynomial<Coeff,Ordering,Policies>& p1,const MultivariatePolynomial<Coeff,Ordering,Policies>& p2)
	: mp1(p1), mp2(p2)
	{
		
	}
	
	/**
	 * 
     * @param approx
     * @return 
     */
	Result calculate(bool approx=true)
	{
		assert(approx);
		
		// We start with some trivial cases.
		if(mp1 == (Coeff)1 || mp2 == (Coeff)1) return {Polynomial((Coeff)1), Polynomial((Coeff)1), Polynomial((Coeff)1)};
		if(mp1.nrTerms() == 1 && mp2.nrTerms() == 1)
		{
			//return Polynomial(Term::gcd(*mp1.lterm(), *mp2.lterm()));
		}
		
		// And we do some simplifications for the input.
		// In order to do so, we gather some information about the polynomials, as we most certainly need them later on.
		
		// We check for mutual trivial factorizations.
		
		// And we check for linearly appearing variables. Notice that ay + b is irreducible and thus,
		// gcd(p, ay + b) is either ay + b or 1.
		
		// Here, we follow notation from @cite GCL92. We also add the notation from MY73.
		Variable x = getMainVar(mp1, mp2);
		UnivReprPol A = mp1.toUnivariatePolynomial(x);
		UnivReprPol B = mp2.toUnivariatePolynomial(x);
		Polynomial a;// = A.cont(); // In MY73, fbar
		Polynomial b;// = B.cont(); // In MY73, gbar.
		A /= a; // In MY73, F
		B /= b; // IN MY73, G.
		Polynomial g = gcd(a,b); // In MY72, dbar
		a = a.divideBy(g).quotient;
		b = b.divideBy(g).quotient;
		
		Integer p = getPrime(A,B);
		std::map<Variable, Integer> eval_b = findEval(A,B,p); // bold b in @cite GCL92.
		UnivPol A_I = A.evaluateCoefficient(eval_b).mod(p); // F_b
		UnivPol B_I = B.evaluateCoefficient(eval_b).mod(p); // G_b
		UnivPol C_I = UnivPol::gcd(A_I, B_I); // In MY73, D_b
		unsigned d =  C_I.degree(); // In MY72, delta.
		if(d == 0)
		{
			// In MY73 step A3.
			return {Polynomial(a * A), Polynomial(b * B), g};
		}
		while(true)
		{
			Integer p_prime = getPrime(A,B);
			std::map<Variable, Integer> eval_c = findEval(A,B,p_prime); // bold c in book.
			UnivPol A_I_prime = A.evaluateCoefficient(eval_c).mod(p_prime);
			UnivPol B_I_prime = B.evaluateCoefficient(eval_c).mod(p_prime);
			UnivPol C_I_prime = UnivPol::gcd(A_I, B_I);
			unsigned d_I_prime = C_I_prime.degree();
			if(d_I_prime < d)
			{
				// Previous evaluation was bad. 
				A_I = A_I_prime;
				B_I = B_I_prime;
				C_I = C_I_prime;
				d = d_I_prime;
				eval_b = eval_c;
				continue;
			}
			else if (d < d_I_prime)
			{
				// This evaluation was bad. 
				continue;
			}
			assert(d == d_I_prime);
			
			// Test for special cases.
			if(d == 0)
			{
				return {Polynomial(a * A), Polynomial(b * B), g};
			}
			if(d == B.degree())
			{
				if(A.divides(B))
				{
					return {Polynomial(a*(A.divideBy(B).quotient)), b, Polynomial(B*g)};
				}
				else
				{
					d--;
					continue;
				}
			}
			// Check for relatively prime cofactors
			// In MY73, step A6.
			UnivReprPol U_I(x);
			UnivPol H_I(x);
			Polynomial c;
			if(UnivPol::gcd(B_I, C_I).is_one())
			{
				U_I = B;   //  B = G.
				H_I = B_I.divideBy(C_I).quotient; // B_o[hat] = G_b / D_b
				c = b; // Could not find in MY73
			}
			else if(UnivPol::gcd(A_I, C_I).is_one())
			{
				U_I = A; // B = F.
				H_I = A_I.divideBy(C_I).quotient; //B_o[hat] = F_b / D_b
				c = a; // Could not find in MY73
			}
			else
			{
				// Special gcd.
				CARL_LOG_NOTIMPLEMENTED();
			}
			
			// Lifting step.
			// 
			U_I = c*U_I;
			Coeff c_I = mod(c.evaluate(eval_b),p);
			C_I = c_I * C_I;
			//MultivariateH
			CARL_LOG_NOTIMPLEMENTED();
			std::vector<Polynomial> CE; //= EZ_LIFT(U_I, C_I, H_i, b, c_I) // EZ_LIFT(U_I, C_I, H_i, b, p, c)
			assert(CE.size() == 2);
			if(U_I == CE.front()*CE.back()) 
			{
				continue;
			}
			Polynomial C;// = CE.front().primitivePart();
			if(C.divides(mp2) && C.divides(mp1))
			{
				return {a*mp1/C, b*mp2/C, g*C};
			}
			else
			{
				continue;
			}
		}
	}

	private:
	/**
	 * Given the two polynomials, find a suitable main variable for gcd.
     * @param p1
     * @param p2
     * @return NoVariable if intersection is empty, otherwise some variable v which is in p1 and p2.
     */
	Variable getMainVar(const Polynomial p1, const Polynomial p2) const
	{
		// TODO find good heuristic.
		std::vector<Variable> common;
		auto v1 = carl::variables(p1).as_vector();
		auto v2 = carl::variables(p2).as_vector();
		
		std::set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),
                  std::inserter(common,common.begin()));
		if(common.empty())
		{
			return Variable::NO_VARIABLE;
		}
		else
		{
			return *common.begin();
		}
		
	}	
		
	Integer getPrime(const UnivReprPol& A, const UnivReprPol& B)
	{
		Integer p;
		do 
		{
			p = mPrimeFactory.next_prime();
		} while( carl::is_zero(!A.lcoeff().mod(p)) || carl::is_zero(!A.lcoeff().mod(p)));
		return p;
		
	}
	
	/**
     * Find a valid evaluation point b = (b_1, ... , b_k)
	 * with 0 <= b_i < p and b_i = 0 for as many i as possible.
     * @param A Polynomial in Z[x, y_1,...,y_k]
     * @param B Polynomial in Z[x, y_1,...,y_k]
     * @param p Prime number
     * @return the evaluation point.
     */
	std::map<Variable, Integer> findEval( const UnivReprPol& A, const UnivReprPol& B, Integer p ) const
	{
		std::map<Variable, Integer> result;
		
		//assert(A.evaluateCoefficient(result).;
		return result;
	}
	
	
};

}
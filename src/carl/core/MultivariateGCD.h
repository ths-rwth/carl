/** 
 * @file:   MultivariateGCD.h
 * @author: Sebastian Junges
 *
 * @since September 3, 2013
 */

#pragma once
#include "MultivariatePolynomial.h"
#include "../numbers/PrimeFactory.h"


namespace carl
{

template<typename Coeff, typename Ordering, typename Policy>
class MultivariateGCD
{
	
	typedef MultivariatePolynomial<Coeff,Ordering,Policy> Polynomial;
	typedef	typename Polynomial::TermType Term; 
	typedef typename IntegralT<Coeff>::type Integer;
	typedef UnivariatePolynomial<typename Polynomial> UnivPol;
	
	const Polynomial& mp1;
	const Polynomial& mp2;
	PrimeFactory<Integer> mPrimeFactory;
	
	
	public:
	MultivariateGCD(const MultivariatePolynomial<Coeff,Ordering,Policy>& p1,const MultivariatePolynomial<Coeff,Ordering,Policy>& p2)
	: mp1(p1), mp2(p2)
	{
		
	}
	
	/**
	 * 
     * @param approx
     * @return 
     */
	MultivariatePolynomial<Coeff, Ordering, Policy> calculate(bool approx=true)
	{
		assert(approx);
		
		// We start with some trivial cases.
		if(mp1 == (Coeff)1 || mp2 == (Coeff)1) return Polynomial((Coeff)1);
		if(mp1->nrTerms() == 1 && mp2->nrTerms() == 1)
		{
			return Polynomial(Term::gcd(*mp1->lterm(), *mp2->lterm()));
		}
		
		// And we do some simplifications for the input.
		// In order to do so, we gather some information about the polynomials, as we most certainly need them later on.
		
		// We check for mutual trivial factorizations.
		
		// And we check for linearly appearing variables. Notice that ay + b is irreducible and thus,
		// gcd(p, ay + b) is either ay + b or 1.
		
		Variable x = getMainVar(mp1, mp2);
		UnivPol A;// = mp1.toUnivariatePolynomial(x);
		UnivPol B;// = mp2.toUnivariatePolynomial(y);
		Polynomial a;// = A.cont();
		Polynomial b;// = B.cont();
		A /= a;
		B /= b;
		Polynomial g = gcd(a,b);
		a /= g;
		b /= g;
		
		Integer p = getPrime(A,B);
		std::map<Variable, Integer> eval_b = findEval(A,B,p); // bold b in book.
		UnivPol A_I(x);// = A.evaluateCoefficient(eval_b).mod(p);
		UnivPol B_I(x);// = B.evaluateCoefficient(eval_b).mod(p);
		UnivPol C_I(x);// = gcd(A_I, B_I);
		unsigned d =  C_I.degree();
		if(d == 0)
		{
			return GCDResult(a * A, b * B, g);
		}
		while(true)
		{
			Integer p_prime = getPrime(A,B);
			std::map<Variable, Integer> eval_c = findEval(A,B,p_prime); // bold c in book.
			UnivPol A_I_prime(x);// = A.evaluateCoefficient(eval_c).mod(p_prime);
			UnivPol B_I_prime(x);// = B.evaluateCoefficient(eval_c).mod(p_prime);
			UnivPol C_I_prime(x);// = gcd(A_I, B_I);
			unsigned d_I_prime C_I_prime.degree();
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
				return GCDResult(a * A, b * B, g);
			}
			if(d == B.degree())
			{
				if(A.divides(B))
				{
					return GCDResult(a*A/B, b, B*g);
				}
				else
				{
					d--;
					continue;
				}
			}
			// Check for relatively prime cofactors
			UnivPol U_I(x), H_I(x);
			Polynomial c;
			if(gcd(B_I, C_I) == 1)
			{
				U_I = B;
				H_I = B_I/C_I;
				c = b;
			}
			else if(gcd(A_I, C_I) == 1)
			{
				U_I = A;
				H_I = A_I/C_I;
				c = a;
			}
			else
			{
				LOG_NOTIMPLEMENTED();
			}
			
			// Lifting step.
			U_I = c*U_I;
			Coeff c_I = c.evaluate(eval_b).mod(p);
			C_I = c_I * C_I;
			std::vector<Polynomial> CE; //= EZ_LIFT(U_I, C_I, H_i, b, c_I) // EZ_LIFT(U_I, C_I, H_i, b, c)
			assert(CE.size() == 2);
			if(U_I == CE.front()*CE.back) 
			{
				continue;
			}
			Polynomial C = CE.front().primitivePart;
			if(C.divides(B) && C.divides(A))
			{
				return GCDResult(a*A/C, b*B/C, g*C);
			}
			else
			{
				continue;
			}
		}
	}

	private:
	Integer getPrime(const UnivPol& A, const UnivPol& B)
	{
		Integer p;
		do 
		{
			p = mPrimeFactory.nextPrime();
		} while(mod(A.lcoeff(), p) != 0 || mod(B.lcoeff(), p) != 0);
		return p;
		
	}
	
};
}
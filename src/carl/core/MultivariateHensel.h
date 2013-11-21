/** 
 * @file:   MultivariateHensel.h
 * @author: Sebastian Junges
 *
 * @since October 14, 2013
 */

#pragma once
#include "../numbers/GFNumber.h"
#include "UnivariatePolynomial.h"
#include "logging.h"

/**
 * Includes the algorithms 6.2 and 6.3 from the book 
 * Algorithms for Computer Algebra by Geddes, Czaper, Labahn.
 * 
 * The Algorithms are used to computer the Multivariate GCD.
 */
namespace carl
{
template<typename Integer>
class DiophantineEquations
{
	typedef UnivariatePolynomial<GFNumber<Integer>> Polynomial;
	const GaloisField<Integer>* mGf_pk;
	const GaloisField<Integer>* mGf_p;
	
	public:
	DiophantineEquations(unsigned p, unsigned k) :
	mGf_pk(GaloisFieldManager<Integer>::getInstance().getField(p,k)),
    mGf_p(GaloisFieldManager<Integer>::getInstance().getField(p))
	{
		
	}
	
	std::vector<Polynomial> solveMultivariateDiophantine(const std::vector<Polynomial>& a,
														 const Polynomial& c,
														 const std::map<Variable, Integer>& I,
														 unsigned d) const
	{
		assert(a.size() > (unsigned)1);
		size_t r = a.size();
		size_t v = I.size() + 1;
		Variable x_v = I.rend()->first;
		Integer a_v = I.rend()->second;
		if(v > 1)
		{
			LOG_NOTIMPLEMENTED();
		}
		else
		{
			
		}
		
	}
	
	/**
	 * Solve in Z_(p^k)[x] the univariate polynomial Diophantine equation:
	 *  s_1 x b_1 + ... s_r x b_r === x^m (mod p^k)
	 * where in terms of the given list a: [a_1, ... a_r] the polynomials b_i 
	 * for i = 1...r are defined by:
	 * b_i = a_1 x ... x a_{i-1} x a_{i+1} x ... x a_r
	 * The unique solution s_1,... s_r, will be computed such that deg(s_i) < deg(a_i).
	 */
	std::vector<Polynomial> univariateDiophantine(const std::vector<Polynomial>& a, Variable::Arg x, unsigned m) const
	{
		assert(a.size() > 1);
		Polynomial xm(x,GFNumber<Integer>(1, mGf_pk),m);
		if(a.size() == 2)
		{
			std::vector<Polynomial> s = EEAlift(a.back(), a.front());
			DivisionResult<Polynomial> d = (xm * s.front()).divide(a.front());
			Polynomial q = d.quotient.normalizeCoefficients();
			return { d.remainder.normalizeCoefficients(), (xm * s.back() + q * a.back()).normalizeCoefficients() };
		}
		else
		{
			assert(a.size() > 2);
			std::vector<Polynomial> s = MultiTermEEAlift(a);
			assert(a.size() == s.size());
			std::vector<Polynomial> res;
			for(unsigned j = 0; j < s.size(); ++j)
			{
				res.push_back( (xm * s.at(j)).reduce(a.at(j)).normalizeCoefficients());
			}
			return res;
		}
	}
	private:
	std::vector<Polynomial> MultiTermEEAlift(const std::vector<Polynomial>& a) const
	{
		assert(a.size() > 2);
		const Variable& x = a.front().mainVar();
		size_t r = a.size();
		
		std::vector<Polynomial> q(r,Polynomial(x));
		q[r-1] = a.back();
		// TODO this folding should be more elegant..
		for(size_t j = r - 2; j < r-1; --j)
		{
			q[j] = a[j+1] * q[j+1];
		}
		std::vector<Polynomial> s;
		Polynomial beta(x,GFNumber<Integer>(1,mGf_pk),0);
		for(unsigned j = 0; j < r-1; ++j)
		{
			std::vector<Polynomial> sigma = solveMultivariateDiophantine({q.at(j), a.at(j)}, beta, {}, (unsigned)0);
			assert(sigma.size() == 2);
			beta = sigma.front();
			s.push_back(sigma.back());
		}
		s.push_back(beta);
		
		assert(s.size() == a.size());
		return s;
	}
	
	/**
	 * EEAlift computes s,t such that s*a + tb == 1 (mod p^k)
	 * with deg(s) < deg(b) and deg(t) < deg(a).
	 * Assumption GCD(a mod p, b mod p) = 1 in Z_p[x]
     * @param a Polynomial in Z_p^k[x]
     * @param b Polynomial in Z_p^k[x]
     * @return 
     */
	std::vector<Polynomial> EEAlift(Polynomial a, Polynomial b) const
	{
		assert(a.mainVar() == b.mainVar());
		LOGMSG_DEBUG("carl.core.hensel", "EEALIFT: a=" << a << ", b=" << b );
		const Variable& x = a.mainVar();
		Polynomial amodp = a.toFiniteDomain(mGf_p);
		Polynomial bmodp = b.toFiniteDomain(mGf_p);
		LOGMSG_DEBUG("carl.core.hensel", "EEALIFT: a mod p=" << amodp << ", b mod p=" << bmodp );
		Polynomial s(x);
		Polynomial t(x);
		Polynomial g(x);
		g = Polynomial::extended_gcd(amodp,bmodp,s,t);
		LOGMSG_DEBUG("carl.core.hensel", "EEALIFT: g=" << g << ", s=" << s << ", t=" << t );
		LOG_ASSERT(g.isOne(), "g expected to be one");
		Polynomial smodp = s;
		Polynomial tmodp = t;
		assert( mGf_p->p() == mGf_pk->p());
		Integer p = mGf_p->p();
		Integer modulus = p;
		Polynomial e(x),c(x);
		for(unsigned j=1; j<mGf_pk->k(); ++j)
		{
			// TODO check moduli.
			// e = 1 - s*a - t*b. // c = (e/modulus) mod p.
			GFNumber<Integer> one(1,mGf_pk);
			c =  -s*a-t*b+one;
			UnivariatePolynomial<Integer> cf = c.toIntegerDomain();
			cf /= modulus;
			c = cf.toFiniteDomain(mGf_p);
			Polynomial sigmaprime =  smodp * c;
			Polynomial tauprime = tmodp * c;
			DivisionResult<Polynomial> d = sigmaprime.divide(bmodp);
			Polynomial q = d.quotient.normalizeCoefficients();
			Polynomial sigma = d.remainder.normalizeCoefficients();
			Polynomial tau = (tauprime + q * amodp).normalizeCoefficients();
			s += sigma*modulus;
			t += tau*modulus;
			modulus *= p;
		}
		assert((s.toFiniteDomain(mGf_pk)*a + t.toFiniteDomain(mGf_pk)*b).isOne());
		return {s,t};
		
	}
	
	
	
};
}
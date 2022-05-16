/** 
 * @file:   MultivariateHensel.h
 * @author: Sebastian Junges
 *
 * @since October 14, 2013
 */

#pragma once
#include <carl/numbers/numbers.h>
#include "../UnivariatePolynomial.h"
#include <carl-logging/carl-logging.h>
#include <list>


namespace carl
{

/**
 * Includes the algorithms 6.2 and 6.3 from the book 
 * Algorithms for Computer Algebra by Geddes, Czaper, Labahn.
 * 
 * The Algorithms are used to computer the Multivariate GCD.
 */
template<typename Integer>
class DiophantineEquations
{
	typedef UnivariatePolynomial<GFNumber<Integer>> Polynomial;
        typedef MultivariatePolynomial<GFNumber<Integer>> MultiPoly;
	const GaloisField<Integer>* mGf_pk;
	const GaloisField<Integer>* mGf_p;
	
	public:
	DiophantineEquations(unsigned p, unsigned k) :
	mGf_pk(GaloisFieldManager<Integer>::getInstance().field(p,k)),
    mGf_p(GaloisFieldManager<Integer>::getInstance().field(p))
	{
		
	}
	/**
         * Solve in the domain Z_(p^k)[x_!,...,x_v] the multivariate polynomial
         * diophantine equation
         *      sigma_1 * b_1 + ... sigma_r * b_r = c (mod <I^(d+1), p^k>)
         * where, in terms of the given list of polynomials a_1,...,a_r
         * the polynomials b_i, i = 1,...,r, are defined by:
         *      b_i = a_1 * ... * a_(i-1) * a_(i+1) * ... * a_r.
         * The unique solution sigma_i, i = 1,...,r, will be computed such that
         *      degree(sigma_i,x_i) < degree(a_i,x_1).
         * 
         * Conditions:
         * (1) p must not divide lcoeff(a_i mod I), i = 1,...,r;
         * (2) A_i mod <I,p>, i = 1,...,r, must be pairwise relatively prime
         * in Z_p[x_1];
         * (3) degree(c,x_1) < sum(degree(a_i,x_1), i = 1,...,r)
         *
         * The prime integer p and the positive integer k must bei specified in
         * the constructor.
         * 
         * @param a A list a of r > 1 polynomials in the domain Z_(p^k)[x_1,...,x_v].
         * @param c A polynomial c from Z_(p^k)[x_1,...,x_v].
         * @param I A list of equations [x_2 = alpha_2,...,x_v = alpha_v].
         * @param d A nonnegative integer d specifying the maximum total degree 
         * with respect to x_2,...,x_v of the desired result.
         * @return The list sigma = [sigma_1,...,sigma_r].
         */
	std::vector<Polynomial> solveMultivariateDiophantine(
                        const std::vector<Polynomial>& a, // must be of type MultiPoly??
                        const MultiPoly& c,
			const std::map<Variable, GFNumber<Integer>>& I, // should be  Integer instead of GFNumber<Integer> ??
			unsigned d) const
	{
                // TODO: check the conditions
		assert(a.size() > (unsigned)1);
                size_t r = a.size();
                size_t v = I.size() + 1;
                Variable x_v = I.rend()->first;
                Integer alpha_v = I.rend()->second;
                if(v > 1) {
                        // Multivariate case
                        CARL_LOG_NOTIMPLEMENTED();
                        // A = product(a_i, i = 1,...,r)
                        MultiPoly A;
                        A = MultiPoly(1);
                        for(unsigned i = 1; i <= r; i++) {
                                A = A * a[i];
                        }
                        // b_j = A / a_j
                        std::vector<MultiPoly> b;
                        for(unsigned j = 1; j <= r; j++) {
                                b[j] = A / Multipoly(a[j]);
                        }
                        // a_new = substitute(x_v = alpha_v, a)
                        std::vector<Polynomial> a_new;
                        for(unsigned i = 1; i <= r; i++) {
                                a_new.pushBack(substitute(a[i], x_v, alpha_v));
                        }
                        // c_new = substitute(x_v = alpha_v, c);
                        Polynomial c_new = substitute(c,x_v, alpha_v);
                        // I_new = updated list I with x_v = alpha_v deleted
                        std::map<Variable, GFNumber<Integer>> I_new(I);
                        I_new.erase(x_v);
                        // sigma = MultivariateDiohant(a_new,c_new,I_new,d,p,k)
                        std::vector<Polynomial> sigma = MultivariateDiophant(a_new, c_new, I_new, d);
                        // e = (c - sum(sigma_i * b_i, i = 1,...,r))) mod p^k
                        // note that the mod p^k operation is performed automatically
                        MultiPoly e(c);
                        for(unsigned i = 1; i <= r; i++) {
                                c -= sigma[i] * b[i];
                        }
                        // monomial = 1
                        for(unsigned m = 1; m <= d; m++) {
                                if(e == 0) break;
                                
                           
                        }
                } 
                else {
                        // Univariate Case
                        /// @todo implement
                        // the list of equations is empty, a contains univarate polnomials
                        //Variable x_1 = a[0].mainVar();
                        // sigma = zero lost of length  r
                        std::vector<Polynomial> sigma(r, Polynomial(0));
                        for(auto& z : c.getTerms()) {
                                
                        }
                }
                //Prvent warning
                return {};
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
			DivisionResult<Polynomial> d = (xm * s.front()).divideBy(a.front());
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
				res.push_back( (xm * s.at(j)).remainder(a.at(j)).normalizeCoefficients());
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
			std::vector<Polynomial> sigma = solveMultivariateDiophantine({q.at(j), a.at(j)}, beta, std::map<Variable, Integer>(), (unsigned)0);
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
		CARL_LOG_DEBUG("carl.core.hensel", "EEALIFT: a=" << a << ", b=" << b );
		const Variable& x = a.mainVar();
		Polynomial amodp = a.toFiniteDomain(mGf_p);
		Polynomial bmodp = b.toFiniteDomain(mGf_p);
		CARL_LOG_DEBUG("carl.core.hensel", "EEALIFT: a mod p=" << amodp << ", b mod p=" << bmodp );
		Polynomial s(x);
		Polynomial t(x);
		Polynomial g(x);
		g = Polynomial::extended_gcd(amodp,bmodp,s,t);
		CARL_LOG_DEBUG("carl.core.hensel", "EEALIFT: g=" << g << ", s=" << s << ", t=" << t );
		CARL_LOG_ASSERT("carl.core.hensel", g.isOne(), "g expected to be one");
		Polynomial smodp = s;
		Polynomial tmodp = t;
		assert( mGf_p->p() == mGf_pk->p());
		Integer p = mGf_p->p();
		Integer modulus = p;
		Polynomial c(x);
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
			DivisionResult<Polynomial> d = sigmaprime.divideBy(bmodp);
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

template<typename Coeff, typename Ordering, typename Policies>
class MultivariateHensel
{
	typedef Coeff Integer;
	typedef UnivariatePolynomial<MultivariatePolynomial<Coeff,Ordering,Policies>> UnivReprPol;
	
	
	static std::list<UnivReprPol> calculate(const UnivReprPol& , const std::map<Variable, Coeff>& , Integer )
	{
		
	}
	//{}
};
}

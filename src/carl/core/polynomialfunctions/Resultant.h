#pragma once

#include <list>
#include <vector>

namespace carl {
enum class SubresultantStrategy {
	Generic, Lazard, Ducos, Default = Lazard
};

template<typename Coeff>
class UnivariatePolynomial;

template<typename Coeff>
std::list<UnivariatePolynomial<Coeff>> subresultants(const UnivariatePolynomial<Coeff>&, const UnivariatePolynomial<Coeff>&, SubresultantStrategy = SubresultantStrategy::Default);
template<typename Coeff>
std::vector<UnivariatePolynomial<Coeff>> principalSubresultantsCoefficients(const UnivariatePolynomial<Coeff>&, const UnivariatePolynomial<Coeff>&, SubresultantStrategy = SubresultantStrategy::Default);
template<typename Coeff>
UnivariatePolynomial<Coeff> resultant(const UnivariatePolynomial<Coeff>&, const UnivariatePolynomial<Coeff>&, SubresultantStrategy = SubresultantStrategy::Default);
template<typename Coeff>
UnivariatePolynomial<Coeff> discriminant(const UnivariatePolynomial<Coeff>&, SubresultantStrategy = SubresultantStrategy::Default);
}

#include "../UnivariatePolynomial.h"

namespace carl {

template<typename Coeff>
std::list<UnivariatePolynomial<Coeff>> subresultants(
		const UnivariatePolynomial<Coeff>& pol1,
		const UnivariatePolynomial<Coeff>& pol2,
		SubresultantStrategy strategy
) {
	/* The algorithm consists of three parts:
	 * Part 1: Initialization, i.e. preparation of the input so that the requirements of the core algorithm in parts 2 and 3 are met.
	 * Part 2: First part of the main loop. If the two subresultants which were added before (initially the two inputs) differ by more
	 *		 than 1 in their degree, an intermediate subresultant is computed by reducing the last one added with the leading coefficient
	 *		 of the one before this one.
	 * Part 3: Second part of the main loop. The pseudo remainder of the last two subresultants (the one possibly added in Part 2 disregarded)
	 *		 is computed and added to the subresultant sequence.
	 */

	/* Part 1
	 * Check and normalize input, initialize local variables.
	 */
	assert(pol1.mainVar() == pol2.mainVar());
	CARL_LOG_TRACE("carl.core.resultant", "subresultants(" << pol1 << ", " << pol2 << ")");
	std::list<UnivariatePolynomial<Coeff>> subresultants;
	Variable variable = pol1.mainVar();
	
	assert(!pol1.isZero());
	assert(!pol2.isZero());
	// We initialize p and q with pol1 and pol2.
	UnivariatePolynomial<Coeff> p(pol1), q(pol2);
	
	// pDeg >= qDeg shall hold, so switch if it does not hold
	if (p.degree() < q.degree()) {
		std::swap(p, q);
	}
	CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
	CARL_LOG_TRACE("carl.core.resultant", "q = " << q);
	
	subresultants.push_front(p);
	if (q.isZero()) {
		CARL_LOG_TRACE("carl.core.resultant", "q is Zero.");
		return subresultants;
	}
	subresultants.push_front(q);
	
	// SPECIAL CASE: both, p and q, are constant
	if (q.isConstant()) {
		CARL_LOG_TRACE("carl.core.resultant", "q is constant.");
		return subresultants;
	}
	
	// Explicitly check preconditions
	assert(p.degree() >= q.degree());
	assert(q.degree() >= 1);
	
	// BUG in Duco's article(?):
	//ex subresLcoeff = GiNaC::pow( a.lcoeff(), a.degree() - b.degree() );	// initialized on the basis of the smaller-degree polynomial
	//Coeff subresLcoeff(a.lcoeff()); // initialized on the basis of the smaller-degree polynomial
	Coeff subresLcoeff = q.lcoeff().pow(p.degree() - q.degree());
	CARL_LOG_TRACE("carl.core.resultant", "subresLcoeff = " << subresLcoeff);
	
	UnivariatePolynomial<Coeff> tmp = q;
	q = p.prem(-q);
	p = tmp;
	CARL_LOG_TRACE("carl.core.resultant", "q = p.prem(-q) = " << q);
	CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
	//CARL_LOG_TRACE("carl.core.resultant", "p = q");
	//CARL_LOG_TRACE("carl.core.resultant", "q = p.prem(-q)");
	
	/* Parts 2 and 3
	 * Main loop filling the subresultants chain step by step.
	 */
	// MAIN: start main loop containing different computation strategies
	while (true) {
		CARL_LOG_TRACE("carl.core.resultant", "Looping...");
		CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
		CARL_LOG_TRACE("carl.core.resultant", "q = " << q);
		if (q.isZero()) return subresultants;
		uint pDeg = p.degree();
		uint qDeg = q.degree();
		subresultants.push_front(q);
		
		// Part 2
		assert(pDeg >= qDeg);
		uint delta = pDeg - qDeg;
		CARL_LOG_TRACE("carl.core.resultant", "delta = " << delta);
		
		/** Case distinction on delta: either we choose b as next subresultant or we could reduce b (delta > 1)
		 * and add the reduced version c as next subresultant. The reduction is done by division, which
		 * depends on the internal variable order of GiNaC and might fail although for some order it would succeed.
		 * In this case, we just do not reduce b. (A relaxed reduction could also be applied.)
		 *
		 * After the if-else block, bDeg is the degree of the front-most element of subresultants, be it c or b.
		 */
		UnivariatePolynomial<Coeff> c(variable);
		if (delta > 1) {
			// compute c
			// Notation hints: Compared to [Duc98], here S_{d-1} is b and S_d is a, and S_e is c.
			switch (strategy) {
				case SubresultantStrategy::Generic: {
					CARL_LOG_TRACE("carl.core.resultant", "Part 2: Generic strategy");
					UnivariatePolynomial<Coeff> reductionCoeff = q.lcoeff().pow(delta - 1) * q;
					Coeff dividant = subresLcoeff.pow(delta-1);
					bool res = reductionCoeff.divideBy(dividant, c);
					if (res) {
						subresultants.push_front(c);
						assert(!c.isZero());
						qDeg = c.degree();
					} else {
						c = q;
					}
					break;
				}
				case SubresultantStrategy::Ducos:
				case SubresultantStrategy::Lazard: {
					CARL_LOG_TRACE("carl.core.resultant", "Part 2: Ducos/Lazard strategy");
					// "dichotomous Lazard": efficient exponentiation
					uint deltaReduced = delta-1;
					CARL_LOG_TRACE("carl.core.resultant", "deltaReduced = " << deltaReduced);
					// should be true by the loop condition
					assert(deltaReduced > 0);
					
					Coeff lcoeffQ = q.lcoeff();
					UnivariatePolynomial<Coeff> reductionCoeff(variable, lcoeffQ);
					
					CARL_LOG_TRACE("carl.core.resultant", "lcoeffQ = " << lcoeffQ);
					CARL_LOG_TRACE("carl.core.resultant", "reductionCoeff = " << reductionCoeff);
					
					uint exponent = highestPower(deltaReduced);
					deltaReduced -= exponent;
					CARL_LOG_TRACE("carl.core.resultant", "exponent = " << exponent);
					CARL_LOG_TRACE("carl.core.resultant", "deltaReduced = " << deltaReduced);
					
					while (exponent != 1) {
						exponent /= 2;
						CARL_LOG_TRACE("carl.core.resultant", "exponent = " << exponent);
						bool res = (reductionCoeff*reductionCoeff).divideBy(subresLcoeff, reductionCoeff);
						if (res && deltaReduced >= exponent) {
							(reductionCoeff*lcoeffQ).divideBy(subresLcoeff, reductionCoeff);
							deltaReduced -= exponent;
						}
					}
					CARL_LOG_TRACE("carl.core.resultant", "reductionCoeff = " << reductionCoeff);
					reductionCoeff *= q;
					CARL_LOG_TRACE("carl.core.resultant", "reductionCoeff = " << reductionCoeff);
					bool res = reductionCoeff.divideBy(subresLcoeff, c);
					if (res) {
						subresultants.push_front(c);
						assert(!c.isZero());
						qDeg = c.degree();
						CARL_LOG_TRACE("carl.core.resultant", "qDeg = " << qDeg);
					} else {
						c = q;
					}
					CARL_LOG_TRACE("carl.core.resultant", "c = " << c);
					break;
				}
			}
		} else {
			c = q;
		}
		if (qDeg == 0) return subresultants;
		
		CARL_LOG_TRACE("carl.core.resultant", "Mid");
		//CARL_LOG_TRACE("carl.core.resultant", "p = " << p);
		//CARL_LOG_TRACE("carl.core.resultant", "q = " << q);
		
		// Part 3
		switch (strategy) {
			// Compared to [Duc98], here S_{d-1} is b and S_d is a, S_e is c, and s_d is subresLcoeff.
			case SubresultantStrategy::Generic:
			case SubresultantStrategy::Lazard: {
				CARL_LOG_TRACE("carl.core.resultant", "Part 3: Generic/Lazard strategy");
				if (p.isZero()) return subresultants;
				
				/* If b was constant, the degree properties for subresultants are still met, enforcing us to disregard whether
				 * the above division was successful (in this case, reducedNewB remains unchanged).
				 * If it was successful, the resulting term is safely added to the list, yielding an optimized resultant.
				 */
				UnivariatePolynomial<Coeff> reducedNewB = p.prem(-q);
				bool r = reducedNewB.divideBy(subresLcoeff.pow(delta)*p.lcoeff(), q);
				assert(r);
				break;
			}
			case SubresultantStrategy::Ducos: {
				CARL_LOG_TRACE("carl.core.resultant", "Part 3: Ducos strategy");
				// Ducos' optimization
				Coeff lcoeffQ = q.lcoeff();
				Coeff lcoeffC = c.lcoeff();
				std::vector<Coeff> h(pDeg);
				
				for (uint d = 0; d < qDeg; d++) {
					h[d] = lcoeffC * Coeff(variable).pow(d);
				}
				if (pDeg != qDeg) { // => aDeg > bDeg
					h[qDeg] = Coeff(lcoeffC * Coeff(variable).pow(qDeg) - c); // H_e
				}
				for (uint d = qDeg + 1; d < pDeg; d++) {
					Coeff t = h[d-1] * variable;
					UnivariatePolynomial<Coeff> reducedNewB = t.toUnivariatePolynomial(variable).coefficients()[qDeg] * q;
					bool res = reducedNewB.divideBy(lcoeffQ, reducedNewB);
					assert(res || reducedNewB.isConstant());
					h[d] = Coeff(t - reducedNewB);
				}
				
				UnivariatePolynomial<Coeff> sum(p.mainVar(), h.front() * p.coefficients()[0]);
				for (uint d = 1; d < pDeg; d++) {
					sum += h[d] * p.coefficients()[d];
				}
				UnivariatePolynomial<Coeff> normalizedSum(p.mainVar());
				bool res = sum.divideBy(p.lcoeff(), normalizedSum);
				assert(res || sum.isConstant());
				
				UnivariatePolynomial<Coeff> t(variable, {0, h.back()});
				UnivariatePolynomial<Coeff> reducedNewB = ((t + normalizedSum) * lcoeffQ - t.coefficients()[qDeg].toUnivariatePolynomial(variable));
				reducedNewB.divideBy(p.lcoeff(), reducedNewB);
				if (delta % 2 == 0) {
					q = -reducedNewB;
				} else {
					q = reducedNewB;
				}
				break;
			}
		}
		p = c;
		subresLcoeff = p.lcoeff();
	}
}

template<typename Coeff>
std::vector<UnivariatePolynomial<Coeff>> principalSubresultantsCoefficients(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q,
		SubresultantStrategy strategy
) {
	// Attention: Mathematica / Wolframalpha has one entry less (the last one) which is identical to p!
	std::list<UnivariatePolynomial<Coeff>> subres = subresultants(p, q, strategy);
	CARL_LOG_DEBUG("carl.upoly", "PSC of " << p << " and " << q << " on " << p.mainVar() << ": " << subres);
	std::vector<UnivariatePolynomial<Coeff>> subresCoeffs;
	for (const auto& s: subres) {
		assert(!s.isZero());
		subresCoeffs.emplace_back(s.mainVar(), s.lcoeff());
	}
	return subresCoeffs;
}

template<typename Coeff>
UnivariatePolynomial<Coeff> resultant(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q,
		SubresultantStrategy strategy
) {
	assert(p.mainVar() == q.mainVar());
	if (p.isZero() || q.isZero()) return UnivariatePolynomial<Coeff>(p.mainVar());
	UnivariatePolynomial<Coeff> resultant = subresultants(p.normalized(), q.normalized(), strategy).front();
	CARL_LOG_TRACE("carl.core.resultant", "resultant(" << p << ", " << q << ") = " << resultant);
	if (resultant.isConstant()) {
		return resultant;
	} else {
		return UnivariatePolynomial<Coeff>(p.mainVar());
	}
}

template<typename Coeff>
UnivariatePolynomial<Coeff> discriminant(
	const UnivariatePolynomial<Coeff>& p,
	SubresultantStrategy strategy
) {
	UnivariatePolynomial<Coeff> res = resultant(p, p.derivative(), strategy);
	if (p.isLinearInMainVar()) return res;
	uint d = p.degree();
	Coeff sign = ((d*(d-1) / 2) % 2 == 0) ? Coeff(1) : Coeff(-1);
	Coeff redCoeff = sign * p.lcoeff();
	bool result = res.divideBy(redCoeff, res);
	assert(result);
	CARL_LOG_TRACE("carl.cad", "discriminant(" << p << ") = " << res);
	return res;
}

}

#pragma once

// UnivariatePolynomial::content() may need gcd().
#include "GCD.h"

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

/**
 * Implements a subresultants algorithm with optimizations described in @cite Ducos00 .
 * @param pol1 First polynomial.
 * @param pol2 First polynomial.
 * @param strategy Strategy.
 * @return Subresultants of pol1 and pol2.
 */
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

namespace resultant_debug {
	/**
	 * A reimplementation of the resultant algorithm from z3.
	 * Used for a comparative analysis of our own algorithm.
	 */
	template<typename Coeff>
	UnivariatePolynomial<Coeff> resultant_z3(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q
	) {
		//std::cout << "resultant(" << p << ", " << q << ", " << q.mainVar() << ")" << std::endl;
		if (p.isZero() || q.isZero()) {
			//std::cout << "Zero" << std::endl;
			return UnivariatePolynomial<Coeff>(q.mainVar());
		}

		if (p.isConstant()) {
			//std::cout << "A is const" << std::endl;
			if (q.isConstant()) {
				return UnivariatePolynomial<Coeff>(q.mainVar(), Coeff(1));
			} else {
				return p.pow(q.degree());
			}
		}
		if (q.isConstant()) {
			//std::cout << "B is const" << std::endl;
		   return q.pow(q.degree());
		}

		UnivariatePolynomial<Coeff> nA(q.normalized());
		UnivariatePolynomial<Coeff> nB(p.normalized());

		//std::cout << "Content" << std::endl;
		Coeff cA = nA.content();
		Coeff cB = nB.content();
		Coeff A(nA.primitivePart());
		Coeff B(nB.primitivePart());
		//std::cout << "Done" << std::endl;
		cA = carl::pow(cA, p.degree());
		cB = carl::pow(cB, q.degree());
		Coeff t = cA * cB;

		//std::cout << "Pre" << std::endl;
		int s = 1;
		std::size_t degA = A.degree(q.mainVar());
		std::size_t degB = B.degree(q.mainVar());
		if (degA < degB) {
			std::swap(A, B);
			if (degA % 2 == 1 && degB % 2 == 1) s = -1;
		}

		Coeff R;
		Coeff g(1);
		Coeff h(1);
		Coeff new_h;
		bool div_res = false;

		while (true) {
			//std::cout << "Loop " << A << ", " << B << std::endl;
			//std::cout << "A = " << A << ", B = " << B << std::endl;
			degA = A.degree(q.mainVar());
			degB = B.degree(q.mainVar());
			//std::cout << "Degrees: " << degA << " / " << degB << std::endl;
			assert(degA >= degB);
			std::size_t delta = degA - degB;
			//std::cout << "1: delta = " << delta << std::endl;
			if (degA % 2 == 1 && degB % 2 == 1) s = -s;
			R = A.prem(B, q.mainVar());
			A = B;
			//std::cout << "R = " << R << std::endl;
			//std::cout << "g = " << g << std::endl;
			div_res = R.divideBy(g, B);
			assert(div_res);
			for (std::size_t i = 0; i < delta; i++) {
				//std::cout << "i = " << i << std::endl;
				div_res = B.divideBy(h, B);
				assert(div_res);
			}
			g = A.lcoeff(q.mainVar());
			//std::cout << "2: delta = " << delta << std::endl;
			//std::cout << "g = " << g << std::endl;
			new_h = carl::pow(g, delta);
			//std::cout << "Pow done" << delta << std::endl;
			if (delta > 1) {
				for (std::size_t i = 0; i < delta - 1; i++) {
					div_res = new_h.divideBy(h, new_h);
					assert(div_res);
				}
			}
			h = new_h;
			if (B.degree(q.mainVar()) == 0) {
				std::size_t degA = A.degree(q.mainVar());
				new_h = B.lcoeff(q.mainVar());
				new_h = carl::pow(new_h, degA);
				if (degA > 1) {
					for (std::size_t i = 0; i < degA - 1; i++) {
						div_res = new_h.divideBy(h, new_h);
						assert(div_res);
					}
				}
				h = new_h;
				if (s < 0) return UnivariatePolynomial<Coeff>(q.mainVar(), -t * h);
				return UnivariatePolynomial<Coeff>(q.mainVar(), t * h);
			}
		}
	}

	
	/**
	 * Eliminates the leading factor of p with q.
	 */
	template<typename Coeff>
	UnivariatePolynomial<Coeff> eliminate(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q
	) {
		//std::cout << "eliminate " << p << " with " << q << std::endl;
		assert(p.mainVar() == q.mainVar());
		assert(p.degree() >= q.degree());
		std::size_t degdiff = p.degree() - q.degree();
		if (degdiff == 0) return p * q.lcoeff() - q * p.lcoeff();
		else return p * q.lcoeff() - q * p.lcoeff() * UnivariatePolynomial<Coeff>(p.mainVar(), Coeff(1), degdiff);
	}

	/**
	 * An implementation of the naive resultant algorithm based on the silvester matrix.
	 */
	template<typename Coeff>
	UnivariatePolynomial<Coeff> resultant_det(
		const UnivariatePolynomial<Coeff>& p,
		const UnivariatePolynomial<Coeff>& q
	) {
		if (p.isZero() || q.isZero()) {
			return UnivariatePolynomial<Coeff>(q.mainVar());
		}
		if (p.isConstant()) {
			if (q.isConstant()) {
				return UnivariatePolynomial<Coeff>(p.mainVar(), Coeff(1));
			} else {
				return p.pow(q.degree());
			}
		}
		if (q.isConstant()) {
		   return q.pow(p.degree());
		}
		if (p == q) return UnivariatePolynomial<Coeff>(p.mainVar());

		UnivariatePolynomial<Coeff> f = q;
		UnivariatePolynomial<Coeff> g = p;
		if (f.degree() > g.degree()) std::swap(f, g);

		// Three stages:
		//   1. Eliminate leading coefficients of all g-rows with f at once
		//      -> Until last g-row can be eliminated with last f-row
		//   2. Eliminate leading coefficients of g-rows with f while still possible
		//      -> Until no g-row can be eliminated with last f-row.
		//         Now, there is a deg(f)-square-matrix at the lower right.
		//   3. Eliminate the square matrix at the lower right.

		// Sylvester Matrix of f and g:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//   g3  g2  g1  g0
		//       g3  g2  g1  g0
		//
		// First we eliminate g3 with f, resulting in:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//       g2' g1' g0
		//           g2' g1' g0
		//
		// Now we eliminate g2', result looks like this:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//           g1* g0*
		//               g1* g0*
		//
		// And finally, we eliminate g3' from the first g-row:
		//   f2  f1  f0
		//       f2  f1  f0
		//           f2  f1  f0
		//               g0' t
		//               g1*  g0*
		//
		// Now, the first degree(f) columns are diagonal.
		//std::cout << "f = " << f << std::endl;
		//std::cout << "g = " << g << std::endl;
		UnivariatePolynomial<Coeff> gRest = g;
		// Do the elimination that works the same for all g-rows
		for (std::size_t i = 0; i < g.degree() - f.degree() + 1; i++) {
			//std::cout << gRest << " % " << f << std::endl;
			gRest = eliminate(gRest, f);
			//std::cout << "-> " << gRest << std::endl;
		}
		// Store result of first eliminations in matrix.
		//PolyMatrix<Coeff> matrix(f.degree() + g.degree(), f.mainVar());
		for (std::size_t i = 0; i < g.degree(); i++) {
			// Store f-rows
			//matrix.set(i, g.degree()-i-1, f);
		}
		// Finish eliminations of g-rows.
		std::vector<UnivariatePolynomial<Coeff>> m;
		m.resize(f.degree(), UnivariatePolynomial<Coeff>(f.mainVar()));
		//matrix.set(g.degree() + f.degree()-1, 0, gRest);
		m[f.degree()-1] = gRest;
		for (std::size_t i = 1; i < f.degree(); i++) {
			gRest = eliminate(gRest * g.mainVar(), f);
			//matrix.set(g.degree() + f.degree()-1-i, 0, gRest);
			m[f.degree()-1-i] = gRest;
		}
		//std::cout << m << std::endl;
		for (std::size_t i = 0; i < m.size()-1; i++) {
			for (std::size_t j = i+1; j < m.size(); j++) {
				m[j] = eliminate(m[j], m[i]);
			}
		}
		//std::cout << m << std::endl;

		Coeff determinant = f.lcoeff();//.pow(f.degree());
		//std::cout << "det = " << determinant << std::endl;
		for (std::size_t i = 0; i < m.size(); i++) {
			//std::cout << "   *= " << m[i].coefficients()[m.size()-1-i] << std::endl;
			if (m[i].degree() >= m.size()-1-i) {
				determinant *= m[i].coefficients()[m.size()-1-i];
			} else {
				determinant = Coeff(0);
			}
		}
		//determinant = m.back().tcoeff();
		determinant = determinant.coprimeCoefficients();
		//std::cout << "det = " << determinant << std::endl;

		return UnivariatePolynomial<Coeff>(f.mainVar(), determinant);
	}
}

}

#pragma once

#include "../../../core/UnivariatePolynomial.h"

#include "../../../interval/Interval.h"

#include <list>

namespace carl {
namespace ran {
  /**
   * FIX isn't this the standard representation of a real algebraic number?
   */
	template<typename Number>
	struct IntervalContent {
		using Polynomial = UnivariatePolynomial<Number>;
		
		static const Variable auxVariable;
		
		Polynomial polynomial;
		Interval<Number> interval;
		std::list<Polynomial> sturmSequence;
		std::size_t refinementCount;
		
		Polynomial replaceVariable(const Polynomial& p) const {
			return p.replaceVariable(auxVariable);
		}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i
		):
			polynomial(replaceVariable(p)),
			interval(i),
			sturmSequence(p.standardSturmSequence()),
			refinementCount(0)
		{}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i,
			const std::list<UnivariatePolynomial<Number>>& seq
		):
			polynomial(replaceVariable(p)),
			interval(i),
			sturmSequence(seq),
			refinementCount(0)
		{}
		bool isIntegral() {
			return interval.isPointInterval() && carl::isInteger(interval.lower());
		}
		
		Sign sgn(const Polynomial& p) const {
			Polynomial tmp = replaceVariable(p);
			if (polynomial == tmp) return Sign::ZERO;
			auto seq = polynomial.standardSturmSequence(polynomial.derivative() * tmp);
			int variations = Polynomial::countRealRoots(seq, interval);
			assert((variations == -1) || (variations == 0) || (variations == 1));
			switch (variations) {
				case -1: return Sign::NEGATIVE;
				case 0: return Sign::ZERO;
				case 1: return Sign::POSITIVE;
			}
			CARL_LOG_ERROR("carl.ran", "Unexpected number of variations, should be -1, 0, 1 but was " << variations);
			return Sign::ZERO;
		}
		
		void refine() {
			Number pivot = interval.sample();
			assert(interval.contains(pivot));
			if (polynomial.isRoot(pivot)) {
				interval = Interval<Number>(pivot, pivot);
			} else {
				if (polynomial.countRealRoots(Interval<Number>(interval.lower(), BoundType::STRICT, pivot, BoundType::STRICT)) > 0) {
					interval.setUpper(pivot);
				} else {
					interval.setLower(pivot);
				}
				refinementCount++;
				assert(interval.isConsistent());
			}
		}
			
		/** Refine the interval i of this real algebraic number yielding the interval j such that !j.meets(n). If true is returned, n is the exact numeric representation of this root. Otherwise not.
		 * @param n
		 * @rcomplexity constant
		 * @scomplexity constant
		 * @return true, if n is the exact numeric representation of this root, otherwise false
		 */
		bool refineAvoiding(const Number& n) {
			if (interval.contains(n)) {
				if (polynomial.isRoot(n)) {
					interval = Interval<Number>(n, n);
					return true;
				}
				if (polynomial.countRealRoots(Interval<Number>(interval.lower(), BoundType::STRICT, n, BoundType::STRICT)) > 0) {
					interval.setUpper(n);
				} else {
					interval.setLower(n);
				}
				refinementCount++;
			} else if (interval.lower() != n && interval.upper() != n) {
				return false;
			}
			
			bool isLeft = interval.lower() == n;
			
			Number newBound = interval.sample();
			
			if (polynomial.isRoot(newBound)) {
				interval = Interval<Number>(newBound, newBound);
				return false;
			}
			
			if (isLeft) {
				interval.setLower(newBound);
			} else {
				interval.setUpper(newBound);
			}
			
			while (polynomial.countRealRoots(interval) == 0) {
				if (isLeft) {
					Number oldBound = interval.lower();
					newBound = Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT).sample();
					if (polynomial.isRoot(newBound)) {
						interval = Interval<Number>(newBound, newBound);
						return false;
					}
					interval.setUpper(oldBound);
					interval.setLower(newBound);
				} else {
					Number oldBound = interval.upper();
					newBound = Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT).sample();
					if (polynomial.isRoot(newBound)) {
						interval = Interval<Number>(newBound, newBound);
						return false;
					}
					interval.setLower(oldBound);
					interval.setUpper(newBound);
				}
			}
			return false;
		}
		
		void refineToIntegrality() {
			while (!interval.isPointInterval() && interval.containsInteger()) {
				refine();
			}
		}
	};

	template<typename Number>
	const Variable IntervalContent<Number>::auxVariable = freshRealVariable("__r");
}
}

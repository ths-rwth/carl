#pragma once

#include "../../../core/UnivariatePolynomial.h"
#include "../../../core/polynomialfunctions/RootCounting.h"
#include "../../../core/polynomialfunctions/SturmSequence.h"

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
		std::vector<Polynomial> sturmSequence;
		std::size_t refinementCount;
		
		Polynomial replaceVariable(const Polynomial& p) const {
			return p.replaceVariable(auxVariable);
		}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i
		): IntervalContent(p, i, carl::sturm_sequence(p))
		{}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i,
			const std::vector<UnivariatePolynomial<Number>>& seq
		):
			polynomial(replaceVariable(p)),
			interval(i),
			sturmSequence(seq),
			refinementCount(0)
		{
			assert(!carl::isZero(polynomial) && polynomial.degree() > 0);
			assert(interval.isOpenInterval() || interval.isPointInterval());
			assert(count_real_roots(polynomial, interval) == 1);
			if (polynomial.degree() == 1) {
				Number a = polynomial.coefficients()[1];
				Number b = polynomial.coefficients()[0];
				interval = Interval<Number>(Number(-b / a));
			} else {
				if (interval.contains(0)) refineAvoiding(0);
				refineToIntegrality();
			}
		}

		bool is_zero() const {
			return carl::isZero(interval);
		}
		bool is_number() const {
			return interval.isPointInterval();
		}
		const auto& get_number() const {
			assert(is_number());
			return interval.lower();
		}
		bool is_integral() const {
			return interval.isPointInterval() && carl::isInteger(interval.lower());
		}
		Number integer_below() const {
			return carl::floor(interval.lower());
		}

		std::size_t size() const {
			return carl::bitsize(interval.lower()) + carl::bitsize(interval.upper()) + polynomial.degree();
		}

		bool is_root_of(const UnivariatePolynomial<Number>& p) const {
			return carl::count_real_roots(p, interval) == 1;
		}

		bool isIntegral() {
			return interval.isPointInterval() && carl::isInteger(interval.lower());
		}
		
		void setPolynomial(const Polynomial& p) {
			polynomial = replaceVariable(p);
			sturmSequence = carl::sturm_sequence(polynomial);
		}

		Sign sgn() const {
			return interval.sgn();
		}
		
		Sign sgn(const Polynomial& p) const {
			Polynomial tmp = replaceVariable(p);
			if (polynomial == tmp) return Sign::ZERO;
			auto seq = carl::sturm_sequence(polynomial, derivative(polynomial) * tmp);
			int variations = carl::count_real_roots(seq, interval);
			assert((variations == -1) || (variations == 0) || (variations == 1));
			switch (variations) {
				case -1: return Sign::NEGATIVE;
				case 0: return Sign::ZERO;
				case 1: return Sign::POSITIVE;
				default:
					CARL_LOG_ERROR("carl.ran", "Unexpected number of variations, should be -1, 0, 1 but was " << variations);
					return Sign::ZERO;
			}
		}

		bool containedIn(const Interval<Number>& i) {
			if (interval.contains(i.lower())) {
				refineAvoiding(i.lower());
			}
			if (interval.contains(i.upper())) {
				refineAvoiding(i.upper());
			}
			return i.contains(interval);
		}
		
		void refine() {
			Number pivot = carl::sample(interval);
			assert(interval.contains(pivot));
			if (polynomial.isRoot(pivot)) {
				interval = Interval<Number>(pivot, pivot);
			} else {
				if (carl::count_real_roots(polynomial, Interval<Number>(interval.lower(), BoundType::STRICT, pivot, BoundType::STRICT)) > 0) {
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
				if (carl::count_real_roots(polynomial, Interval<Number>(interval.lower(), BoundType::STRICT, n, BoundType::STRICT)) > 0) {
					interval.setUpper(n);
				} else {
					interval.setLower(n);
				}
				refinementCount++;
			} else if (interval.lower() != n && interval.upper() != n) {
				return false;
			}
			
			bool isLeft = interval.lower() == n;
			
			Number newBound = carl::sample(interval);
			
			if (polynomial.isRoot(newBound)) {
				interval = Interval<Number>(newBound, newBound);
				return false;
			}
			
			if (isLeft) {
				interval.setLower(newBound);
			} else {
				interval.setUpper(newBound);
			}
			
			while (carl::count_real_roots(polynomial, interval) == 0) {
				if (isLeft) {
					Number oldBound = interval.lower();
					newBound = carl::sample(Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT));
					if (polynomial.isRoot(newBound)) {
						interval = Interval<Number>(newBound, newBound);
						return false;
					}
					interval.setUpper(oldBound);
					interval.setLower(newBound);
				} else {
					Number oldBound = interval.upper();
					newBound = carl::sample(Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT));
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
bool operator==(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	if (lhs.interval.upper() < rhs.interval.lower()) return false;
	if (lhs.interval.lower() > rhs.interval.upper()) return false;
	if (lhs.polynomial == rhs.polynomial) {
		if (lhs.interval.lower() <= rhs.interval.lower()) {
			if (rhs.interval.upper() <= lhs.interval.upper()) return true;
			lhs.refineAvoiding(rhs.interval.lower());
			rhs.refineAvoiding(lhs.interval.upper());
		} else {
			assert(rhs.interval.lower() <= lhs.interval.lower());
			if (lhs.interval.upper() <= rhs.interval.upper()) return true;
			lhs.refineAvoiding(rhs.interval.upper());
			rhs.refineAvoiding(lhs.interval.lower());
		}
	} else {
		assert(lhs.polynomial != rhs.polynomial);
		assert(lhs.polynomial.mainVar() == rhs.polynomial.mainVar());
		auto g = carl::gcd(lhs.polynomial, rhs.polynomial);
		if (carl::isOne(g)) return false;
		if (lhs.is_root_of(g)) {
			lhs.setPolynomial(g);
		} else {
			assert(carl::isZero(lhs.polynomial.divideBy(g).remainder));
			lhs.setPolynomial(lhs.polynomial.divideBy(g).quotient);
		}
		if (rhs.is_root_of(g)) {
			rhs.setPolynomial(g);
		} else {
			assert(carl::isZero(rhs.polynomial.divideBy(g).remainder));
			rhs.setPolynomial(rhs.polynomial.divideBy(g).quotient);
		}
	}
	return lhs == rhs;
}

template<typename Number>
bool operator<(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	if (lhs == rhs) return false;
	while (true) {
		if (lhs.interval.upper() < rhs.interval.lower()) return true;
		if (lhs.interval.lower() > rhs.interval.upper()) return false;
		lhs.refineAvoiding(rhs.interval.lower());
		lhs.refineAvoiding(rhs.interval.upper());
		rhs.refineAvoiding(lhs.interval.lower());
		rhs.refineAvoiding(lhs.interval.upper());
	}
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const IntervalContent<Num>& ran) {
	return os << "IR " << ran.interval << ", " << ran.polynomial;
}

	template<typename Number>
	const Variable IntervalContent<Number>::auxVariable = freshRealVariable("__r");
}
}

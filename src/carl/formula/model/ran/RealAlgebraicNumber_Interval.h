#pragma once

#include "../../../core/UnivariatePolynomial.h"
#include "../../../core/polynomialfunctions/Resultant.h"
#include "../../../core/polynomialfunctions/RootCounting.h"
#include "../../../core/polynomialfunctions/SquareFreePart.h"
#include "../../../core/polynomialfunctions/SturmSequence.h"

#include "../../../interval/Interval.h"
#include "../../../interval/IntervalEvaluation.h"

#include <list>

namespace carl {
namespace ran {
	template<typename Number>
	struct IntervalContent {
		using Polynomial = UnivariatePolynomial<Number>;
		static const Variable auxVariable;

		template<typename Num>
		friend bool operator==(IntervalContent<Num>& lhs, IntervalContent<Num>& rhs);

	private:
		struct Content {
			Polynomial polynomial;
			Interval<Number> interval;
			std::vector<Polynomial> sturmSequence;
			std::size_t refinementCount = 0;

			Content(Polynomial&& p, const Interval<Number>& i, std::vector<UnivariatePolynomial<Number>>&& seq):
				polynomial(std::move(p)), interval(i), sturmSequence(std::move(seq))
			{
				assert(polynomial == carl::squareFreePart(polynomial));
			}
			Content(const Polynomial& p, const Interval<Number>& i):
				polynomial(carl::squareFreePart(p)), interval(i), sturmSequence(carl::sturm_sequence(polynomial))
			{}
		};

		mutable std::shared_ptr<Content> mContent;

		static Polynomial replaceVariable(const Polynomial& p) {
			return p.replaceVariable(auxVariable);
		}
	public:

		IntervalContent():
			IntervalContent(Polynomial(auxVariable, {0, 1}), Interval<Number>(0))
		{
			assert(is_consistent());
		}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i
		):
			mContent(std::make_shared<Content>(replaceVariable(p), i))
		{
			CARL_LOG_DEBUG("carl.ran.ir", "Creating " << *this);
			assert(!carl::isZero(polynomial()) && polynomial().degree() > 0);
			assert(interval().isOpenInterval() || interval().isPointInterval());
			assert(interval().isPointInterval() || count_real_roots(polynomial(), interval()) == 1);
			assert(is_consistent());
			if (polynomial().degree() == 1) {
				Number a = polynomial().coefficients()[1];
				Number b = polynomial().coefficients()[0];
				interval() = Interval<Number>(Number(-b / a));
			} else {
				if (interval().contains(0)) refineAvoiding(0);
				refineToIntegrality();
			}
		}

		bool is_consistent() const {
			if (interval().isPointInterval()) {
				return polynomial().isRoot(interval().lower());
			} else {
				if (polynomial().normalized() != carl::squareFreePart(polynomial()).normalized()) {
					CARL_LOG_DEBUG("carl.ran.ir", "Poly is not square free: " << polynomial());
					return false;
				}
				auto lsgn = polynomial().sgn(interval().lower());
				auto usgn = polynomial().sgn(interval().upper());
				if (lsgn == Sign::ZERO || usgn == Sign::ZERO || lsgn == usgn) {
					CARL_LOG_DEBUG("carl.ran.ir", "Interval does not define a zero");
					return false;
				}
				return true;
			}
		}

		auto& polynomial() const {
			return mContent->polynomial;
		}
		auto& interval() const {
			return mContent->interval;
		}
		auto& sturm_sequence() const {
			return mContent->sturmSequence;
		}
		auto& refinementCount() const {
			return mContent->refinementCount;
		}

		bool is_zero() const {
			return carl::isZero(interval());
		}
		bool is_integral() const {
			return interval().isPointInterval() && carl::isInteger(interval().lower());
		}
		Number integer_below() const {
			return carl::floor(interval().lower());
		}

		std::size_t size() const {
			return carl::bitsize(interval().lower()) + carl::bitsize(interval().upper()) + polynomial().degree();
		}

		bool is_root_of(const UnivariatePolynomial<Number>& p) const {
			if (interval().isPointInterval())
				return p.isRoot(interval().lower());
			else
				return carl::count_real_roots(p, interval()) == 1; // TODO in general incorrect: take carl::gcd(p,polynomial())
		}

		bool isIntegral() {
			return interval().isPointInterval() && carl::isInteger(interval().lower());
		}
		
		void setPolynomial(const Polynomial& p) const {
			polynomial() = replaceVariable(p);
			sturm_sequence() = carl::sturm_sequence(polynomial());
			assert(is_consistent());
		}

		Sign sgn() const {
			return interval().sgn();
		}
		
		Sign sgn(const Polynomial& p) const {
			Polynomial tmp = replaceVariable(p);
			if (polynomial() == tmp) return Sign::ZERO;
			auto seq = carl::sturm_sequence(polynomial(), derivative(polynomial()) * tmp);
			int variations = carl::count_real_roots(seq, interval());
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

		bool contained_in(const Interval<Number>& i) {
			if (interval().contains(i.lower())) {
				refineAvoiding(i.lower());
			}
			if (interval().contains(i.upper())) {
				refineAvoiding(i.upper());
			}
			return i.contains(interval());
		}
		
		void refine(bool newone = true) const {
			Number pivot = carl::sample(interval());
			assert(interval().contains(pivot));
			if (newone) {
				// assert(is_consistent());
				auto psgn = polynomial().sgn(pivot);
				if (psgn == Sign::ZERO) {
					interval() = Interval<Number>(pivot, pivot);
					return;
				}
				auto lsgn = polynomial().sgn(interval().lower());
				if (psgn == lsgn) {
					interval().setLower(pivot);
				} else {
					interval().setUpper(pivot);
				}
			} else {
				if (polynomial().isRoot(pivot)) {
					interval() = Interval<Number>(pivot, pivot);
					return;
				}
				if (carl::count_real_roots(sturm_sequence(), Interval<Number>(interval().lower(), BoundType::STRICT, pivot, BoundType::STRICT)) > 0) {
					interval().setUpper(pivot);
				} else {
					interval().setLower(pivot);
				}
			}
			refinementCount()++;
			assert(interval().isConsistent());
		}
			
		/** Refine the interval i of this real algebraic number yielding the interval j such that !j.meets(n). If true is returned, n is the exact numeric representation of this root. Otherwise not.
		 * @param n
		 * @rcomplexity constant
		 * @scomplexity constant
		 * @return true, if n is the exact numeric representation of this root, otherwise false
		 */
		bool refineAvoiding(const Number& n, bool newone = true) const {
			if (newone) {
				// assert(is_consistent());
				if (interval().contains(n)) {
					auto psgn = carl::sgn(polynomial().evaluate(n));
					if (psgn == Sign::ZERO) {
						interval() = Interval<Number>(n, n);
						return true;
					}
					auto lsgn = carl::sgn(polynomial().evaluate(interval().lower()));
					if (psgn == lsgn) {
						interval().setLower(n);
					} else {
						interval().setUpper(n);
					}
					refinementCount()++;
				} else if (interval().lower() != n && interval().upper() != n) {
					return false;
				}
				
				bool isLeft = interval().lower() == n;
				
				Number newBound = carl::sample(interval());
				
				if (polynomial().isRoot(newBound)) {
					interval() = Interval<Number>(newBound, newBound);
					return false;
				}
				
				if (isLeft) {
					interval().setLower(newBound);
				} else {
					interval().setUpper(newBound);
				}
				
				while (polynomial().sgn(interval().lower()) == polynomial().sgn(interval().upper())) { // root is not in interval
					if (isLeft) {
						Number oldBound = interval().lower();
						newBound = carl::sample(Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT));
						if (polynomial().isRoot(newBound)) {
							interval() = Interval<Number>(newBound, newBound);
							return false;
						}
						interval().setUpper(oldBound);
						interval().setLower(newBound);
					} else {
						Number oldBound = interval().upper();
						newBound = carl::sample(Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT));
						if (polynomial().isRoot(newBound)) {
							interval() = Interval<Number>(newBound, newBound);
							return false;
						}
						interval().setLower(oldBound);
						interval().setUpper(newBound);
					}
				}
				return false;
			} else {
				if (interval().contains(n)) {
					if (polynomial().isRoot(n)) {
						interval() = Interval<Number>(n, n);
						return true;
					}
					if (carl::count_real_roots(sturm_sequence(), Interval<Number>(interval().lower(), BoundType::STRICT, n, BoundType::STRICT)) > 0) {
						interval().setUpper(n);
					} else {
						interval().setLower(n);
					}
					refinementCount()++;
				} else if (interval().lower() != n && interval().upper() != n) {
					return false;
				}
				
				bool isLeft = interval().lower() == n;
				
				Number newBound = carl::sample(interval());
				
				if (polynomial().isRoot(newBound)) {
					interval() = Interval<Number>(newBound, newBound);
					return false;
				}
				
				if (isLeft) {
					interval().setLower(newBound);
				} else {
					interval().setUpper(newBound);
				}
				
				while (carl::count_real_roots(sturm_sequence(), interval()) == 0) {
					if (isLeft) {
						Number oldBound = interval().lower();
						newBound = carl::sample(Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT));
						if (polynomial().isRoot(newBound)) {
							interval() = Interval<Number>(newBound, newBound);
							return false;
						}
						interval().setUpper(oldBound);
						interval().setLower(newBound);
					} else {
						Number oldBound = interval().upper();
						newBound = carl::sample(Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT));
						if (polynomial().isRoot(newBound)) {
							interval() = Interval<Number>(newBound, newBound);
							return false;
						}
						interval().setLower(oldBound);
						interval().setUpper(newBound);
					}
				}
				return false;
			}
		}
		
		void refineToIntegrality() {
			while (!interval().isPointInterval() && interval().containsInteger()) {
				refine();
			}
		}


		void simplifyByPolynomial(Variable var, const MultivariatePolynomial<Number>& poly) const {
			UnivariatePolynomial<Number> irp(var, polynomial().template convert<Number>().coefficients());
			CARL_LOG_DEBUG("carl.ran", "gcd(" << irp << ", " << poly << ")");
			auto gmv = carl::gcd(MultivariatePolynomial<Number>(irp), poly);
			CARL_LOG_DEBUG("carl.ran", "Simplyfing, gcd = " << gmv);
			if (carl::isOne(gmv)) return;
			auto g = gmv.toUnivariatePolynomial();
			if (is_root_of(g)) {
				CARL_LOG_DEBUG("carl.ran", "Is a root of " << g);
				setPolynomial(g);
			} else {
				CARL_LOG_DEBUG("carl.ran", "Is not a root of " << g);
				CARL_LOG_DEBUG("carl.ran", "Dividing " << polynomial() << " by " << g);
				setPolynomial(polynomial().divideBy(g.replaceVariable(auxVariable)).quotient);
			}
		}
	};

template<typename Number>
IntervalContent<Number> abs(const IntervalContent<Number>& n) {
	n.refineAvoiding(constant_zero<Number>::get());
	if (n.interval().isPositive()) return n;
	return IntervalContent<Number>(n.polynomial().negateVariable(), n.interval().abs());
}

template<typename Number>
const Number& branching_point(const IntervalContent<Number>& n) {
	return carl::sample(n.interval());
}

template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluatePolynomial(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, IntervalContent<Number>>& m,
		std::map<Variable, Interval<Number>>& varToInterval
) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	Variable v = p.mainVar();
	UnivariatePolynomial<Coeff> tmp = p;
	for (const auto& i: m) {
		if (!tmp.has(i.first)) {
			if (p.has(i.first)) {
				CARL_LOG_DEBUG("carl.ran", i.first << " vanished from " << tmp << " but was present in " << p);
				if (!is_number(i.second)) {
					// Variable vanished, add it to varToInterval
					varToInterval[i.first] = i.second.interval();
				}
			}
			continue;
		}
		if (is_number(i.second)) {
			CARL_LOG_DEBUG("carl.ran", "Direct substitution: " << i.first << " = " << i.second);
			tmp.substituteIn(i.first, Coeff(get_number(i.second)));
		} else {
			CARL_LOG_DEBUG("carl.ran", "IR substitution: " << i.first << " = " << i.second);
			i.second.simplifyByPolynomial(i.first, MultivariatePolynomial<Number>(tmp));
			UnivariatePolynomial<Coeff> p2(i.first, i.second.polynomial().template convert<Coeff>().coefficients());
			CARL_LOG_DEBUG("carl.ran", "Simplifying " << tmp.switchVariable(i.first) << " with " << p2);
			tmp = tmp.switchVariable(i.first).prem(p2);
			CARL_LOG_DEBUG("carl.ran", "Using " << p2 << " with " << tmp);
			tmp = carl::resultant(tmp, p2);
			CARL_LOG_DEBUG("carl.ran", "-> " << tmp);
			varToInterval[i.first] = i.second.interval();
		}
		CARL_LOG_DEBUG("carl.ran", "Substituted " << i.first << " -> " << i.second << ", result: " << tmp);
	}
	CARL_LOG_DEBUG("carl.ran", "Result: " << tmp.switchVariable(v).toNumberCoefficients());
	return tmp.switchVariable(v).toNumberCoefficients();
}

/**
 * Evaluate the given polynomial with the given values for the variables.
 * Asserts that all variables of p have an assignment in m and that m has no additional assignments.
 *
 * @param p Polynomial to be evaluated
 * @param m Variable assignment
 * @return Evaluation result
 */
template<typename Number>
IntervalContent<Number> evaluate(const MultivariatePolynomial<Number>& p, const std::map<Variable, IntervalContent<Number>>& m) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	assert(m.size() > 0);
	auto poly = p.toUnivariatePolynomial(m.begin()->first);
	if (m.size() == 1 && m.begin()->second.sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
		CARL_LOG_DEBUG("carl.ran", "Returning " << IntervalContent<Number>());
		return IntervalContent<Number>();
	}
	Variable v = freshRealVariable();
	// compute the result polynomial and the initial result interval
	std::map<Variable, Interval<Number>> varToInterval;
	UnivariatePolynomial<Number> res = evaluatePolynomial(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), m, varToInterval);
	assert(!varToInterval.empty());
	poly = p.toUnivariatePolynomial(varToInterval.begin()->first);
	CARL_LOG_DEBUG("carl.ran", "res = " << res);
	CARL_LOG_DEBUG("carl.ran", "varToInterval = " << varToInterval);
	CARL_LOG_DEBUG("carl.ran", "poly = " << poly);
	Interval<Number> interval = IntervalEvaluation::evaluate(poly, varToInterval);
	CARL_LOG_DEBUG("carl.ran", "-> " << interval);

	auto sturmSeq = sturm_sequence(res);
	// the interval should include at least one root.
	assert(!carl::isZero(res));
	assert(
		res.sgn(interval.lower()) == Sign::ZERO ||
		res.sgn(interval.upper()) == Sign::ZERO ||
		count_real_roots(sturmSeq, interval) >= 1
	);
	while (
		res.sgn(interval.lower()) == Sign::ZERO ||
		res.sgn(interval.upper()) == Sign::ZERO ||
		count_real_roots(sturmSeq, interval) != 1) 
	{
		// refine the result interval until it isolates exactly one real root of the result polynomial
		for (auto it = m.begin(); it != m.end(); it++) {
			it->second.refine();
			if (is_number(it->second)) {
				return evaluate(p, m);
			} else {
				varToInterval[it->first] = it->second.interval();
			}
		}
		interval = IntervalEvaluation::evaluate(poly, varToInterval);
	}
	CARL_LOG_DEBUG("carl.ran", "Result is " << IntervalContent<Number>(res, interval));
	return IntervalContent<Number>(res, interval);
}

template<typename Number, typename Poly>
bool evaluate(const Constraint<Poly>& c, const std::map<Variable, IntervalContent<Number>>& m, bool use_intervals = false) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << c << " on " << m);
	Poly p = c.lhs();

	if (use_intervals) { // TODO needs some profound considerations ...
		CARL_LOG_DEBUG("carl.ran", "Evaluate constraint using interval arithmetic");
		static Number min_width = Number(1)/(Number(65536)); // (1/2)^16, taken from Z3
		while(true) {
			// evaluate
			std::map<Variable, Interval<Number>> varToInterval;
			for (const auto& var : p.gatherVariables()) {
				varToInterval[var] = m.at(var).interval();
			}
			
			CARL_LOG_DEBUG("carl.ran",  "Evaluate " << p << " on " << varToInterval);
			auto res = IntervalEvaluation::evaluate(p, varToInterval);
			CARL_LOG_DEBUG("carl.ran",  "Interval evaluation obtained " << res);

			if (res.isPositive()) {
				CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
				return carl::evaluate(Sign::POSITIVE, c.relation());
			} else if (res.isNegative()) {
				CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
				return carl::evaluate(Sign::NEGATIVE, c.relation());
			} else if (res.isZero()) {
				CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
				return carl::evaluate(Sign::ZERO, c.relation());
			}
		
			// refine RANs
			bool refined = false;
			for (const auto& a : varToInterval) {
				if (a.second.diameter() > min_width) {
					if (p.has(a.first)) { // is var still in p?
						CARL_LOG_DEBUG("carl.ran", "Refine " <<  m.at(a.first) << " (" << a.first << ")");
						m.at(a.first).refine();
						// if RAN converted to a number, plug it in
						if (is_number(m.at(a.first))) {
							CARL_LOG_DEBUG("carl.ran",  m.at(a.first) << " (" << a.first << ") simplified to a number, plugging in");
							p.substituteIn(a.first, Poly(get_number(m.at(a.first))));
						}
						refined = true;
					}				
				}
			}
			if (!refined) {
				CARL_LOG_DEBUG("carl.ran", "Nothing to refine");
				break; // nothing to refine
			}

			// if all variables are substituted we're done
			if (p.gatherVariables().size() == 0) {
				CARL_LOG_DEBUG("carl.ran", c << " simplified to " << p << " ~ 0");
				assert(p.isNumber());
				return carl::evaluate(p.constantPart(), c.relation());
			}
		}
	}

	CARL_LOG_DEBUG("carl.ran", "Evaluate constraint by evaluating poly");
	IntervalContent<Number> res = evaluate(p, m);
	return evaluate(res.sgn(), c.relation());
}

template<typename Number>
const Interval<Number>& get_interval(const IntervalContent<Number>& n) {
	return n.interval();
}

template<typename Number>
const auto& get_number(const IntervalContent<Number>& n) {
	assert(is_number(n));
	return n.interval().lower();
}

template<typename Number>
const UnivariatePolynomial<Number>& get_polynomial(const IntervalContent<Number>& n) {
	return n.polynomial();
}

template<typename Number>
bool is_number(const IntervalContent<Number>& n) {
	return n.interval().isPointInterval();
}

template<typename Number>
Number sample_above(const IntervalContent<Number>& n) {
	return carl::floor(n.interval().upper()) + 1;
}
template<typename Number>
Number sample_below(const IntervalContent<Number>& n) {
	return carl::ceil(n.interval().lower()) - 1;
}
template<typename Number>
Number sample_between(IntervalContent<Number>& lower, IntervalContent<Number>& upper) {
	lower.refineAvoiding(upper.interval().lower());
	upper.refineAvoiding(lower.interval().upper());
	while (lower.interval().upper() >= upper.interval().lower()) {
		lower.refine();
		upper.refine();
	}
	return sample_between(NumberContent<Number>(lower.interval().upper()), NumberContent<Number>(upper.interval().lower()));
}
template<typename Number>
Number sample_between(IntervalContent<Number>& lower, const NumberContent<Number>& upper) {
	lower.refineAvoiding(upper.value());
	while (lower.interval().upper() >= upper.value()) {
		lower.refine();
	}
	return sample_between(NumberContent<Number>(lower.interval().upper()), upper);
}
template<typename Number>
Number sample_between(const NumberContent<Number>& lower, IntervalContent<Number>& upper) {
	upper.refineAvoiding(lower.value());
	while (lower.value() >= upper.interval().lower()) {
		upper.refine();
	}
	return sample_between(lower, NumberContent<Number>(upper.interval().lower()));
}

template<typename Number>
bool operator==(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	if (lhs.mContent.get() == rhs.mContent.get()) return true;
	if (lhs.interval().upper() < rhs.interval().lower()) return false;
	if (lhs.interval().lower() > rhs.interval().upper()) return false;
	if (lhs.interval().isPointInterval() && lhs.interval() == rhs.interval()) return true;
	if (lhs.polynomial() == rhs.polynomial()) {
		if (lhs.interval().lower() <= rhs.interval().lower()) {
			if (rhs.interval().upper() <= lhs.interval().upper()) {
				rhs = lhs;
				return true;
			}
			lhs.refineAvoiding(rhs.interval().lower());
			rhs.refineAvoiding(lhs.interval().upper());
		} else {
			assert(rhs.interval().lower() <= lhs.interval().lower());
			if (lhs.interval().upper() <= rhs.interval().upper()) {
				rhs = lhs;
				return true;
			}
			lhs.refineAvoiding(rhs.interval().upper());
			rhs.refineAvoiding(lhs.interval().lower());
		}
	} else {
		assert(lhs.polynomial() != rhs.polynomial());
		assert(lhs.polynomial().mainVar() == rhs.polynomial().mainVar());
		auto g = carl::gcd(lhs.polynomial(), rhs.polynomial());
		if (carl::isOne(g)) return false;
		if (lhs.is_root_of(g)) {
			lhs.setPolynomial(g);
		} else {
			assert(carl::isZero(lhs.polynomial().divideBy(g).remainder));
			lhs.setPolynomial(lhs.polynomial().divideBy(g).quotient);
		}
		if (rhs.is_root_of(g)) {
			rhs.setPolynomial(g);
		} else {
			assert(carl::isZero(rhs.polynomial().divideBy(g).remainder));
			rhs.setPolynomial(rhs.polynomial().divideBy(g).quotient);
		}
	}
	return lhs == rhs;
}

template<typename Number>
bool operator==(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return lhs.refineAvoiding(rhs.value());
}

template<typename Number>
bool operator==(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return rhs == lhs;
}

template<typename Number>
bool operator<(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	if (lhs == rhs) return false;
	while (true) {
		if (lhs.interval().upper() < rhs.interval().lower()) return true;
		if (lhs.interval().lower() > rhs.interval().upper()) return false;
		lhs.refineAvoiding(rhs.interval().lower());
		lhs.refineAvoiding(rhs.interval().upper());
		rhs.refineAvoiding(lhs.interval().lower());
		rhs.refineAvoiding(lhs.interval().upper());
	}
}

template<typename Number>
bool operator<(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	if (lhs.refineAvoiding(rhs.value())) return false;
		return lhs.interval().upper() < rhs.value();
}

template<typename Number>
bool operator<(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	if (rhs.refineAvoiding(lhs.value())) return false;
	return lhs.value() < rhs.interval().lower();
}

template<typename Num>
std::ostream& operator<<(std::ostream& os, const IntervalContent<Num>& ran) {
	return os << "IR " << ran.interval() << ", " << ran.polynomial();
}

template<typename Number>
const Variable IntervalContent<Number>::auxVariable = freshRealVariable("__r");

}
}

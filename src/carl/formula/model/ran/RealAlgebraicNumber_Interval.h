#pragma once

#include "../../../core/UnivariatePolynomial.h"
#include "../../../core/polynomialfunctions/AlgebraicSubstitution.h"
#include "../../../core/polynomialfunctions/Division.h"
#include "../../../core/polynomialfunctions/Evaluation.h"
#include "../../../core/polynomialfunctions/Resultant.h"
#include "../../../core/polynomialfunctions/RootCounting.h"
#include "../../../core/polynomialfunctions/SquareFreePart.h"
#include "../../../core/polynomialfunctions/Representation.h"
#include "../../../core/polynomialfunctions/RootBounds.h"
#include "../../../core/polynomialfunctions/to_univariate_polynomial.h"

#include "../../../interval/Interval.h"
#include "../../../interval/IntervalEvaluation.h"
#include "../../../interval/set_theory.h"

#include <list>

namespace carl {
namespace ran {
	template<typename Number>
	struct IntervalContent {
		using Polynomial = UnivariatePolynomial<Number>;
		static const Variable auxVariable;

		template<typename Num>
		friend bool compare(IntervalContent<Num>& lhs, IntervalContent<Num>& rhs, const Relation);

	private:
		struct Content {
			Polynomial polynomial;
			Interval<Number> interval;
			std::size_t refinementCount = 0;
			// TODO cache sgn at endpoints in RAN

			Content(Polynomial&& p, const Interval<Number>& i):
				polynomial(std::move(p)), interval(i)
			{}
			Content(const Polynomial& p, const Interval<Number>& i):
				polynomial(p), interval(i)
			{}
		};

		mutable std::shared_ptr<Content> mContent;

		static Polynomial replaceVariable(const Polynomial& p) {
			return carl::replace_main_variable(p, auxVariable);
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
			// assert(interval().isPointInterval() || count_real_roots(sturm_sequence(), interval()) == 1);
			assert(is_consistent());
			if (polynomial().degree() == 1) {
				Number a = polynomial().coefficients()[1];
				Number b = polynomial().coefficients()[0];
				interval() = Interval<Number>(Number(-b / a));
			} else {
				if (interval().contains(0)) refine_using(0);
				refineToIntegrality();
			}
		}

		bool is_consistent() const {
			if (interval().isPointInterval()) {
				return carl::is_root_of(polynomial(), interval().lower());
			} else {
				if (polynomial().normalized() != carl::squareFreePart(polynomial()).normalized()) {
					CARL_LOG_DEBUG("carl.ran.ir", "Poly is not square free: " << polynomial());
					return false;
				}
				auto lsgn = carl::sgn(carl::evaluate(polynomial(), interval().lower()));
				auto usgn = carl::sgn(carl::evaluate(polynomial(), interval().upper()));
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
		auto& refinementCount() const {
			return mContent->refinementCount;
		}

		bool is_zero() const {
			return carl::isZero(interval());
		}
		bool is_integral() const {
			refineToIntegrality();
			return interval().isPointInterval() && carl::isInteger(interval().lower());
		}
		Number integer_below() const {
			refineToIntegrality();
			return carl::floor(interval().lower());
		}

		std::size_t size() const {
			return carl::bitsize(interval().lower()) + carl::bitsize(interval().upper()) + polynomial().degree();
		}

		bool is_root_of(const UnivariatePolynomial<Number>& p) const { // TODO remove
			if (interval().isPointInterval())
				return carl::is_root_of(p, interval().lower());
			else
				return carl::count_real_roots(p, interval()) == 1; // TODO in general incorrect: take carl::gcd(p,polynomial())
		}

		bool isIntegral() {
			return interval().isPointInterval() && carl::isInteger(interval().lower());
		}
		
		void setPolynomial(const Polynomial& p) const {
			polynomial() = replaceVariable(p);
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
				refine_internal(i.lower());
			}
			if (interval().contains(i.upper())) {
				refine_internal(i.upper());
			}
			return i.contains(interval());
		}
		
		void refine() const {
			Number pivot = carl::sample(interval());
			refine_internal(pivot);
		}

		std::optional<Sign> refine_using(const Number& pivot) const {
			if (interval().contains(pivot)) {
				return refine_internal(pivot);
			}
			return std::nullopt;
		}

		/**
		 * Returns the sign of "interval() - pivot":
		 * Returns ZERO if pivot is equal to RAN.
		 * Returns POSITIVE if pivot is less than RAN resp. the new lower bound.
		 * Returns NEGATIVE if pivot is greater than RAN resp. the new upper bound.
		 */
		Sign refine_internal(const Number& pivot) const {
			// assert(is_consistent());
			assert(interval().contains(pivot));
			auto psgn = carl::sgn(carl::evaluate(polynomial(), pivot));
			if (psgn == Sign::ZERO) {
				interval() = Interval<Number>(pivot, pivot);
				return Sign::ZERO;
			}
			refinementCount()++;
			auto lsgn = carl::sgn(carl::evaluate(polynomial(), interval().lower()));
			if (psgn == lsgn) {
				interval().setLower(pivot);
				assert(interval().isConsistent());
				return Sign::POSITIVE;
			} else {
				interval().setUpper(pivot);
				assert(interval().isConsistent());
				return Sign::NEGATIVE;
			}
		}
			
		/** Refine the interval i of this real algebraic number yielding the interval j such that !j.meets(n). If true is returned, n is the exact numeric representation of this root. Otherwise not.
		 * @param n
		 * @rcomplexity constant
		 * @scomplexity constant
		 * @return true, if n is the exact numeric representation of this root, otherwise false
		 */
		bool refineAvoiding(const Number& n) const { // TODO remove?
			// assert(is_consistent());
			if (interval().contains(n)) {
				auto psgn = carl::sgn(carl::evaluate(polynomial(), n));
				if (psgn == Sign::ZERO) {
					interval() = Interval<Number>(n, n);
					return true;
				}
				auto lsgn = carl::sgn(carl::evaluate(polynomial(), interval().lower()));
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
			
			if (carl::is_root_of(polynomial(), newBound)) {
				interval() = Interval<Number>(newBound, newBound);
				return false;
			}
			
			if (isLeft) {
				interval().setLower(newBound);
			} else {
				interval().setUpper(newBound);
			}
			
			while (carl::sgn(carl::evaluate(polynomial(), interval().lower())) == carl::sgn(carl::evaluate(polynomial(), interval().upper()))) { // root is not in interval
				if (isLeft) {
					Number oldBound = interval().lower();
					newBound = carl::sample(Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT));
					if (carl::is_root_of(polynomial(), newBound)) {
						interval() = Interval<Number>(newBound, newBound);
						return false;
					}
					interval().setUpper(oldBound);
					interval().setLower(newBound);
				} else {
					Number oldBound = interval().upper();
					newBound = carl::sample(Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT));
					if (carl::is_root_of(polynomial(), newBound)) {
						interval() = Interval<Number>(newBound, newBound);
						return false;
					}
					interval().setLower(oldBound);
					interval().setUpper(newBound);
				}
			}
			return false;
			
			/* this method based using Sturm sequences was used before (and is significantly slower):
			if (interval().contains(n)) {
				if (carl::is_root_of(polynomial(), n)) {
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
			
			if (carl::is_root_of(polynomial(), newBound)) {
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
					if (carl::is_root_of(polynomial(), newBound)) {
						interval() = Interval<Number>(newBound, newBound);
						return false;
					}
					interval().setUpper(oldBound);
					interval().setLower(newBound);
				} else {
					Number oldBound = interval().upper();
					newBound = carl::sample(Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT));
					if (carl::is_root_of(polynomial(), newBound)) {
						interval() = Interval<Number>(newBound, newBound);
						return false;
					}
					interval().setLower(oldBound);
					interval().setUpper(newBound);
				}
			}
			return false;
			*/
		}
		
		void refineToIntegrality() const {
			while (!interval().isPointInterval() && interval().containsInteger()) {
				refine();
			}
		}

		void simplifyByPolynomial(Variable var, const MultivariatePolynomial<Number>& poly) const { // TODO remove
			UnivariatePolynomial<Number> irp(var, polynomial().template convert<Number>().coefficients());
			CARL_LOG_DEBUG("carl.ran", "gcd(" << irp << ", " << poly << ")");
			auto gmv = carl::gcd(MultivariatePolynomial<Number>(irp), poly);
			CARL_LOG_DEBUG("carl.ran", "Simplyfing, gcd = " << gmv);
			if (carl::isOne(gmv)) return;
			auto g = carl::to_univariate_polynomial(gmv);
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
	n.refine_using(constant_zero<Number>::get());
	if (n.interval().isSemiPositive()) return n;
	return IntervalContent<Number>(n.polynomial().negateVariable(), n.interval().abs());
}

template<typename Number>
Number branching_point(const IntervalContent<Number>& n) {
	return carl::sample(n.interval());
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
	if (m.size() == 1) {
		auto poly = carl::to_univariate_polynomial(p, m.begin()->first);
		if (m.begin()->second.sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
			CARL_LOG_DEBUG("carl.ran", "Returning " << IntervalContent<Number>());
			return IntervalContent<Number>();
		}
	}
	Variable v = freshRealVariable();
	// compute the result polynomial
	std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>> algebraic_information;
	for (const auto& cur: m) {
		algebraic_information.emplace_back(replace_main_variable(cur.second.polynomial(), cur.first).template convert<MultivariatePolynomial<Number>>());
	}
	UnivariatePolynomial<Number> res = carl::algebraic_substitution(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), algebraic_information);
	res = carl::squareFreePart(res);
	// Note that res cannot be zero as v is a fresh variable in v-p.
	// compute the initial result interval
	std::map<Variable, Interval<Number>> varToInterval;
	for (const auto& [var, ran] : m) {
		if (!p.has(var)) continue;
		varToInterval.emplace(var,ran.interval());
	}
	assert(!varToInterval.empty());
	CARL_LOG_DEBUG("carl.ran", "res = " << res);
	CARL_LOG_DEBUG("carl.ran", "varToInterval = " << varToInterval);
	CARL_LOG_DEBUG("carl.ran", "p = " << p);
	Interval<Number> interval = IntervalEvaluation::evaluate(p, varToInterval);
	CARL_LOG_DEBUG("carl.ran", "-> " << interval);

	auto sturmSeq = sturm_sequence(res);
	// the interval should include at least one root.
	assert(!carl::isZero(res));
	assert(
		carl::is_root_of(res, interval.lower()) ||
		carl::is_root_of(res, interval.upper()) ||
		count_real_roots(sturmSeq, interval) >= 1
	);
	while (
		carl::is_root_of(res, interval.lower()) ||
		carl::is_root_of(res, interval.upper()) ||
		count_real_roots(sturmSeq, interval) != 1) 
	{
		// refine the result interval until it isolates exactly one real root of the result polynomial
		for (auto it = m.begin(); it != m.end(); it++) {
			if (varToInterval.find(it->first) == varToInterval.end()) continue;
			it->second.refine();
			if (is_number(it->second)) {
				return evaluate(p, m);
			} else {
				varToInterval[it->first] = it->second.interval();
			}
		}
		interval = IntervalEvaluation::evaluate(p, varToInterval);
	}
	CARL_LOG_DEBUG("carl.ran", "Result is " << IntervalContent<Number>(res, interval));
	return IntervalContent<Number>(res, interval);
}

template<typename Number, typename Poly>
bool evaluate(const Constraint<Poly>& c, const std::map<Variable, IntervalContent<Number>>& m, bool use_intervals = false, bool use_root_bounds = false) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << c << " on " << m);
	Poly p = c.lhs();

	if (use_intervals) { // TODO needs some profound considerations ...
		CARL_LOG_DEBUG("carl.ran", "Evaluate constraint using interval arithmetic");
		static Number min_width = Number(1)/(Number(65536)); // (1/2)^16, taken from Z3
		while(true) {
			// evaluate
			std::map<Variable, Interval<Number>> varToInterval;
			for (const auto& var : carl::variables(p).underlyingVariables()) {
				varToInterval[var] = m.at(var).interval();
			}
			
			CARL_LOG_DEBUG("carl.ran",  "Evaluate " << p << " on " << varToInterval);
			auto res = IntervalEvaluation::evaluate(p, varToInterval);
			CARL_LOG_DEBUG("carl.ran",  "Interval evaluation obtained " << res);

			if (res.isPositive()) {
				CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
				return evaluate(Sign::POSITIVE, c.relation());
			} else if (res.isNegative()) {
				CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
				return evaluate(Sign::NEGATIVE, c.relation());
			} else if (res.isZero()) {
				CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
				return evaluate(Sign::ZERO, c.relation());
			} else if (res.isSemiNegative()) {
				if (c.relation() == Relation::GREATER) {
					CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
					return false;
				} else if (c.relation() == Relation::LEQ) {
					CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
					return true;
				}
			} else if (res.isSemiPositive()) {
				if (c.relation() == Relation::LESS) {
					CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
					return false;
				} else if (c.relation() == Relation::GEQ) {
					CARL_LOG_DEBUG("carl.ran", "Obtained result by interval evaluation");
					return true;
				}
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
							carl::substitute_inplace(p, a.first, Poly(get_number(m.at(a.first))));
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
			if (carl::variables(p).empty()) {
				CARL_LOG_DEBUG("carl.ran", c << " simplified to " << p << " ~ 0");
				assert(p.isNumber());
				return carl::evaluate(p.constantPart(), c.relation());
			}
		}
	}

	if (!use_root_bounds) {
		CARL_LOG_DEBUG("carl.ran", "Evaluate constraint by evaluating poly");
		IntervalContent<Number> res = evaluate(p, m);
		return evaluate(res.sgn(), c.relation());
	} else {
		CARL_LOG_DEBUG("carl.ran", "Evaluate constraint using resultants and root bounds");
		assert(m.size() > 0);
		if (m.size() == 1) {
			auto poly = carl::to_univariate_polynomial(p, m.begin()->first);
			if (m.begin()->second.sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
				CARL_LOG_DEBUG("carl.ran", "Got " << IntervalContent<Number>());
				return evaluate(Sign::ZERO, c.relation());
			}
		}
		Variable v = freshRealVariable();
		// compute the result polynomial
		std::vector<UnivariatePolynomial<MultivariatePolynomial<Number>>> algebraic_information;
		for (const auto& cur: m) {
			algebraic_information.emplace_back(replace_main_variable(cur.second.polynomial(), cur.first).template convert<MultivariatePolynomial<Number>>());
		}
		UnivariatePolynomial<Number> res = carl::algebraic_substitution(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), algebraic_information);
		// Note that res cannot be zero as v is a fresh variable in v-p.
		// compute the initial result interval
		std::map<Variable, Interval<Number>> varToInterval;
		for (const auto& [var, ran] : m) {
			assert(p.has(var));
			varToInterval.emplace(var,ran.interval());
		}
		assert(!varToInterval.empty());
		CARL_LOG_DEBUG("carl.ran", "res = " << res);
		CARL_LOG_DEBUG("carl.ran", "varToInterval = " << varToInterval);
		CARL_LOG_DEBUG("carl.ran", "p = " << p);
		Interval<Number> interval = IntervalEvaluation::evaluate(p, varToInterval);
		CARL_LOG_DEBUG("carl.ran", "-> " << interval);

		// Let pos_lb a lower bound on the positive real roots and neg_ub an upper bound on the negative real roots
		// Then if the zero of res is in the interval (neg_ub,pos_lb), then it must be zero.

		// compute root bounds
		auto pos_lb = lagrangePositiveLowerBound(res);
		CARL_LOG_TRACE("carl.ran", "positive root lower bound: " << pos_lb);
		if (pos_lb == 0) {
			// no positive root exists
			CARL_LOG_DEBUG("carl.ran", "p <= 0");
			if (c.relation() == Relation::GREATER) {
				return false;
			} else if (c.relation() == Relation::LEQ) {
				return true;
			}
		}
		auto neg_ub = lagrangeNegativeUpperBound(res);
		CARL_LOG_TRACE("carl.ran", "negative root upper bound: " << neg_ub);
		if (neg_ub == 0) {
			// no negative root exists
			CARL_LOG_DEBUG("carl.ran", "p >= 0");
			if (c.relation() == Relation::LESS) {
				return false;
			} else if (c.relation() == Relation::GEQ) {
				return true;
			}
		}

		if (pos_lb == 0 && neg_ub == 0) {
			// no positive or negative zero exists
			CARL_LOG_DEBUG("carl.ran", "p = 0");
			return evaluate(Sign::ZERO, c.relation());
		}

		assert(!carl::isZero(res));

		// refine the interval until it is either positive or negative or is contained in (neg_ub,pos_lb)
		CARL_LOG_DEBUG("carl.ran", "Refine until interval is in ("<< neg_ub<<","<<pos_lb<<") or interval is positive or negative");
		while (!( (neg_ub < interval.lower() || neg_ub == 0) && (interval.upper() < pos_lb || pos_lb == 0) )) 
		{

			for (auto it = m.begin(); it != m.end(); it++) {
				it->second.refine();
				if (is_number(it->second)) {
					return evaluate(c, m);
				} else {
					varToInterval[it->first] = it->second.interval();
				}
			}
			interval = IntervalEvaluation::evaluate(p, varToInterval);
			if (interval.isNegative()) {
				CARL_LOG_DEBUG("carl.ran", "p < 0");
				return evaluate(Sign::NEGATIVE, c.relation());
			} else if (interval.isPositive()) {
				CARL_LOG_DEBUG("carl.ran", "p > 0");
				return evaluate(Sign::POSITIVE, c.relation());
			} else if (interval.isSemiNegative()) {
				CARL_LOG_DEBUG("carl.ran", "p <= 0");
				if (c.relation() == Relation::GREATER) {
					return false;
				} else if (c.relation() == Relation::LEQ) {
					return true;
				}
			} else if (interval.isSemiPositive()) {
				CARL_LOG_DEBUG("carl.ran", "p >= 0");
				if (c.relation() == Relation::LESS) {
					return false;
				} else if (c.relation() == Relation::GEQ) {
					return true;
				}
			}
		}
		
		CARL_LOG_DEBUG("carl.ran", "p = 0");
		return evaluate(Sign::ZERO, c.relation());
	}
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
	lower.refine_using(upper.interval().lower());
	upper.refine_using(lower.interval().upper());
	assert(lower.interval().upper() <= upper.interval().lower());
	return sample_between(NumberContent<Number>(lower.interval().upper()), NumberContent<Number>(upper.interval().lower()));
}
template<typename Number>
Number sample_between(IntervalContent<Number>& lower, const NumberContent<Number>& upper) {
	lower.refine_using(upper.value());
	assert(lower.interval().upper() <= upper.value());
	return sample_between(NumberContent<Number>(lower.interval().upper()), upper);
}
template<typename Number>
Number sample_between(const NumberContent<Number>& lower, IntervalContent<Number>& upper) {
	upper.refine_using(lower.value());
	assert(lower.value() <= upper.interval().lower());
	return sample_between(lower, NumberContent<Number>(upper.interval().lower()));
}

template<typename Number>
bool compare(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs, const Relation relation) {
	if (lhs.mContent.get() == rhs.mContent.get()) {
		return evaluate(Sign::ZERO, relation);
	}

	if (lhs.interval().isPointInterval() && lhs.interval() == rhs.interval()) {
		return evaluate(Sign::ZERO, relation);
	}

	if (carl::set_have_intersection(lhs.interval(), rhs.interval())) {
		auto intersection = carl::set_intersection(lhs.interval(), rhs.interval());
		assert(!intersection.isEmpty());
		lhs.refine_using(intersection.lower());
		rhs.refine_using(intersection.lower());
		if (intersection.isPointInterval()) {
			lhs.refine_using(intersection.upper());
			rhs.refine_using(intersection.upper());
		}
	}
	// now: intervals are either equal or disjoint

	if (lhs.interval() == rhs.interval()) {
		if (lhs.polynomial() == rhs.polynomial()) {
			return evaluate(Sign::ZERO, relation);
		}
		auto g = carl::gcd(lhs.polynomial(), rhs.polynomial());
		auto lsgn = carl::sgn(carl::evaluate(g, lhs.interval().lower()));
		auto usgn = carl::sgn(carl::evaluate(g, lhs.interval().upper()));
		if (lsgn != usgn) {
			lhs.setPolynomial(g);
			rhs.setPolynomial(g);
			return evaluate(Sign::ZERO, relation);
		} else {
			if (relation == Relation::EQ) return false;
			if (relation == Relation::NEQ) return true;
			while (lhs.interval() == rhs.interval()) {
				lhs.refine();
				rhs.refine();
			}
		}
	}
	// now: intervals are disjoint
	if (lhs.interval().upper() < rhs.interval().lower()) {
		return relation == Relation::LESS || relation == Relation::LEQ;
	}
	if (lhs.interval().lower() > rhs.interval().upper()) {
		return relation == Relation::GREATER || relation == Relation::GEQ;
	}
	
	assert(false);
	return false;
}

template<typename Number>
bool operator==(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::EQ);
}

template<typename Number>
bool operator!=(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::NEQ);
}
template<typename Number>
bool operator<=(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::LEQ);
}
template<typename Number>
bool operator>=(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::GEQ);
}
template<typename Number>
bool operator<(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::LESS);
}
template<typename Number>
bool operator>(IntervalContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::GREATER);
}

template<typename Number>
bool compare(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs, const Relation relation) {
	auto res = lhs.refine_using(rhs.value());
	if (res) {
		return evaluate(*res, relation);
	} else {
		if (relation == Relation::EQ) return false;
		else if (relation == Relation::NEQ) return true;
		else if (relation == Relation::LESS || relation == Relation::LEQ) return lhs.interval().upper() <= rhs.value();
		else if (relation == Relation::GREATER || relation == Relation::GEQ) return lhs.interval().lower() >= rhs.value();
	}
	assert(false);
	return false;
}

template<typename Number>
bool operator==(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::EQ);
}
template<typename Number>
bool operator!=(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::NEQ);
}
template<typename Number>
bool operator<=(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::LEQ);
}
template<typename Number>
bool operator>=(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::GEQ);
}
template<typename Number>
bool operator<(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::LESS);
}
template<typename Number>
bool operator>(IntervalContent<Number>& lhs, const NumberContent<Number>& rhs) {
	return compare(lhs, rhs, Relation::GREATER);
}

template<typename Number>
bool operator==(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return rhs == lhs;
}
template<typename Number>
bool operator!=(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return rhs != lhs;
}
template<typename Number>
bool operator<=(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return rhs >= lhs;
}
template<typename Number>
bool operator>=(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return rhs <= lhs;
}
template<typename Number>
bool operator<(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return rhs > lhs;
}
template<typename Number>
bool operator>(const NumberContent<Number>& lhs, IntervalContent<Number>& rhs) {
	return rhs < lhs;
}


template<typename Num>
std::ostream& operator<<(std::ostream& os, const IntervalContent<Num>& ran) {
	return os << "IR " << ran.interval() << ", " << ran.polynomial();
}

template<typename Number>
const Variable IntervalContent<Number>::auxVariable = freshRealVariable("__r");

}
}

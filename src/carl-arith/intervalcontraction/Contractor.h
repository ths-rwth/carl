#pragma once

#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/core/Variables.h>
#include <carl-arith/constraint/BasicConstraint.h>
#include <carl-arith/interval/Interval.h>
#include <carl-arith/poly/umvpoly/functions/IntervalEvaluation.h>
#include <carl-arith/interval/set_theory.h>

#include <algorithm>


namespace carl {
namespace contractor {

/**
 * Represents a contraction operation of the form 
 * 
 * mRoot'th root of (mNumerator / mDenominator)
 */
template<typename Polynomial>
class Evaluation {
private:
	Variable mVar;
	Polynomial mNumerator;
	Polynomial mDenominator;
	std::size_t mRoot;
	std::vector<Variable> mDependees;

	bool make_integer() const {
		return mVar.type() == VariableType::VT_INT;
	}

	template<typename Number>
	void add_root(const Interval<Number>& i, std::vector<Interval<Number>>& result) const {
		if (mRoot == 1) {
			result.emplace_back(i);
			return;
		}
		auto tmp = i.root(static_cast<int>(mRoot));
		CARL_LOG_DEBUG("carl.contractor", mRoot << "th root: " << tmp);
		if (make_integer()) {
			tmp = tmp.integral_part();
			CARL_LOG_DEBUG("carl.contractor", "Strictening for integer: " << tmp);
		}
		if (tmp.is_empty()) {
			CARL_LOG_DEBUG("carl.contractor", "Is empty.");
			return;
		}
		if (mRoot % 2 == 0) {
			// Also consider the negative part
			Interval<Number> resA;
			Interval<Number> resB;
			if (set_union(tmp, -tmp, resA, resB)) {
				CARL_LOG_DEBUG("carl.contractor", mRoot << "th root of " << i << " -> " << resA << " / " << resB);
				result.emplace_back(resA);
				result.emplace_back(resB);
			} else {
				CARL_LOG_DEBUG("carl.contractor", mRoot << "th root of " << i << " -> " << resA);
				result.emplace_back(resA);
			}
		} else {
			CARL_LOG_DEBUG("carl.contractor", mRoot << "th root of " << i << " -> " << tmp);
			result.emplace_back(tmp);
		}
	}
	public:
	template<typename Number>
	void normalize(std::vector<Interval<Number>>& intervals) const {
		if (intervals.empty()) return;
		CARL_LOG_DEBUG("carl.contractor", "From " << intervals);
		std::sort(intervals.begin(), intervals.end(),
			[](const auto& lhs, const auto& rhs) {
				return lhs.lower() < rhs.lower();
			}
		);
		std::size_t last = 0;
		for (std::size_t i = 1; i < intervals.size(); ++i) {
			if (set_have_intersection(intervals[last], intervals[i])) {
				Interval<Number> dummy;
				CARL_LOG_DEBUG("carl.contractor", "Combining " << intervals[last] << " and " << intervals[i]);
				bool res = set_union(intervals[last], intervals[i], intervals[last], dummy);
				assert(!res);
			} else {
				++last;
			}
		}
		intervals.resize(last + 1);
		CARL_LOG_DEBUG("carl.contractor", "->   " << intervals);
	}
public:
	Evaluation(const Polynomial& p, Variable v):
		mVar(v)
	{
		assert(p.has(v));
		mRoot = p.degree(v);
		for (const auto& t: p) {
			if (t.monomial() && t.monomial()->exponent_of_variable(v) == mRoot) {
				mDenominator += t.drop_variable(v);
			} else {
				mNumerator -= t;
			}
		}
		CARL_LOG_DEBUG("carl.contractor", p << " with " << v << " -> " << *this);
		carlVariables vars;
		carl::variables(mNumerator, vars);
		carl::variables(mDenominator, vars);
		mDependees = vars.as_vector();
	}

	auto var() const {
		return mVar;
	}
	const auto& numerator() const {
		return mNumerator;
	}
	const auto& denominator() const {
		return mDenominator;
	}
	auto root() const {
		return mRoot;
	}
	const auto& dependees() const {
		return mDependees;
	}

	/**
	 * Evaluate this contraction over the given assignment.
	 * Returns a list of resulting intervals.
	 * 
	 * Allows to integrate a relation symbol as follows:
	 * - Transform relation into an interval (e.g. < 0 to = (-oo, 0))
	 * - Transform constraint to equality (e.g. p*x - q < 0 to p*x - q = h)
	 * - Evaluate with respect to interval h (e.g. x = (q + h) / p)
	 */
	template<typename Number>
	std::vector<Interval<Number>> evaluate(const std::map<Variable, Interval<Number>>& assignment, const Interval<Number>& h = Interval<Number>(0,0)) const {
		std::vector<Interval<Number>> res;
		CARL_LOG_DEBUG("carl.contractor", "Evaluating on " << assignment);
		auto num = carl::evaluate(numerator(), assignment);
		CARL_LOG_DEBUG("carl.contractor", numerator() << " -> " << num);
		num += h;
		CARL_LOG_DEBUG("carl.contractor", "Subtracting " << h << " -> " << num);
		if (!is_one(denominator())) {
			auto den = carl::evaluate(denominator(), assignment);;
			CARL_LOG_DEBUG("carl.contractor", denominator() << " -> " << den);
			Interval<Number> resA;
			Interval<Number> resB;
			if (num.div_ext(den, resA, resB)) {
				Interval<Number> resC;
				Interval<Number> resD;
				if (set_union(resA, resB, resC, resD)) {
					CARL_LOG_DEBUG("carl.contractor", "Got a split -> " << resC << " and " << resD);
					add_root(resC, res);
					add_root(resD, res);
				} else {
					CARL_LOG_DEBUG("carl.contractor", "Got no split -> " << resC);
					add_root(resC, res);
				}
			} else {
				CARL_LOG_DEBUG("carl.contractor", "Got no split -> " << resA);
				add_root(resA, res);
			}
		} else {
			CARL_LOG_DEBUG("carl.contractor", "Got no denominator -> " << num);
			add_root(num, res);
		}
		normalize(res);
		CARL_LOG_DEBUG("carl.contractor", "Result: " << res);
		return res;
	}
};

template<typename Polynomial>
std::ostream& operator<<(std::ostream& os, const Evaluation<Polynomial>& e) {
	if (e.root() != 1) {
		os << e.root() << "th root of ";
	}
	if (!is_one(e.denominator())) {
		os << "(" << e.numerator() << " / " << e.denominator() << ")";
	} else {
		os << e.numerator();
	}
	return os;
}

template<typename Origin, typename Polynomial, typename Number = double>
class Contractor {
private:
	Evaluation<Polynomial> mEvaluation;
	Interval<Number> mRelation;
	Origin mOrigin;
public:
	Contractor(const Origin& origin, const BasicConstraint<Polynomial>& c, Variable v):
		mEvaluation(c.lhs(), v),
		mOrigin(origin)
	{
		switch (c.relation()) {
			case Relation::LESS:
				mRelation = Interval<Number>(0, BoundType::INFTY, 0, BoundType::STRICT);
				break;
			case Relation::LEQ:
				mRelation = Interval<Number>(0, BoundType::INFTY, 0, BoundType::WEAK);
				break;
			case Relation::EQ:
				mRelation = Interval<Number>(0, BoundType::WEAK, 0, BoundType::WEAK);
				break;
			case Relation::NEQ:
				assert(false);
				mRelation = Interval<Number>(0, BoundType::STRICT, 0, BoundType::STRICT);
				break;
			case Relation::GEQ:
				mRelation = Interval<Number>(0, BoundType::WEAK, 0, BoundType::INFTY);
				break;
			case Relation::GREATER:
				mRelation = Interval<Number>(0, BoundType::STRICT, 0, BoundType::INFTY);
				break;
		}
	}

	auto var() const {
		return mEvaluation.var();
	}
	const auto& dependees() const {
		return mEvaluation.dependees();
	}
	const auto& origin() const {
		return mOrigin;
	}

	std::vector<Interval<Number>> evaluate(const std::map<Variable, Interval<Number>>& assignment) const {
		CARL_LOG_DEBUG("carl.contractor", "Evaluating " << mEvaluation << " on " << assignment);
		return mEvaluation.evaluate(assignment, mRelation);
	}

	std::vector<Interval<Number>> contract(const std::map<Variable, Interval<Number>>& assignment) const {
		auto res = evaluate(assignment);
		assert(assignment.find(mEvaluation.var()) != assignment.end());
		auto cur = assignment.find(mEvaluation.var())->second;
		CARL_LOG_DEBUG("carl.contractor", "Intersecting " << res << " with " << cur);

		std::size_t last = 0;
		for (std::size_t i = 0; i < res.size(); ++i) {
			auto tmp = set_intersection(res[i], cur);
			if (!tmp.is_empty()) {
				res[last] = tmp;
				last++;
			}
		}
		res.resize(last);

		CARL_LOG_DEBUG("carl.contractor", "-> " << res);
		return res;
	}
};

}
}

#pragma once

#include "../core/MultivariatePolynomial.h"
#include "Interval.h"
#include "IntervalEvaluation.h"
#include "set_theory.h"

#include <algorithm>


namespace carl {
namespace contractor {

/**
 * Represents a contraction operation of the form 
 * 
 * mRoot'th root of (mNumerator / mDenominator)
 */
template<typename Polynomial>
class Contraction {
private:
	Variable mVar;
	Polynomial mNumerator;
	Polynomial mDenominator;
	std::size_t mRoot;

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
			tmp = tmp.integralPart();
			CARL_LOG_DEBUG("carl.contractor", "Strictening for integer: " << tmp);
		}
		if (tmp.isEmpty()) {
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
	Contraction(const Polynomial& p, Variable v):
		mVar(v)
	{
		assert(p.has(v));
		mRoot = p.degree(v);
		for (const auto& t: p) {
			if (t.monomial() && t.monomial()->exponentOfVariable(v) == mRoot) {
				mDenominator += t.dropVariable(v);
			} else {
				mNumerator -= t;
			}
		}
		if (mDenominator.isNumber()) {
			mNumerator /= mDenominator.constantPart();
			mDenominator = Polynomial(1);
		}
		CARL_LOG_DEBUG("carl.contractor", p << " with " << v << " -> " << *this);
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

	/**
	 * Evaluate this contraction over the given assignment.
	 * Returns a list of resulting intervals.
	 */
	template<typename Number>
	std::vector<Interval<Number>> evaluate(const std::map<Variable, Interval<Number>>& assignment) const {
		std::vector<Interval<Number>> res;
		CARL_LOG_DEBUG("carl.contractor", "Evaluating on " << assignment);
		auto num = IntervalEvaluation::evaluate(numerator(), assignment);
		CARL_LOG_DEBUG("carl.contractor", numerator() << " -> " << num);
		if (!isOne(denominator())) {
			auto den = IntervalEvaluation::evaluate(denominator(), assignment);;
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
std::ostream& operator<<(std::ostream& os, const Contraction<Polynomial>& c) {
	if (c.root() != 1) {
		os << c.root() << "th root of ";
	}
	if (!isOne(c.denominator())) {
		os << "(" << c.numerator() << " / " << c.denominator() << ")";
	} else {
		os << c.numerator();
	}
	return os;
}

}
}
#pragma once

#include <carl-arith/poly/umvpoly/functions/SquareFreePart.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/functions/Division.h>
#include <carl-arith/poly/umvpoly/functions/Evaluation.h>
#include <carl-arith/poly/umvpoly/functions/Representation.h>
#include <carl-arith/poly/umvpoly/functions/RootBounds.h>
#include <carl-arith/poly/umvpoly/functions/RootCounting.h>
#include <carl-arith/constraint/BasicConstraint.h>

#include <carl-arith/interval/Interval.h>
#include <carl-arith/poly/umvpoly/functions/IntervalEvaluation.h>
#include <carl-arith/interval/set_theory.h>

#include "../common/Operations.h"
#include "../common/NumberOperations.h"

#include <list>
#include <boost/logic/tribool.hpp>

namespace carl {

template<typename Number>
class IntRepRealAlgebraicNumber {
	static const Variable auxVariable;

	template<typename Num>
	friend bool compare(const IntRepRealAlgebraicNumber<Num>&, const IntRepRealAlgebraicNumber<Num>&, const Relation);

	template<typename Num>
	friend bool compare(const IntRepRealAlgebraicNumber<Num>&, const Num&, const Relation);

	template<typename Num, typename Poly>
	friend boost::tribool evaluate(const BasicConstraint<Poly>&, const Assignment<IntRepRealAlgebraicNumber<Num>>&, bool, bool);

	template<typename Num>
	friend std::optional<IntRepRealAlgebraicNumber<Num>> evaluate(MultivariatePolynomial<Num>, const Assignment<IntRepRealAlgebraicNumber<Num>>&, bool);

	template<typename Num>
	friend Num branching_point(const IntRepRealAlgebraicNumber<Num>& n);

	template<typename Num>
	friend Num sample_above(const IntRepRealAlgebraicNumber<Num>& n);

	template<typename Num>
	friend Num sample_below(const IntRepRealAlgebraicNumber<Num>& n);

	template<typename Num>
	friend Num sample_between(const IntRepRealAlgebraicNumber<Num>& lower, const IntRepRealAlgebraicNumber<Num>& upper);

	template<typename Num>
	friend Num sample_between(const IntRepRealAlgebraicNumber<Num>& lower, const Num& upper);

	template<typename Num>
	friend Num sample_between(const Num& lower, const IntRepRealAlgebraicNumber<Num>& upper);

	template<typename Num>
	friend Num floor(const IntRepRealAlgebraicNumber<Num>& n);

	template<typename Num>
	friend Num ceil(const IntRepRealAlgebraicNumber<Num>& n);

	template<typename Num>
	friend Sign sgn(const IntRepRealAlgebraicNumber<Num>& n, const UnivariatePolynomial<Num>& p);

private:
	struct content {
		std::optional<UnivariatePolynomial<Number>> polynomial;
		Interval<Number> interval;
		/// Sign of polynomial at interval.lower()
		Sign lower_sign;

		content(const Interval<Number>& i)
			: polynomial(std::nullopt), interval(i), lower_sign(Sign::ZERO) {}
		content(UnivariatePolynomial<Number>&& p, const Interval<Number>& i)
			: polynomial(std::move(p)), interval(i), lower_sign(Sign::ZERO) {}
		content(const UnivariatePolynomial<Number>& p, const Interval<Number>& i)
			: polynomial(p), interval(i), lower_sign(Sign::ZERO) {}
		void simplify_to_point() {
			assert(interval.is_point_interval());
			polynomial = std::nullopt;
			lower_sign = Sign::ZERO;
		}
	};

	mutable std::shared_ptr<content> m_content;

	static UnivariatePolynomial<Number> replace_variable(const UnivariatePolynomial<Number>& p) {
		return carl::replace_main_variable(p, auxVariable);
	}

	bool is_consistent() const {
		if (interval_int().is_point_interval()) {
			return !m_content->polynomial && m_content->lower_sign == Sign::ZERO;
		} else {
			if (interval_int().contains(0) || interval_int().contains_integer()) {
				CARL_LOG_DEBUG("carl.ran.interval", "Interval contains 0 or integer");
				return false;
			}
			if (polynomial_int().normalized() != carl::squareFreePart(polynomial_int()).normalized()) {
				CARL_LOG_DEBUG("carl.ran.interval", "Poly is not square free: " << polynomial_int());
				return false;
			}
			auto lsgn = carl::sgn(carl::evaluate(polynomial_int(), interval_int().lower()));
			auto usgn = carl::sgn(carl::evaluate(polynomial_int(), interval_int().upper()));
			if (lsgn == Sign::ZERO || usgn == Sign::ZERO || lsgn == usgn) {
				CARL_LOG_DEBUG("carl.ran.interval", "Interval does not define a zero");
				return false;
			}
			if (m_content->lower_sign != lsgn) {
				CARL_LOG_DEBUG("carl.ran.interval", "Lower sign does not match");
				return false;
			}
			return true;
		}
	}

	void set_polynomial(const UnivariatePolynomial<Number>& p, Sign lower_sign) const {
		assert(!interval_int().is_point_interval());
		polynomial_int() = replace_variable(p);
		m_content->lower_sign = lower_sign;
		assert(is_consistent());
	}

	/**
	 * Returns the sign of "interval_int() - pivot":
	 * Returns ZERO if pivot is equal to RAN.
	 * Returns POSITIVE if pivot is less than RAN resp. the new lower bound.
	 * Returns NEGATIVE if pivot is greater than RAN resp. the new upper bound.
	 */
	Sign refine_internal(const Number& pivot) const {
		// assert(is_consistent());
		assert(interval_int().contains(pivot));
		assert(!interval_int().is_point_interval());
		auto psgn = carl::sgn(carl::evaluate(polynomial_int(), pivot));
		if (psgn == Sign::ZERO) {
			interval_int() = Interval<Number>(pivot, pivot);
			m_content->simplify_to_point();
			return Sign::ZERO;
		}
		if (psgn == m_content->lower_sign) {
			interval_int().set_lower(pivot);
			assert(interval_int().is_consistent());
			return Sign::POSITIVE;
		} else {
			interval_int().set_upper(pivot);
			assert(interval_int().is_consistent());
			return Sign::NEGATIVE;
		}
	}

public: // TODO should be private
	void refine() const {
		if (is_numeric()) return;
		Number pivot = carl::sample(interval_int());
		refine_internal(pivot);
	}

private:
	std::optional<Sign> refine_using(const Number& pivot) const {
		if (interval_int().contains(pivot)) {
			if (is_numeric()) return Sign::ZERO;
			else return refine_internal(pivot);
		}
		return std::nullopt;
	}

	/// Refines until the number is either numeric or the interval does not contain any integer.
	void refine_to_integrality() const {
		while (!interval_int().is_point_interval() && interval_int().contains_integer()) {
			refine();
		}
	}

public:
	IntRepRealAlgebraicNumber()
		: m_content(std::make_shared<content>(Interval<Number>(0))) {}

	IntRepRealAlgebraicNumber(const Number& n)
		: m_content(std::make_shared<content>(Interval<Number>(n))) {}

	IntRepRealAlgebraicNumber(const UnivariatePolynomial<Number>& p, const Interval<Number>& i)
		: m_content(std::make_shared<content>(replace_variable(p), i)) {
		CARL_LOG_DEBUG("carl.ran.interval", "Creating (" << p << "," << i << ")");
		assert(!carl::is_zero(polynomial_int()) && polynomial_int().degree() > 0);
		assert(interval_int().is_open_interval() || interval_int().is_point_interval());
		// assert(interval_int().is_point_interval() || count_real_roots(sturm_sequence(), interval_int()) == 1);
		if (interval_int().is_point_interval()) {
			m_content->simplify_to_point();
		} else if (polynomial_int().degree() == 1) {
			Number a = polynomial_int().coefficients()[1];
			Number b = polynomial_int().coefficients()[0];
			interval_int() = Interval<Number>(Number(-b / a));
			m_content->simplify_to_point();
		} else {
			m_content->lower_sign = carl::sgn(carl::evaluate(polynomial_int(), interval_int().lower()));
			if (interval_int().contains(0)) refine_using(0);
			refine_to_integrality();
		}
		assert(is_consistent());
	}

	IntRepRealAlgebraicNumber(const IntRepRealAlgebraicNumber& ran) = default;
	IntRepRealAlgebraicNumber(IntRepRealAlgebraicNumber&& ran) = default;

	IntRepRealAlgebraicNumber& operator=(const IntRepRealAlgebraicNumber& n) = default;
	IntRepRealAlgebraicNumber& operator=(IntRepRealAlgebraicNumber&& n) = default;

	static IntRepRealAlgebraicNumber<Number> create_safe(const UnivariatePolynomial<Number>& p, const Interval<Number>& i) {
		return IntRepRealAlgebraicNumber<Number>(carl::squareFreePart(p), i);
	}

	bool is_numeric() const {
		return interval_int().is_point_interval();
	}

	const auto& polynomial() const {
		assert(!is_numeric());
		return *(m_content->polynomial);
	}
	const auto& interval() const {
		assert(!is_numeric());
		return m_content->interval;
	}

	const auto& value() const {
		assert(is_numeric());
		return interval_int().lower();
	}

	auto& polynomial_int() const {
		return *(m_content->polynomial);
	}
	auto& interval_int() const {
		return m_content->interval;
	}
};

template<typename Number>
Number branching_point(const IntRepRealAlgebraicNumber<Number>& n) {
	return carl::sample(n.interval_int());
}

template<typename Number>
Number sample_above(const IntRepRealAlgebraicNumber<Number>& n) {
	return carl::floor(n.interval_int().upper()) + 1;
}
template<typename Number>
Number sample_below(const IntRepRealAlgebraicNumber<Number>& n) {
	return carl::ceil(n.interval_int().lower()) - 1;
}
template<typename Number>
Number sample_between(const IntRepRealAlgebraicNumber<Number>& lower, const IntRepRealAlgebraicNumber<Number>& upper) {
	lower.refine_using(upper.interval_int().lower());
	upper.refine_using(lower.interval_int().upper());
	assert(lower.interval_int().upper() <= upper.interval_int().lower());
	if (lower.is_numeric()) {
		return sample_between(lower.value(), upper);
	} else if (upper.is_numeric()) {
		return sample_between(lower, upper.value());
	} else {
		return sample(Interval<Number>(lower.interval_int().upper(), upper.interval_int().lower()), true);
	}
}
template<typename Number>
Number sample_between(const IntRepRealAlgebraicNumber<Number>& lower, const Number& upper) {
	lower.refine_using(upper);
	assert(lower.interval_int().upper() <= upper);
	assert(lower < upper);
	while (lower.interval_int().upper() == upper)
		lower.refine();
	if (lower.is_numeric()) {
		return sample_between(lower.value(), upper);
	} else {
		return sample(Interval<Number>(lower.interval_int().upper(), BoundType::WEAK, upper, BoundType::STRICT), false);
	}
}
template<typename Number>
Number sample_between(const Number& lower, const IntRepRealAlgebraicNumber<Number>& upper) {
	upper.refine_using(lower);
	assert(lower <= upper.interval_int().lower());
	assert(lower < upper);
	while (lower == upper.interval_int().lower())
		upper.refine();
	if (upper.is_numeric()) {
		return sample_between(lower, upper.value());
	} else {
		return sample(Interval<Number>(lower, BoundType::STRICT, upper.interval_int().lower(), BoundType::WEAK), false);
	}
}
template<typename Number>
Number floor(const IntRepRealAlgebraicNumber<Number>& n) {
	return carl::floor(n.interval_int().lower());
}
template<typename Number>
Number ceil(const IntRepRealAlgebraicNumber<Number>& n) {
	return carl::ceil(n.interval_int().upper());
}

template<typename Number>
inline bool is_zero(const IntRepRealAlgebraicNumber<Number>& n) {
	return carl::is_zero(n.interval_int());
}

template<typename Number>
inline bool is_integer(const IntRepRealAlgebraicNumber<Number>& n) {
	return n.interval_int().is_point_interval() && carl::is_integer(n.interval_int().lower());
}

template<typename Number>
inline Number integer_below(const IntRepRealAlgebraicNumber<Number>& n) {
	return carl::floor(n.interval_int().lower());
}

template<typename Number>
static IntRepRealAlgebraicNumber<Number> abs(const IntRepRealAlgebraicNumber<Number>& n) {
	assert(!n.interval_int().contains(constant_zero<Number>::get()) || n.interval_int().is_point_interval());
	if (n.interval_int().is_semi_positive()) {
		return n;
	}
	else {
		if (n.is_numeric()) {
			return IntRepRealAlgebraicNumber<Number>(carl::abs(n.value()));
		} else {
			return IntRepRealAlgebraicNumber<Number>(n.polynomial_int().negate_variable(), abs(n.interval_int()));
		}
	}
}

template<typename Number>
std::size_t size(const IntRepRealAlgebraicNumber<Number>& n) {
	if (n.is_numeric()) {
		return carl::bitsize(n.interval_int().lower()) + carl::bitsize(n.interval_int().upper());
	} else {
		return carl::bitsize(n.interval_int().lower()) + carl::bitsize(n.interval_int().upper()) + n.polynomial_int().degree();
	}
}

template<typename Number>
Sign sgn(const IntRepRealAlgebraicNumber<Number>& n) {
	if (n.interval_int().is_point_interval()) return carl::sgn(n.interval_int().lower());
	assert(!n.interval_int().contains(constant_zero<Number>::get()));
	if (n.interval_int().is_semi_positive())
		return Sign::POSITIVE;
	else {
		assert(n.interval_int().is_semi_negative());
		return Sign::NEGATIVE;
	}
}

template<typename Number>
Sign sgn(const IntRepRealAlgebraicNumber<Number>& n, const UnivariatePolynomial<Number>& p) {
	UnivariatePolynomial<Number> tmp = IntRepRealAlgebraicNumber<Number>::replace_variable(p);
	if (n.polynomial_int() == tmp) return Sign::ZERO;
	auto seq = carl::sturm_sequence(n.polynomial_int(), derivative(n.polynomial_int()) * tmp);
	int variations = carl::count_real_roots(seq, n.interval_int());
	assert((variations == -1) || (variations == 0) || (variations == 1));
	switch (variations) {
	case -1:
		return Sign::NEGATIVE;
	case 0:
		return Sign::ZERO;
	case 1:
		return Sign::POSITIVE;
	default:
		CARL_LOG_ERROR("carl.ran.interval", "Unexpected number of variations, should be -1, 0, 1 but was " << variations);
		return Sign::ZERO;
	}
}

template<typename Number>
bool contained_in(const IntRepRealAlgebraicNumber<Number>& n, const Interval<Number>& i) {
	if (!n.is_numeric() && n.interval_int().contains(i.lower())) {
		n.refine_internal(i.lower());
	}
	if (!n.is_numeric() && n.interval_int().contains(i.upper())) {
		n.refine_internal(i.upper());
	}
	return i.contains(n.interval_int());
}

template<typename Number>
bool compare(const IntRepRealAlgebraicNumber<Number>& lhs, const IntRepRealAlgebraicNumber<Number>& rhs, const Relation relation) {
	CARL_LOG_DEBUG("carl.ran.interval", "Compare " << lhs << " " << relation << " " << rhs);

	if (lhs.m_content.get() == rhs.m_content.get()) {
		CARL_LOG_TRACE("carl.ran.interval", "Contents are equal");
		return evaluate(Sign::ZERO, relation);
	}

	if (lhs.interval_int().is_point_interval() && rhs.interval_int().is_point_interval()) {
		CARL_LOG_TRACE("carl.ran.interval", "Point interval comparison");
		return evaluate(lhs.interval_int().lower(), relation, rhs.interval_int().lower());
	}

	if (carl::set_have_intersection(lhs.interval_int(), rhs.interval_int())) {
		CARL_LOG_TRACE("carl.ran.interval", "Intervals " << lhs.interval_int() << " and " << rhs.interval_int() << " do intersect");
		auto intersection = carl::set_intersection(lhs.interval_int(), rhs.interval_int());
		assert(!intersection.is_empty());
		lhs.refine_using(intersection.lower());
		rhs.refine_using(intersection.lower());
		if (!intersection.is_point_interval()) {
			lhs.refine_using(intersection.upper());
			rhs.refine_using(intersection.upper());
		}
	}
	// now: intervals are either equal or disjoint
	assert(!carl::set_have_intersection(lhs.interval_int(), rhs.interval_int()) || lhs.interval_int() == rhs.interval_int());
	if (lhs.interval_int() == rhs.interval_int()) {
		CARL_LOG_TRACE("carl.ran.interval", "Intervals " << lhs.interval_int() << " and " << rhs.interval_int() << " are equal");
		if (lhs.is_numeric()) {
			CARL_LOG_TRACE("carl.ran.interval", "Interval " << lhs.interval_int() << " is a point interval");
			return evaluate(Sign::ZERO, relation);
		}
		if (lhs.polynomial_int() == rhs.polynomial_int()) {
			CARL_LOG_TRACE("carl.ran.interval", "Polynomials " << lhs.polynomial_int() << " and " << rhs.polynomial_int() << " are equal");
			return evaluate(Sign::ZERO, relation);
		}
		auto g = carl::gcd(lhs.polynomial_int(), rhs.polynomial_int());
		auto lsgn = carl::sgn(carl::evaluate(g, lhs.interval_int().lower()));
		auto usgn = carl::sgn(carl::evaluate(g, lhs.interval_int().upper()));
		if (lsgn != usgn) {
			CARL_LOG_TRACE("carl.ran.interval", "gcd(lhs,rhs) has a zero in the common interval");
			lhs.set_polynomial(g, lsgn);
			rhs.set_polynomial(g, lsgn);
			return evaluate(Sign::ZERO, relation);
		} else {
			CARL_LOG_TRACE("carl.ran.interval", "gcd(lhs,rhs) has no zero in the common interval");
			if (relation == Relation::EQ) return false;
			if (relation == Relation::NEQ) return true;
			CARL_LOG_TRACE("carl.ran.interval", "Refine until intervals become disjoint");
			while (lhs.interval_int() == rhs.interval_int()) {
				lhs.refine();
				rhs.refine();
			}
		}
	}
	// now: intervals are disjoint
	CARL_LOG_TRACE("carl.ran.interval", "Intervals " << lhs.interval_int() << " and " << rhs.interval_int() << " are disjoint");
	assert(!carl::set_have_intersection(lhs.interval_int(), rhs.interval_int()));
	if (lhs.interval_int().upper() <= rhs.interval_int().lower()) {
		return relation == Relation::LESS || relation == Relation::LEQ;
	}
	if (lhs.interval_int().lower() >= rhs.interval_int().upper()) {
		return relation == Relation::GREATER || relation == Relation::GEQ;
	}

	assert(false);
	return false;
}

template<typename Number>
bool compare(const IntRepRealAlgebraicNumber<Number>& lhs, const Number& rhs, const Relation relation) {
	auto res = lhs.refine_using(rhs);
	if (res) {
		return evaluate(*res, relation);
	} else {
		if (relation == Relation::EQ)
			return false;
		else if (relation == Relation::NEQ)
			return true;
		else if (relation == Relation::LESS || relation == Relation::LEQ)
			return lhs.interval_int().upper() <= rhs;
		else if (relation == Relation::GREATER || relation == Relation::GEQ)
			return lhs.interval_int().lower() >= rhs;
	}
	assert(false);
	return false;
}



template<typename Num>
std::ostream& operator<<(std::ostream& os, const IntRepRealAlgebraicNumber<Num>& ran) {
	if (!ran.is_numeric()) {
		return os << "(IR " << ran.interval() << ", " << ran.polynomial() << ")";
	} else {
		return os << "(NR " << ran.value() << ")";
	}
}

template<typename Number>
const Variable IntRepRealAlgebraicNumber<Number>::auxVariable = fresh_real_variable("__r");

template<typename Number>
struct is_ran_type<IntRepRealAlgebraicNumber<Number>>: std::true_type {};
}

namespace std {
template<typename Number>
struct hash<carl::IntRepRealAlgebraicNumber<Number>> {
    std::size_t operator()(const carl::IntRepRealAlgebraicNumber<Number>& n) const {
		return carl::hash_all(integer_below(n));
	}
};
}
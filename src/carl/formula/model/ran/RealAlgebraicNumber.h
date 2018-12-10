#pragma once

/**
 * @file
 * Represent a real algebraic number (RAN) in one of several ways:
 * - Implicitly by a univariate polynomial and an interval.
 * - Implicitly by a polynomial and a sequence of signs (called Thom encoding).
 * - Explicitly by a rational number.
 * Rationale:
 * - A real number cannot always be adequately represented in finite memory, since
 *   it may be infinitely long. Representing
 *   it by a float or any other finite-precision representation and doing
 *   arithmatic may introduce unacceptable rouding errors.
 *   The algebraic reals, a subset of the reals, is the set of those reals that can be represented
 *   as the root of a univariate polynomial with rational coefficients so there is always
 *   an implicit, finite, full-precision
 *   representation by an univariate polynomial and an isolating interval that
 *   contains this root (only this root and no other). It is also possible
 *   to do relatively fast arithmetic with this representation without rounding errors.
 * - When the algebraic real is only finitely long prefer the rational number
 *   representation because it's faster.
 * - The idea of the Thom-Encoding is as follows: Take a square-free univariate polynomial p
 *   with degree n that has the algebraic real as its root, compute the first n-1 derivates of p,
 *   plug in this algebraic real into each derivate and only keep the sign.
 *   Then polynomial p with this sequence of signs uniquely represents this algebraic real.
 */

#include <iostream>
#include <memory>
#include <variant>

#include "../../../core/Sign.h"
#include "../../../core/UnivariatePolynomial.h"
#include "../../../core/polynomialfunctions/GCD.h"
#include "../../../core/MultivariatePolynomial.h"
#include "../../../core/polynomialfunctions/RootCounting.h"
#include "../../../core/polynomialfunctions/SturmSequence.h"
#include "../../../interval/Interval.h"
#include "../../../interval/sampling.h"

#include "RealAlgebraicNumberSettings.h"

#include "../../../thom/ThomEncoding.h"
#include "RealAlgebraicNumber_Number.h"
#include "RealAlgebraicNumber_Interval.h"
#include "RealAlgebraicNumber_Thom.h"
#include "adaption_z3/Z3Ran.h"
#include "../../../numbers/config.h"

namespace carl {

template<typename Number> class Z3Ran;
namespace ran {
	template<typename Number> struct IntervalContent;
	template<typename Number> struct equal;
	template<typename Number> struct less;
}

enum class RANSampleHeuristic { Center, CenterInt, LeftInt, RightInt, ZeroInt, InftyInt, Default = RightInt };
inline std::ostream& operator<<(std::ostream& os, RANSampleHeuristic sh) {
	switch (sh) {
		case RANSampleHeuristic::Center: return os << "Center";
		case RANSampleHeuristic::CenterInt: return os << "CenterInt";
		case RANSampleHeuristic::LeftInt: return os << "LeftInt";
		case RANSampleHeuristic::RightInt: return os << "RightInt";
		case RANSampleHeuristic::ZeroInt: return os << "ZeroInt";
		case RANSampleHeuristic::InftyInt: return os << "InftyInt";
		default: return os << "Invalid sample heuristic (" << static_cast<std::underlying_type<RANSampleHeuristic>::type>(sh) << ")";
	}
}

template<typename Number>
class RealAlgebraicNumber {
private:
	template<typename Num>
	friend RealAlgebraicNumber<Num> sampleBelow(const RealAlgebraicNumber<Num>&);
	template<typename Num>
	friend RealAlgebraicNumber<Num> sampleBetween(const RealAlgebraicNumber<Num>&, const RealAlgebraicNumber<Num>&, RANSampleHeuristic heuristic);
	template<typename Num>
	friend RealAlgebraicNumber<Num> sampleAbove(const RealAlgebraicNumber<Num>&);
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Num>& ran);
	template<typename Num>
	friend struct ran::equal;
	template<typename Num>
	friend struct ran::less;
public:
	using NumberContent = ran::NumberContent<Number>;
	using IntervalContent = ran::IntervalContent<Number>;
	using ThomContent = ran::ThomContent<Number>;
	using Z3Content = std::shared_ptr<Z3Ran<Number>>;
private:
	using Polynomial = UnivariatePolynomial<Number>;
	using Content = std::variant<
		NumberContent,
		IntervalContent,
		ThomContent,
		Z3Content
	>;

	mutable Content mContent = NumberContent();

	//mutable Number mValue = carl::constant_zero<Number>::get();
	// A flag/tag that a user of this class can set.
	// It indicates that this number stems from an outside root computation.
	bool mIsRoot = true;
  // ThomEncoding
	std::shared_ptr<ThomEncoding<Number>> mTE;

	/// Convert to a plain number if possible.
	void checkForSimplification() const {
		if (std::holds_alternative<NumberContent>(mContent)) return;
		if (std::visit(overloaded {
			[](const IntervalContent& c) { return c.is_number(); },
			[](const ThomContent& c) { return c.is_number(); },
			[](const auto& c) { return c->is_number(); }
		}, mContent)) {
			switchToNR(std::visit(overloaded {
				[](const IntervalContent& c) { return c.get_number(); },
				[](const ThomContent& c) { return c.get_number(); },
				[](const auto& c) { return c->get_number(); }
			}, mContent));
		}
	}
	// Switch to numeric representation.
	void switchToNR(Number n) const {
		mContent = NumberContent{ n };
	}

public:
	RealAlgebraicNumber() = default;
	explicit RealAlgebraicNumber(const Number& n, bool isRoot = true):
		mContent(NumberContent { n }),
		mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(Variable var, bool isRoot = true):
		mContent(std::in_place_type<IntervalContent>, Polynomial(var), Interval<Number>::zeroInterval()),
		mIsRoot(isRoot)
	{
	}
	explicit RealAlgebraicNumber(const Polynomial& p, const Interval<Number>& i, bool isRoot = true):
		mContent(std::in_place_type<IntervalContent>, p.normalized(), i),
		mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(const Polynomial& p, const Interval<Number>& i, std::vector<UnivariatePolynomial<Number>> sturmSequence, bool isRoot = true):
		mContent(std::in_place_type<IntervalContent>, p.normalized(), i, std::move(sturmSequence)),
		mIsRoot(isRoot)
	{}

	explicit RealAlgebraicNumber(const ThomEncoding<Number>& te, bool isRoot = true):
		mContent(std::in_place_type<ThomContent>, te),
		mIsRoot(isRoot)
	{
	}

	explicit RealAlgebraicNumber(const Z3Ran<Number>& zr, bool isRoot = true)
		#ifdef USE_Z3_RANS
			: mContent(std::make_shared<Z3Ran<Number>>(zr)), mIsRoot(isRoot)
		#endif
	{
		#ifndef USE_Z3_RANS
		assert(false);
		#endif
	}

	explicit RealAlgebraicNumber(Z3Ran<Number>&& zr, bool isRoot = true)
		#ifdef USE_Z3_RANS
			: mContent(std::make_shared<Z3Ran<Number>>(std::move(zr))), mIsRoot(isRoot)
		#endif
	{
		#ifndef USE_Z3_RANS
		assert(false);
		#endif
	}

	RealAlgebraicNumber(const RealAlgebraicNumber& ran) = default;
	RealAlgebraicNumber(RealAlgebraicNumber&& ran) = default;

	RealAlgebraicNumber& operator=(const RealAlgebraicNumber& n) = default;
	RealAlgebraicNumber& operator=(RealAlgebraicNumber&& n) = default;

	const auto& content() const {
		return mContent;
	}

	/**
	 * Return the size of this representation in memory in number of bits.
	 */
	std::size_t size() const {
		return std::visit(
			[](const auto& c) { return c->size(); }
		, mContent);
	}

	/**
	 * Check if this number stems from an outside root computation.
	 */
	bool isRoot() const {
		return mIsRoot;
	}

	/**
	 * Set the flag marking whether the number stems from an outside root computation.
	 */
	void setIsRoot(bool isRoot) noexcept {
		mIsRoot = isRoot;
	}

	bool isZero() const {
		return std::visit(overloaded {
			[](const IntervalContent& c) { return c.is_zero(); },
			[](const ThomContent& c) { return c.is_zero(); },
			[](const auto& c) { return c->is_zero(); }
		}, mContent);
	}

	/**
	 * Check if the underlying representation is an explicit number.
	 */
	bool isNumeric() const {
		checkForSimplification();
		return std::holds_alternative<NumberContent>(mContent);
	}
	/**
	 * Check if the underlying representation is an implict number
	 * (encoded by a polynomial and an interval).
	 */
	bool isInterval() const {
		checkForSimplification();
		return std::holds_alternative<IntervalContent>(mContent);
	}

	/**
	 * Check if the underlying representation is an implicit number
	 * that uses the Thom encoding.
	 */
	bool isThom() const noexcept {
		checkForSimplification();
		return std::holds_alternative<ThomContent>(mContent);
	}
	const ThomEncoding<Number>& getThomEncoding() const {
		assert(isThom());
		return std::get<ThomContent>(mContent).thom_encoding();
	}

	bool isZ3Ran() const {
		checkForSimplification();
		return std::holds_alternative<Z3Content>(mContent);
	}
	const Z3Ran<Number>& getZ3Ran() const {
		assert(isZ3Ran());
		return *std::get<Z3Content>(mContent);
	}

	bool isIntegral() const {
		refineToIntegrality();
		return std::visit(overloaded {
			[](const IntervalContent& c) { return c.is_integral(); },
			[](const ThomContent& c) { return c.is_integral(); },
			[](const auto& c) { return c->is_integral(); }
		}, mContent);
	}

	Number integerBelow() const {
		refineToIntegrality();
		return std::visit(overloaded {
			[](const IntervalContent& c) { return c.integer_below(); },
			[](const ThomContent& c) { return c.integer_below(); },
			[](const auto& c) { return c->integer_below(); }
		}, mContent);
	}
	
	Number branchingPoint() const {
		if (isNumeric()) return value();
		else if (isInterval()) return carl::sample(getInterval());
		else if (isZ3Ran()) return getZ3Ran().branchingPoint();
		assert(!isThom());
	}

	const Number& value() const noexcept {
		assert(isNumeric());
		return std::get<NumberContent>(mContent).value;
	}

	std::size_t getRefinementCount() const {
		assert(isInterval());
		return std::get<IntervalContent>(mContent).refinementCount();
	}
	const Interval<Number>& getInterval() const {
		if (isZ3Ran()) return getZ3Ran().getInterval();
		assert(isInterval());
		return std::get<IntervalContent>(mContent).interval();
	}
	const Number& lower() const {
		if (isZ3Ran()) getZ3Ran().lower();
		return getInterval().lower();
	}
	const Number& upper() const {
		if (isZ3Ran()) getZ3Ran().upper();
		return getInterval().upper();
	}
	const Polynomial& getIRPolynomial() const {
		if (isInterval()) return std::get<IntervalContent>(mContent).polynomial();
		else if (isZ3Ran()) return getZ3Ran().getPolynomial();
		assert(!isThom() && !isNumeric());
		return std::get<IntervalContent>(mContent).polynomial();
	}
	const auto& getIRSturmSequence() const {
		assert(isInterval());
		return std::get<IntervalContent>(mContent).sturm_sequence();
	}

	Sign sgn() const {
		return std::visit(overloaded {
			[](const NumberContent& c) { return carl::sgn(c.value); },
			[](const IntervalContent& c) { return c.sgn(); },
			[](const ThomContent& c) { return c.sgn(); },
			[](const auto& c) { return c->sgn(); }
		}, mContent);
	}

	Sign sgn(const Polynomial& p) const {
		return std::visit(overloaded {
			[&p](const NumberContent& c) { return carl::sgn(p.evaluate(c.value)); },
			[&p](const IntervalContent& c) { return c.sgn(p); },
			[&p](const ThomContent& c) { return c.sgn(p); },
			[&p](const auto& c) { return c->sgn(p); }
		}, mContent);
	}

	bool isRootOf(const UnivariatePolynomial<Number>& p) const {
		return std::visit(overloaded {
			[&p](const NumberContent& c) { return carl::count_real_roots(p, Interval<Number>(c.value)) == 1; },
			[&p](const IntervalContent& c) { return c.is_root_of(p); },
			[&p](const ThomContent& c) { return c.sgn(p) == Sign::ZERO; },
			[&p](const auto& c) { return c->sgn(p) == Sign::ZERO; }
		}, mContent);
	}

	/**
	 * Check if this (possibly implicitly represented) number lies within
	 * the bounds of interval 'i'.
	 */
	bool containedIn(const Interval<Number>& i) const {
		return std::visit(overloaded {
			[&i](const NumberContent& c) { return i.contains(c.value); },
			[&i](IntervalContent& c) { return c.contained_in(i); },
			[&i](ThomContent& c) { return c.contained_in(i); },
			[&i](const auto& c) { return c->containedIn(i); }
		}, mContent);
	}

	bool refineAvoiding(const Number& n) const {
		assert(isInterval());
		bool res = std::get<IntervalContent>(mContent).refineAvoiding(n);
		checkForSimplification();
		return res;
	}
	/// Refines until the number is either numeric or the interval does not contain any integer.
	void refineToIntegrality() const {
		std::visit(overloaded {
			[](IntervalContent& c) { c.refineToIntegrality(); },
			[](const auto& c) {}
		}, mContent);
	}
	void refine() const {
		std::visit(overloaded {
			[](IntervalContent& c) { c.refine(); },
			[](const auto& c) {}
		}, mContent);
	}

	void simplifyByPolynomial(Variable var, const MultivariatePolynomial<Number>& poly) const {
		assert(isInterval());
		UnivariatePolynomial<Number> irp(var, getIRPolynomial().template convert<Number>().coefficients());
		CARL_LOG_DEBUG("carl.ran", "gcd(" << irp << ", " << poly << ")");
		auto gmv = carl::gcd(MultivariatePolynomial<Number>(irp), poly);
		CARL_LOG_DEBUG("carl.ran", "Simplyfing, gcd = " << gmv);
		if (carl::isOne(gmv)) return;
		auto g = gmv.toUnivariatePolynomial();
		if (isRootOf(g)) {
			CARL_LOG_DEBUG("carl.ran", "Is a root of " << g);
			std::get<IntervalContent>(mContent).setPolynomial(g);
		} else {
			CARL_LOG_DEBUG("carl.ran", "Is not a root of " << g);
			CARL_LOG_DEBUG("carl.ran", "Dividing " << std::get<IntervalContent>(mContent).polynomial() << " by " << g);
			std::get<IntervalContent>(mContent).setPolynomial(std::get<IntervalContent>(mContent).polynomial().divideBy(g.replaceVariable(ran::IntervalContent<Number>::auxVariable)).quotient);
		}
	}
	
	RealAlgebraicNumber<Number> abs() const {
		if (isNumeric()) return RealAlgebraicNumber<Number>(carl::abs(value()), mIsRoot);
		if (isInterval()) {
			if (getInterval().contains(constant_zero<Number>::get())) {
				std::get<IntervalContent>(mContent).refineAvoiding(constant_zero<Number>::get());
				return abs();
			}
			if (std::get<IntervalContent>(mContent).interval().isPositive()) return *this;
			return RealAlgebraicNumber<Number>(std::get<IntervalContent>(mContent)->polynomial.negateVariable(), std::get<IntervalContent>(mContent)->interval.abs(), mIsRoot);
		}
		if (isZ3Ran()) {
			return RealAlgebraicNumber<Number>(getZ3Ran().abs());
		}
		return RealAlgebraicNumber<Number>();
	}
};

template<typename Num>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Num>& ran) {
	return std::visit(overloaded {
		[&os,&ran](const ran::IntervalContent<Num>& c) -> auto& { return os << "(" << c << (ran.isRoot() ? " R" : "") << ")"; },
		[&os,&ran](const ran::ThomContent<Num>& c) -> auto& { return os << "(" << c << (ran.isRoot() ? " R" : "") << ")"; },
		[&os,&ran](const auto& c) -> auto& { return os << "(" << *c << (ran.isRoot() ? " R" : "") << ")"; }
	}, ran.mContent);
}

}

namespace std {

	template<typename Number>
	struct hash<carl::RealAlgebraicNumber<Number>> {
		std::size_t operator()(const carl::RealAlgebraicNumber<Number>& n) const {
			return carl::hash_all(n.isRoot(), n.integerBelow());
		}
	};

}

#include "RealAlgebraicNumber_operators.h"
#include "RealAlgebraicNumberOperations.h"
#include "RealAlgebraicNumber.tpp"

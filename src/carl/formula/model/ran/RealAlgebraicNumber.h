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
#include "../../Constraint.h"
#include "../../../core/polynomialfunctions/RootCounting.h"
#include "../../../core/polynomialfunctions/SturmSequence.h"
#include "../../../interval/Interval.h"
#include "../../../interval/sampling.h"

#include "RealAlgebraicNumber_Number.h"
#include "RealAlgebraicNumber_Interval.h"
#ifdef RAN_USE_THOM
#include "RealAlgebraicNumber_Thom.h"
#endif
#ifdef RAN_USE_Z3
#include "RealAlgebraicNumber_Z3.h"
#endif
#include "../../../numbers/config.h"

namespace carl {

namespace ran {
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
	friend bool operator==(const RealAlgebraicNumber<Num>& lhs, const RealAlgebraicNumber<Num>& rhs);
	template<typename Num>
	friend bool operator<(const RealAlgebraicNumber<Num>& lhs, const RealAlgebraicNumber<Num>& rhs);
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Num>& ran);
	template<typename Num>
	friend struct ran::equal;
	template<typename Num>
	friend struct ran::less;
public:
	using NumberContent = ran::NumberContent<Number>;
	using IntervalContent = ran::IntervalContent<Number>;
#ifdef RAN_USE_THOM
	using ThomContent = ran::ThomContent<Number>;
#else
	using ThomContent = void;
#endif
#ifdef RAN_USE_Z3
	using Z3Content = ran::Z3Content<Number>;
#else
	using Z3Content = void;
#endif
private:
	using Polynomial = UnivariatePolynomial<Number>;
	using Content = std::variant<
		NumberContent,
		IntervalContent
#ifdef RAN_USE_THOM
		, ThomContent
#endif
#ifdef RAN_USE_Z3
		, Z3Content
#endif
	>;

	mutable Content mContent = NumberContent();

	// A flag/tag that a user of this class can set.
	// It indicates that this number stems from an outside root computation.
	bool mIsRoot = true;

	/// Convert to a plain number if possible.
	void checkForSimplification() const {
		if (std::holds_alternative<NumberContent>(mContent)) return;
		if (call_on_content(
			[](const auto& c) { return ran::is_number(c); }
		)) {
			switchToNR(call_on_content(
				[](const auto& c) { return ran::get_number(c); }
			));
		}
	}
	// Switch to numeric representation.
	void switchToNR(Number n) const {
		mContent = NumberContent{ n };
	}

	template<typename... F>
	auto call_on_content(F&&... f) const {
		return std::visit(overloaded {
				std::forward<F>(f)...
			},
			mContent
		);
	}

public:
	RealAlgebraicNumber() = default;
	explicit RealAlgebraicNumber(const Number& n, bool isRoot = true):
		mContent(std::in_place_type<NumberContent>, n),
		mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(Variable var, bool isRoot = true):
		mContent(std::in_place_type<IntervalContent>, Polynomial(var), Interval<Number>::zeroInterval()),
		mIsRoot(isRoot)
	{
	}
	explicit RealAlgebraicNumber(const Polynomial& p, const Interval<Number>& i, bool isRoot = true):
		mContent(std::in_place_type<IntervalContent>, p, i),
		mIsRoot(isRoot)
	{}
	static RealAlgebraicNumber createSafeIR(const Polynomial& p, const Interval<Number>& i, bool isRoot = true) {
		return RealAlgebraicNumber(carl::squareFreePart(p), i, isRoot);
	}
	explicit RealAlgebraicNumber(const NumberContent& ran, bool isRoot = true):
		mContent(ran), mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(const IntervalContent& ran, bool isRoot = true):
		mContent(ran), mIsRoot(isRoot)
	{}
#ifdef RAN_USE_THOM
	explicit RealAlgebraicNumber(const ThomEncoding<Number>& te, bool isRoot = true):
		mContent(std::in_place_type<ThomContent>, te),
		mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(const ThomContent& ran, bool isRoot = true):
		mContent(ran), mIsRoot(isRoot)
	{}
#endif

#ifdef RAN_USE_Z3
	explicit RealAlgebraicNumber(const Z3Content& ran, bool isRoot = true):
		mContent(ran), mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(const Z3Ran<Number>& zr, bool isRoot = true)
		: mContent(std::in_place_type<Z3Content>, zr), mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(Z3Ran<Number>&& zr, bool isRoot = true)
		: mContent(std::in_place_type<Z3Content>, std::move(zr)), mIsRoot(isRoot)
	{}
#endif

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
		return call_on_content(
			[](const auto& c) { return c.size(); }
		);
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
		return call_on_content(
			[](const auto& c) { return c.is_zero(); }
		);
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

#ifdef RAN_USE_THOM
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
#endif

#ifdef RAN_USE_Z3
	bool isZ3Ran() const {
		checkForSimplification();
		return std::holds_alternative<Z3Content>(mContent);
	}
	const Z3Ran<Number>& getZ3Ran() const {
		assert(isZ3Ran());
		return std::get<Z3Content>(mContent).z3_ran();
	}
#endif

	bool isIntegral() const {
		return call_on_content(
			[](const auto& c) { return c.is_integral(); }
		);
	}

	Number integerBelow() const {
		return call_on_content(
			[](const auto& c) { return c.integer_below(); }
		);
	}
	
	Number branchingPoint() const {
		return call_on_content(
			[](const auto& c) { return ran::branching_point(c); }
		);
	}

	const Number& value() const noexcept {
		assert(isNumeric());
		return std::get<NumberContent>(mContent).value();
	}

	std::size_t getRefinementCount() const {
		assert(isInterval());
		return std::get<IntervalContent>(mContent).refinementCount();
	}
	Interval<Number> getInterval() const {
		return call_on_content(
			[](const auto& c) { return ran::get_interval(c); }
		);
	}
	//const Number& lower() const {
	//	return std::visit(overloaded {
	//		[](const auto& c) { ran::lower(c); }
	//	}, mContent);
	//}
	//const Number& upper() const {
	//	return std::visit(overloaded {
	//		[](const auto& c) { ran::upper(c); }
	//	}, mContent);
	//}
	Polynomial getIRPolynomial() const {
		return call_on_content(
			[](const auto& c) { return ran::get_polynomial(c); }
		);
	}

	Sign sgn() const {
		return call_on_content(
			[](const auto& c) { return c.sgn(); }
		);
	}

	Sign sgn(const Polynomial& p) const {
		return call_on_content(
			[&p](const auto& c) { return c.sgn(p); }
		);
	}

	bool isRootOf(const UnivariatePolynomial<Number>& p) const {
		return call_on_content(
			[&p](const NumberContent& c) { return p.sgn(c.value()) == carl::Sign::ZERO; },
			[&p](const IntervalContent& c) { return c.is_root_of(p); },
			[&p](const auto& c) { return c.sgn(p) == Sign::ZERO; }
		);
	}

	/**
	 * Check if this (possibly implicitly represented) number lies within
	 * the bounds of interval 'i'.
	 */
	bool containedIn(const Interval<Number>& i) const {
		return call_on_content(
			[&i](auto& c) { return c.contained_in(i); }
		);
	}

	bool refineAvoiding(const Number& n) const {
		assert(isInterval());
		bool res = std::get<IntervalContent>(mContent).refineAvoiding(n);
		checkForSimplification();
		return res;
	}
	/// Refines until the number is either numeric or the interval does not contain any integer.
	void refineToIntegrality() const {
		return call_on_content(
			[](IntervalContent& c) { c.refineToIntegrality(); },
			[](const auto& c) {}
		);
	}
	void refine() const {
		return call_on_content(
			[](IntervalContent& c) { c.refine(); },
			[](const auto& c) {}
		);
	}

	RealAlgebraicNumber<Number> abs() const {
		return call_on_content(
			[this](const auto& c) { return RealAlgebraicNumber<Number>(ran::abs(c), mIsRoot); }
		);
	}
};

template<typename Num>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Num>& ran) {
	return std::visit(overloaded {
		[&os,&ran](const auto& c) -> auto& { return os << "(" << c << (ran.isRoot() ? " R" : "") << ")"; }
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

#pragma once

#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include "../Ran.h"

#include <carl-arith/interval/set_theory.h>
#include <carl-arith/interval/sampling.h>
#include <carl-arith/poly/umvpoly/functions/Factorization_univariate.h>
#include <carl-arith/poly/umvpoly/functions/SignVariations.h>
#include <carl-common/util/streamingOperators.h>
#include <carl-arith/poly/umvpoly/functions/EigenWrapper.h>
#include <carl-arith/poly/umvpoly/functions/Evaluation.h>
#include <carl-arith/poly/umvpoly/functions/RootElimination.h>

namespace carl::ran::interval {

using carl::operator<<;

/**
 * Compact class to isolate real roots from a univariate polynomial using bisection.
 * 
 * After some rather easy preprocessing (make polynomial square-free, eliminate zero roots, solve low-degree polynomial trivially, use root bounds to shrink the interval) 
 * we employ bisection which can optionally be initialized by approximations.
 */
template<typename Number>
class RealRootIsolation {
	/// Initialize bisection intervals using approximations.
	static constexpr bool initialize_bisection_by_approximation = true;
	/// Factorize polynomial and handle factors individually.
	static constexpr bool simplify_by_factorization = false;

	/// The polynomial.
	UnivariatePolynomial<Number> mPolynomial;
	/// The list of roots.
	std::vector<IntRepRealAlgebraicNumber<Number>> mRoots;
	/// The bounding interval.
	Interval<Number> mInterval;
	/// The sturm sequence for mPolynomial.
	// std::optional<std::vector<UnivariatePolynomial<Number>>> mSturmSequence;

	/// Return the sturm sequence for mPolynomial, create it if necessary.
	// const auto& sturm_sequence() {
	// 	if (!mSturmSequence) {
	// 		mSturmSequence = carl::sturm_sequence(mPolynomial);
	// 	}
	// 	return *mSturmSequence;
	// }
	/// Reset the sturm sequence, used if the polynomial was modified.
	// void reset_sturm_sequence() {
	// 	mSturmSequence.reset();
	// }

	/// Handle zero roots (p(0) == 0)
	void eliminate_zero_roots() {
		if (mPolynomial.zero_is_root()) {
			if (mInterval.contains(0)) {
				mRoots.emplace_back(0);
			}
			eliminate_zero_root(mPolynomial);
		}
	}

	/// Directly solve low-degree polynomials.
	bool isolate_roots_trivially() {
		CARL_LOG_DEBUG("carl.ran.interval", "Trying to trivially solve mPolynomial " << mPolynomial);
		switch (mPolynomial.degree()) {
			case 0: {
				CARL_LOG_TRACE("carl.ran.interval", "Constant polynomial, thus no roots");
				break;
			}
			case 1: {
				CARL_LOG_DEBUG("carl.ran.interval", "Trivially solving linear mPolynomial " << mPolynomial);
				const auto& a = mPolynomial.coefficients()[1];
				const auto& b = mPolynomial.coefficients()[0];
				assert(!carl::is_zero(a));
				add_trivial_root(-b / a);
				break;
			}
			case 2: {
				CARL_LOG_DEBUG("carl.ran.interval", "Trivially solving quadratic mPolynomial " << mPolynomial);
				const auto& a = mPolynomial.coefficients()[2];
				const auto& b = mPolynomial.coefficients()[1];
				const auto& c = mPolynomial.coefficients()[0];
				assert(!carl::is_zero(a));
				/* Use this formulation of p-q-formula:
				* x = ( -b +- \sqrt{ b*b - 4*a*c } ) / (2*a)
				*/
				Number rad = b*b - 4*a*c;
				if (rad == 0) {
					add_trivial_root(-b / (2*a));
				} else if (rad > 0) {
					std::pair<Number, Number> res = carl::sqrt_fast(rad);
					if (res.first == res.second) {
						// Root could be calculated exactly
						add_trivial_root((-b - res.first) / (2*a));
						add_trivial_root((-b + res.first) / (2*a));
					} else {
						// Root is within interval (res.first, res.second)
						Interval<Number> r(res.first, BoundType::STRICT, res.second, BoundType::STRICT);
						add_trivial_root((Number(-b) - r) / Number(2*a));
						add_trivial_root((Number(-b) + r) / Number(2*a));
					}
				} else {
					// No root.
				}
				break;
			}
			default:
				return false;
		}
		return true;
	}

	void add_trivial_root(const Number& n) {
		CARL_LOG_TRACE("carl.ran.interval", "Add trivial root " << n);
		assert(carl::is_root_of(mPolynomial, n));
		mRoots.emplace_back(n);
	}

	void add_trivial_root(const Interval<Number>& i) {
		CARL_LOG_TRACE("carl.ran.interval", "Add trivial root " << i);
		mRoots.emplace_back(mPolynomial, i);
	}

	/// Use root bounds to shrink mInterval.
	void update_root_bounds() {
		auto bound = carl::lagrangeBound(mPolynomial);
		mInterval = carl::set_intersection(mInterval, carl::Interval<Number>(-bound, bound));
		CARL_LOG_DEBUG("carl.ran.interval", "Updated bounds to " << mInterval);
	}

	/// Add a root to mRoots and simplify polynomial accordingly (essentially divide by x-n)
	void add_root(const Number& n) {
		CARL_LOG_TRACE("carl.ran.interval", "Add root " << n);
		assert(carl::is_root_of(mPolynomial, n));
		// reset_sturm_sequence();
		eliminate_root(mPolynomial, n);
		mRoots.emplace_back(n);
	}
	/// Add a root to mRoots, based on an isolating interval.
	void add_root(const Interval<Number>& i) {
		CARL_LOG_TRACE("carl.ran.interval", "Add root " << i);
		mRoots.emplace_back(mPolynomial, i);
	}

	/// Check whether the interval bounds are roots.
	bool check_interval_bounds() {
		bool found_root = false;
		if (mInterval.lower_bound_type() == BoundType::WEAK) {
			if (carl::is_root_of(mPolynomial, mInterval.lower())) {
				add_root(mInterval.lower());
				found_root = true;
			}
		}
		if (mInterval.upper_bound_type() == BoundType::WEAK) {
			if (carl::is_root_of(mPolynomial, mInterval.upper())) {
				add_root(mInterval.upper());
				found_root = true;
			}
		}
		return found_root;
	}

	/**
	 * Initialize the bisection queue using approximations.
	 * 
	 * The main idea is that the eigenvalues of the companion matrix are the root of a polynomial.
	 * This is implemented in eigen::root_approximation.
	 * We do:
	 * - convert coefficients to doubles
	 * - call eigen::root_approximation
	 * - make approximations smaller, sort them, remove duplicates
	 * - convert approximations to rationals
	 * - create interval endpoints so that each interval contains a single approximation
	 * - initialize queue from these endpoints
	 */
	void bisect_by_approximation(std::deque<Interval<Number>>& queue) {
		assert(queue.empty());
		// Convert polynomial coeffs to double
		std::vector<double> coeffs;
		for (const auto& n: mPolynomial.coefficients()) {
			coeffs.emplace_back(to_double(n));
		}
		CARL_LOG_DEBUG("carl.ran.interval", "Double coeffs: " << coeffs);
		
		// Get approximations of the roots
		std::vector<double> approx = carl::roots::eigen::root_approximation(coeffs);
		// Sort and make them unique
		for (double& a: approx) {
			a = std::round(a*1000) / 1000;
		}
		std::sort(approx.begin(), approx.end());
		approx.erase(std::unique(approx.begin(), approx.end()), approx.end());
		CARL_LOG_DEBUG("carl.ran.interval", "Double approximations: " << approx);
		// Convert to rationals
		std::vector<Number> roots;
		for (double r: approx) {
			if (!is_number(r)) continue;
			Number n = carl::rationalize<Number>(r);
			if (!mInterval.contains(n)) continue;
			if (carl::is_root_of(mPolynomial, n)) {
				add_root(n);
			}
			roots.emplace_back(n);
		}
		CARL_LOG_DEBUG("carl.ran.interval", "Approx roots: " << roots);

		// Get interval endpoints
		std::vector<Number> endpoints;
		endpoints.emplace_back(mInterval.lower());
		if (roots.size() > 0) {
			for (std::size_t i = 0; i < roots.size() - 1; ++i) {
				auto tmp = carl::sample(Interval<Number>(roots[i], BoundType::STRICT, roots[i+1], BoundType::STRICT));
				if (carl::is_root_of(mPolynomial, tmp)) {
					add_root(tmp);
				}
				endpoints.emplace_back(tmp);
			}
		}
		endpoints.emplace_back(mInterval.upper());
		CARL_LOG_DEBUG("carl.ran.interval", "Endpoints: " << endpoints);

		// Fill queue based on the endpoints
		for (std::size_t i = 0; i < endpoints.size() - 1; ++i) {
			queue.emplace_back(endpoints[i], BoundType::STRICT, endpoints[i+1], BoundType::STRICT);
		}
		CARL_LOG_DEBUG("carl.ran.interval", "Queue: " << queue);
	}

	/// Perform bisection
	void isolate_by_bisection() {
		std::deque<Interval<Number>> queue;
		if (initialize_bisection_by_approximation) {
			bisect_by_approximation(queue);
		} else {
			queue.emplace_back(mInterval);
		}

		while (!queue.empty()) {
			auto cur = queue.front();
			queue.pop_front();

			auto variations = carl::sign_variations(mPolynomial, cur);
			
			if (variations == 0) {
				CARL_LOG_DEBUG("carl.ran.interval", "No root within " << cur);
				continue;
			}
			if (variations == 1) {
				CARL_LOG_DEBUG("carl.ran.interval", "A single root within " << cur);
				assert(!carl::is_root_of(mPolynomial, cur.lower()));
				assert(!carl::is_root_of(mPolynomial, cur.upper()));
				assert(count_real_roots(mPolynomial, cur) == 1);
				add_root(cur);
				continue;
			}

			auto pivot = carl::sample(cur);
			if (carl::is_root_of(mPolynomial, pivot)) {
				add_root(pivot);
			}
			CARL_LOG_DEBUG("carl.ran.interval", "Splitting " << cur << " at " << pivot);
			queue.emplace_back(cur.lower(), BoundType::STRICT, pivot, BoundType::STRICT);
			queue.emplace_back(pivot, BoundType::STRICT, cur.upper(), BoundType::STRICT);
		}
	}

	/// Do actual root isolation.
	void compute_roots() {
		// Handle zero polynomial
		if (carl::is_zero(mPolynomial)) {
			return;
		}
		// Check for p(0) == 0
		eliminate_zero_roots();
		assert(!carl::is_zero(mPolynomial));
		// Handle other easy cases
		while (true) {
			// Degree of at most 2 -> use p-q-formula
			if (isolate_roots_trivially()) {
				return;
			}
			// Use bounds to make interval smaller
			update_root_bounds();
			// Check whether interval bounds are roots
			if (!check_interval_bounds()) {
				break;
			}
		}

		// Now do actual bisection
		isolate_by_bisection();
	}

public:
	RealRootIsolation(const UnivariatePolynomial<Number>& polynomial, const Interval<Number>& interval): mPolynomial(carl::squareFreePart(polynomial)), mInterval(interval) {
		CARL_LOG_DEBUG("carl.ran.interval", "Reduced " << polynomial << " to " << mPolynomial);
	}

	/// Compute and sort the roots of mPolynomial within mInterval.
	std::vector<IntRepRealAlgebraicNumber<Number>> get_roots() {
		if (simplify_by_factorization) {
			auto factors = carl::factorization(mPolynomial);
			CARL_LOG_DEBUG("carl.ran.interval", "Factorized " << mPolynomial << " to " << factors);
			auto interval = mInterval;
			for (const auto& factor: factors) {
				CARL_LOG_DEBUG("carl.ran.interval", "Coputing root of factor " << factor);
				mPolynomial = factor.first;
				mInterval = interval;
				// reset_sturm_sequence();
				compute_roots();
			}
		} else {
			compute_roots();
		}
		std::sort(mRoots.begin(), mRoots.end());
		return mRoots;
	}

};

}
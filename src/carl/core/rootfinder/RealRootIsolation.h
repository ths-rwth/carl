#pragma once

#include "../UnivariatePolynomial.h"
#include "../../formula/model/ran/RealAlgebraicNumber.h"
#include "../../interval/set_theory.h"
#include "../../interval/sampling.h"
#include "../polynomialfunctions/SignVariations.h"
#include "../../io/streamingOperators.h"

namespace carl::rootfinder {

using carl::operator<<;

template<typename Number>
class RealRootIsolation {
	static constexpr bool initialize_bisection_by_eigenvalues = true;

	UnivariatePolynomial<Number> mPolynomial;
	std::vector<RealAlgebraicNumber<Number>> mRoots;
	Interval<Number> mInterval;
	std::optional<std::vector<UnivariatePolynomial<Number>>> mSturmSequence;

	const auto& sturm_sequence() {
		if (!mSturmSequence) {
			mSturmSequence = carl::sturm_sequence(mPolynomial);
		}
		return *mSturmSequence;
	}
	void reset_sturm_sequence() {
		mSturmSequence.reset();
	}

	void isolate_zero_roots() {
		if (mPolynomial.zeroIsRoot()) {
			if (mInterval.contains(0)) {
				mRoots.emplace_back(0);
			}
			mPolynomial.eliminateZeroRoots();
		}
	}

	bool isolate_roots_trivially() {
		CARL_LOG_DEBUG("carl.core.rootfinder", "Trying to trivially solve mPolynomial " << mPolynomial);
		switch (mPolynomial.degree()) {
			case 0: break;
			case 1: {
				CARL_LOG_DEBUG("carl.core.rootfinder", "Trivially solving linear mPolynomial " << mPolynomial);
				const auto& a = mPolynomial.coefficients()[1];
				const auto& b = mPolynomial.coefficients()[0];
				assert(!carl::isZero(a));
				mRoots.emplace_back(-b / a);
				break;
			}
			case 2: {
				CARL_LOG_DEBUG("carl.core.rootfinder", "Trivially solving quadratic mPolynomial " << mPolynomial);
				const auto& a = mPolynomial.coefficients()[2];
				const auto& b = mPolynomial.coefficients()[1];
				const auto& c = mPolynomial.coefficients()[0];
				assert(!carl::isZero(a));
				/* Use this formulation of p-q-formula:
				* x = ( -b +- \sqrt{ b*b - 4*a*c } ) / (2*a)
				*/
				Number rad = b*b - 4*a*c;
				if (rad == 0) {
					mRoots.emplace_back(-b / (2*a));
				} else if (rad > 0) {
					std::pair<Number, Number> res = carl::sqrt_fast(rad);
					if (res.first == res.second) {
						// Root could be calculated exactly
						mRoots.emplace_back((-b - res.first) / (2*a));
						mRoots.emplace_back((-b + res.first) / (2*a));
					} else {
						// Root is within interval (res.first, res.second)
						Interval<Number> r(res.first, BoundType::STRICT, res.second, BoundType::STRICT);
						mRoots.emplace_back(mPolynomial, (Number(-b) - r) / Number(2*a), sturm_sequence());
						mRoots.emplace_back(mPolynomial, (Number(-b) + r) / Number(2*a), sturm_sequence());
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

	void update_root_bounds() {
		auto bound = carl::cauchyBound(mPolynomial);
		mInterval = carl::set_intersection(mInterval, carl::Interval<Number>(-bound, bound));
		CARL_LOG_DEBUG("carl.core.rootfinder", "Updated bounds to " << mInterval);
	}

	void add_root(const Number& n) {
		assert(mPolynomial.isRoot(n));
		reset_sturm_sequence();
		mPolynomial.eliminateRoot(n);
		mRoots.emplace_back(n);
	}
	void add_root(const Interval<Number>& i) {
		mRoots.emplace_back(mPolynomial, i, sturm_sequence());
	}

	bool check_interval_bounds() {
		bool found_root = false;
		if (mInterval.lowerBoundType() == BoundType::WEAK) {
			if (mPolynomial.isRoot(mInterval.lower())) {
				add_root(mInterval.lower());
				found_root = true;
			}
		}
		if (mInterval.upperBoundType() == BoundType::WEAK) {
			if (mPolynomial.isRoot(mInterval.upper())) {
				add_root(mInterval.upper());
				found_root = true;
			}
		}
		return found_root;
	}

	void bisect_by_eigenvalues(std::vector<Interval<Number>>& queue) {
		assert(queue.empty());
		// Convert polynomial coeffs to double
		std::vector<double> coeffs;
		for (const auto& n: mPolynomial.coefficients()) {
			coeffs.emplace_back(toDouble(n));
		}
		CARL_LOG_DEBUG("carl.core.rootfinder", "Double coeffs: " << coeffs);
		
		// Get approximations of the roots
		std::vector<double> approx = eigen::root_approximation(coeffs);
		// Sort and make them unique
		for (double& a: approx) {
			a = std::round(a*1000) / 1000;
		}
		std::sort(approx.begin(), approx.end());
		approx.erase(std::unique(approx.begin(), approx.end()), approx.end());
		CARL_LOG_DEBUG("carl.core.rootfinder", "Double approximations: " << approx);
		// Convert to rationals
		std::vector<Number> roots;
		for (double r: approx) {
			if (!isNumber(r)) continue;
			Number n = carl::rationalize<Number>(r);
			if (!mInterval.contains(n)) continue;
			if (mPolynomial.isRoot(n)) {
				add_root(n);
			}
			roots.emplace_back(n);
		}
		CARL_LOG_DEBUG("carl.core.rootfinder", "Approx roots: " << roots);

		// Get interval endpoints
		std::vector<Number> endpoints;
		endpoints.emplace_back(mInterval.lower());
		for (std::size_t i = 0; i < roots.size() - 1; ++i) {
			auto tmp = carl::sample(Interval<Number>(roots[i], BoundType::STRICT, roots[i+1], BoundType::STRICT));
			if (mPolynomial.isRoot(tmp)) {
				add_root(tmp);
			}
			endpoints.emplace_back(tmp);
		}
		endpoints.emplace_back(mInterval.upper());
		CARL_LOG_DEBUG("carl.core.rootfinder", "Endpoints: " << endpoints);

		// Fill queue based on the endpoints
		for (std::size_t i = 0; i < endpoints.size() - 1; ++i) {
			queue.emplace_back(endpoints[i], BoundType::STRICT, endpoints[i+1], BoundType::STRICT);
		}
		CARL_LOG_DEBUG("carl.core.rootfinder", "Queue: " << queue);
	}

	void isolate_by_bisection() {
		std::vector<Interval<Number>> queue;
		if (initialize_bisection_by_eigenvalues) {
			bisect_by_eigenvalues(queue);
		} else {
			queue.emplace_back(mInterval);
		}

		while (!queue.empty()) {
			auto cur = queue.back();
			queue.pop_back();

			auto variations = carl::sign_variations(mPolynomial, cur);
			
			if (variations == 0) {
				CARL_LOG_DEBUG("carl.core.rootfinder", "No root within " << cur);
				continue;
			}
			if (variations == 1) {
				CARL_LOG_DEBUG("carl.core.rootfinder", "A single root within " << cur);
				assert(!mPolynomial.isRoot(cur.lower()));
				assert(!mPolynomial.isRoot(cur.upper()));
				assert(count_real_roots(mPolynomial, cur) == 1);
				add_root(cur);
				continue;
			}

			auto pivot = carl::sample(cur);
			if (mPolynomial.isRoot(pivot)) {
				add_root(pivot);
			}
			CARL_LOG_DEBUG("carl.core.rootfinder", "Splitting " << cur << " at " << pivot);
			queue.emplace_back(cur.lower(), BoundType::STRICT, pivot, BoundType::STRICT);
			queue.emplace_back(pivot, BoundType::STRICT, cur.upper(), BoundType::STRICT);
		}
	}

public:
	RealRootIsolation(const UnivariatePolynomial<Number>& polynomial, const Interval<Number>& interval): mPolynomial(carl::squareFreePart(polynomial)), mInterval(interval) {
		CARL_LOG_DEBUG("carl.core.rootfinder", "Reduced " << polynomial << " to " << mPolynomial);
	}

	std::vector<RealAlgebraicNumber<Number>> get_roots() {
		// Check for p(0) == 0
		isolate_zero_roots();
		// Handle other easy cases
		while (true) {
			// Degree of at most 2 -> use p-q-formula
			if (isolate_roots_trivially()) {
				return mRoots;
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

		return mRoots;
	}

};

}
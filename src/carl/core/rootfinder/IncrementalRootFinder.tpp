/**
 * @file IncrementalRootFinder.tpp
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../../util/debug.h"
#include "../logging.h"
#include "AbstractRootFinder.h"
#include "RootFinder.h"

#ifdef __VS
	#include <Eigen/Dense>
	#include <Eigen/Eigenvalues>
#else
	#include <eigen3/Eigen/Dense>
	#include <eigen3/Eigen/Eigenvalues>
#endif

namespace carl {
namespace rootfinder {

template<typename Number, typename C>
IncrementalRootFinder<Number, C>::IncrementalRootFinder(
		const UnivariatePolynomial<Number>& polynomial,
		const Interval<Number>& interval,
		SplittingStrategy strategy,
		bool tryTrivialSolver
		) :
		AbstractRootFinder<Number>(polynomial, interval, tryTrivialSolver),
		splittingStrategy(strategy),
		nextRoot(this->roots.end())
{
	if (!this->interval.isEmpty()) {
		this->addQueue(this->interval, splittingStrategy);
	}
}

template<typename Number, typename C>
void IncrementalRootFinder<Number, C>::findRoots() {
	while (this->processQueueItem()) {}
	this->setFinished();
}

template<typename Number, typename C>
bool IncrementalRootFinder<Number, C>::hasNext() {
	while (this->nextRoot == this->roots.end()) {
		if (!this->processQueueItem()) {
			this->setFinished();
			return false;
		}
	}
	return true;
}

template<typename Number, typename C>
RealAlgebraicNumber<Number> IncrementalRootFinder<Number, C>::next() {
	assert(this->nextRoot != this->roots.end());
	auto tmp = this->nextRoot++;
	return *tmp;
}


template<typename Number, typename C>
bool IncrementalRootFinder<Number, C>::processQueueItem() {
	if (this->queue.empty()) {
		return false;
	}
	QueueItem item = this->queue.top();
	this->queue.pop();

	Interval<Number> interval = std::get<0>(item);
	SplittingStrategy strategy = std::get<1>(item);
	

	if (strategy == SplittingStrategy::EIGENVALUES) {
		splittingStrategies::EigenValueStrategy<Number>::getInstance()(interval, *this);
		return true;
	} else if (strategy == SplittingStrategy::ABERTH) {
		//AberthStrategy<Number>::instance()(interval, *this);
		//return true;
	}

	if (interval.contains(0)) {
		if (this->polynomial.evaluate(0) == 0) this->addRoot(RealAlgebraicNumber<Number>(0));
		this->addQueue(Interval<Number>(interval.lower(), BoundType::STRICT, 0, BoundType::STRICT), strategy);
		this->addQueue(Interval<Number>(0, BoundType::STRICT, interval.upper(), BoundType::STRICT), strategy);
		return true;
	}

	uint variations = this->polynomial.signVariations(interval);

	if (variations == 0) return true;
	if (variations == 1) {
		// If one of the interval bounds is a root, sturm sequences will break. Hence we continue splitting.
		if (!this->polynomial.isRoot(interval.lower()) && !this->polynomial.isRoot(interval.upper())) {
			assert(this->polynomial.countRealRoots(interval) == 1);
			this->addRoot(interval);
			return true;
		}
	}

	//std::cerr << "calling strategy " << strategy << std::endl;
	switch (strategy) {
		case SplittingStrategy::GENERIC: splittingStrategies::GenericStrategy<Number>::getInstance()(interval, *this);
			break;
		case SplittingStrategy::EIGENVALUES:	// Should not happen, safe fallback anyway
		case SplittingStrategy::ABERTH:		// Should not happen, safe fallback anyway
		case SplittingStrategy::BINARYSAMPLE: splittingStrategies::BinarySampleStrategy<Number>::getInstance()(interval, *this);
			break;
		case SplittingStrategy::BINARYNEWTON: splittingStrategies::BinaryNewtonStrategy<Number>::getInstance()(interval, *this);
			break;
		case SplittingStrategy::GRID: //GridStrategy<Number>::instance()(interval, *this);
			break;
	}
	return true;
}

/**
 * Creates a (guessed) isolation based on approximations for the real roots.
 * @param roots Approximation for the real roots.
 * @param interval Interval bounding the real roots.
 * @param finder Root finder.
 */
template<typename Number>
void buildIsolation(std::vector<double>&& doubleRoots, const Interval<Number>& interval, RootFinder<Number>& finder) {
	assert(interval.lower() < interval.upper());
	std::sort(doubleRoots.begin(), doubleRoots.end());
	auto uniqueIt = std::unique(doubleRoots.begin(), doubleRoots.end());
	doubleRoots.resize(size_t(std::distance(doubleRoots.begin(), uniqueIt)));
	std::vector<Number> roots;
	for (const auto& droot: doubleRoots) {
		if (!isNumber(droot)) continue;
		Number n = carl::rationalize<Number>(droot);
		if (!interval.contains(n)) continue;
		if (roots.size() > 0 && n - roots.back() < 1) continue;
		roots.push_back(n);
	}

	std::vector<Number> res;
	res.reserve(roots.size() + 3);
	
    #ifdef USE_CLN_NUMBERS
	try {
    #endif
		res.push_back(interval.lower());
		if (roots.size() == 1) {
			Number tmp = carl::floor(roots[0]);
			if (interval.contains(tmp)) res.push_back(tmp);
			tmp = carl::ceil(roots[0]);
			if (interval.contains(tmp)) res.push_back(tmp);
		} else if (roots.size() > 1) {
			Number tmp = 2 * roots[0] - roots[1];
			if (interval.contains(tmp)) res.push_back(tmp);
			for (std::size_t i = 0; i < roots.size()-1; i++) {
				if (interval.contains(roots[i]) && finder.getPolynomial().evaluate(roots[i]) == 0) {
					res.push_back(roots[i]);
				}
				Number tmpSample = Interval<Number>(roots[i], BoundType::STRICT, roots[i+1], BoundType::STRICT).sample();
				if (interval.contains(tmpSample)) res.push_back(tmpSample);
			}
			if (interval.contains(roots.back()) && finder.getPolynomial().evaluate(roots.back()) == 0) {
				res.push_back(roots.back());
			}
			tmp = 2 * roots.back() - roots[roots.size()-2];
			if (interval.contains(tmp)) res.push_back(tmp);
		}
		res.push_back(interval.upper());
	///@todo Add carl::floating_point_exception and use this here.
    #ifdef USE_CLN_NUMBERS
	} catch (cln::floating_point_exception) {
		finder.addQueue(interval, SplittingStrategy::BINARYSAMPLE);
		return;
	}
    #endif

	assert(res[0] <= res[1]);
	if (res[0] < res[1]) {
		finder.addQueue(Interval<Number>(res[0], BoundType::STRICT, res[1], BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
	}
	for (std::size_t i = 1; i < res.size()-1; i++) {
		if (finder.getPolynomial().evaluate(res[i]) == 0) {
			finder.addRoot(RealAlgebraicNumber<Number>(res[i]));
		}
		assert(res[i] <= res[i+1]);
		if (res[i] < res[i+1]) {
			finder.addQueue(Interval<Number>(res[i], BoundType::STRICT, res[i+1], BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
		}
	}	
}

namespace splittingStrategies {

template<typename Number>
void GenericStrategy<Number>::operator()(const Interval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.center();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(RealAlgebraicNumber<Number>(pivot));
	}
	finder.addQueue(Interval<Number>(interval.lower(), BoundType::STRICT, pivot, BoundType::STRICT), SplittingStrategy::GENERIC);
	finder.addQueue(Interval<Number>(pivot, BoundType::STRICT, interval.upper(), BoundType::STRICT), SplittingStrategy::GENERIC);
}

template<typename Number>
void BinarySampleStrategy<Number>::operator()(const Interval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.sample();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(RealAlgebraicNumber<Number>(pivot));
	}
	finder.addQueue(Interval<Number>(interval.lower(), BoundType::STRICT, pivot, BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
	finder.addQueue(Interval<Number>(pivot, BoundType::STRICT, interval.upper(), BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
}

template<typename Number>
void BinaryNewtonStrategy<Number>::operator()(const Interval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.sample();
	Number diff = finder.getPolynomial().derivative().evaluate(pivot);
	if (diff != 0) {
		pivot -= finder.getPolynomial().evaluate(pivot) / diff;
	}
	if (!interval.contains(pivot)) pivot = interval.sample();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(RealAlgebraicNumber<Number>(pivot));
	}
	finder.addQueue(Interval<Number>(interval.lower(), BoundType::STRICT, pivot, BoundType::STRICT), SplittingStrategy::BINARYNEWTON);
	finder.addQueue(Interval<Number>(pivot, BoundType::STRICT, interval.upper(), BoundType::STRICT), SplittingStrategy::BINARYNEWTON);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	os << "[" << v.size() << ": ";
	bool first = true;
	for (auto it: v) {
		if (!first) os << ", ";
		first = false;
		os << it;
	}
	return os << "]";
}

template<typename Number>
void EigenValueStrategy<Number>::operator()(const Interval<Number>& interval, RootFinder<Number>& finder) {
	UnivariatePolynomial<Number> p = finder.getPolynomial();
	
	using Index = Eigen::MatrixXd::Index;
	// Create companion matrix
	uint degree = p.degree();
	Eigen::MatrixXd m = Eigen::MatrixXd::Zero(Index(degree), Index(degree));
	m(0, Index(degree)-1) = toDouble(Number(-p.coefficients()[0] / p.coefficients()[degree]));
	for (uint i = 1; i < degree; i++) {
		m(Index(i), Index(i)-1) = 1;
		m(Index(i), Index(degree)-1) = toDouble(Number(-p.coefficients()[i] / p.coefficients()[degree]));
	}
	
	// Obtain eigenvalues
	Eigen::VectorXcd eigenvalues = m.eigenvalues();
	
	// Save real parts to tmp
	std::vector<double> tmp(std::size_t(eigenvalues.size()));
	for (uint i = 0; i < std::size_t(eigenvalues.size()); i++) {
		if (eigenvalues[Index(i)].imag() > eigenvalues[Index(i)].real() / 4) tmp[i] = 0;
		else tmp[i] = eigenvalues[Index(i)].real();
	}
	
	// Build isolation
	buildIsolation(std::move(tmp), interval, finder);
}

}

}
}

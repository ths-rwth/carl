/**
 * @file IncrementalRootFinder.tpp
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>

#include "AbstractRootFinder.h"
#include "RootFinder.h"
#include "../logging.h"
#include "../../util/debug.h"

namespace carl {
namespace rootfinder {

template<typename Number, typename C>
IncrementalRootFinder<Number, C>::IncrementalRootFinder(
		const UnivariatePolynomial<Number>& polynomial,
		const Interval<Number>& interval,
		SplittingStrategy splittingStrategy,
		bool tryTrivialSolver
		) :
		AbstractRootFinder<Number>(polynomial, interval, tryTrivialSolver),
		splittingStrategy(splittingStrategy),
		nextRoot(this->roots.end())
{
	if (!this->interval.isEmpty()) {
		this->addQueue(this->interval, this->splittingStrategy);
	}
}

template<typename Number, typename C>
void IncrementalRootFinder<Number, C>::findRoots() {
	while (this->next() != nullptr) {}
	this->setFinished();
}

template<typename Number, typename C>
RealAlgebraicNumberPtr<Number> IncrementalRootFinder<Number, C>::next() {
	while (this->nextRoot == this->roots.end()) {
		if (!this->processQueueItem()) {
			this->setFinished();
			return nullptr;
		}
	}
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
		if (this->polynomial.evaluate(0) == 0) this->addRoot(RealAlgebraicNumberNR<Number>::create(0));
		this->addQueue(Interval<Number>(interval.lower(), BoundType::STRICT, 0, BoundType::STRICT), strategy);
		this->addQueue(Interval<Number>(0, BoundType::STRICT, interval.upper(), BoundType::STRICT), strategy);
		return true;
	}

	unsigned int variations;
	variations = this->polynomial.signVariations(interval);

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
void buildIsolation(std::vector<double>& roots, const Interval<Number>& interval, RootFinder<Number>& finder) {
	assert(interval.lower() < interval.upper());
	std::sort(roots.begin(), roots.end());
	auto it = std::unique(roots.begin(), roots.end());
	roots.resize((size_t)std::distance(roots.begin(), it));

	std::vector<Number> res;
	res.reserve(roots.size() + 3);
	
	try {
		res.push_back(interval.lower());
		if (roots.size() == 1) {
			Number tmp = carl::rationalize<Number>(carl::floor(roots[0]));
			if (interval.contains(tmp)) res.push_back(tmp);
			tmp = carl::rationalize<Number>(carl::ceil(roots[0]));
			if (interval.contains(tmp)) res.push_back(tmp);
		} else {
			Number tmp = carl::rationalize<Number>(2 * roots[0] - roots[1]);
			if (interval.contains(tmp)) res.push_back(tmp);
			for (unsigned int i = 0; i < roots.size()-1; i++) {
				Number tmp = carl::rationalize<Number>(roots[i]);
				if (interval.contains(tmp) && finder.getPolynomial().evaluate(tmp) == 0) {
					res.push_back(tmp);
				}
				tmp = carl::rationalize<Number>(Interval<double>(roots[i], BoundType::STRICT, roots[i+1], BoundType::STRICT).sample());
				if (interval.contains(tmp)) res.push_back(tmp);
			}
			tmp = carl::rationalize<Number>(roots.back());
			if (interval.contains(tmp) && finder.getPolynomial().evaluate(tmp) == 0) {
				res.push_back(tmp);
			}
			tmp = carl::rationalize<Number>(2 * roots.back() - roots[roots.size()-2]);
			if (interval.contains(tmp)) res.push_back(tmp);
		}
		res.push_back(interval.upper());
	///@todo Add carl::floating_point_exception and use this here.
	} catch (cln::floating_point_exception) {
		finder.addQueue(interval, SplittingStrategy::BINARYSAMPLE);
		return;
	}

	assert(res[0] <= res[1]);
	if (res[0] < res[1]) {
		finder.addQueue(Interval<Number>(res[0], BoundType::STRICT, res[1], BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
	}
	for (unsigned int i = 1; i < res.size()-1; i++) {
		if (finder.getPolynomial().evaluate(res[i]) == 0) {
			finder.addRoot(RealAlgebraicNumberNR<Number>::create(res[i]));
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
		finder.addRoot(RealAlgebraicNumberNR<Number>::create(pivot));
	}
	finder.addQueue(Interval<Number>(interval.lower(), BoundType::STRICT, pivot, BoundType::STRICT), SplittingStrategy::GENERIC);
	finder.addQueue(Interval<Number>(pivot, BoundType::STRICT, interval.upper(), BoundType::STRICT), SplittingStrategy::GENERIC);
}

template<typename Number>
void BinarySampleStrategy<Number>::operator()(const Interval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.sample();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(RealAlgebraicNumberNR<Number>::create(pivot));
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
		finder.addRoot(RealAlgebraicNumberNR<Number>::create(pivot));
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
	
	// Create companion matrix
	unsigned int degree = p.degree();
	Eigen::MatrixXd m = Eigen::MatrixXd::Zero(degree, degree);
	m(0, degree-1) = toDouble(-p.coefficients()[0] / p.coefficients()[degree]);
	for (unsigned int i = 1; i < degree; i++) {
		m(i, i-1) = 1;
		m(i, degree-1) = toDouble(-p.coefficients()[i] / p.coefficients()[degree]);
	}
	
	// Obtain eigenvalues
	Eigen::VectorXcd eigenvalues = m.eigenvalues();
	
	// Save real parts to tmp
	std::vector<double> tmp((size_t)eigenvalues.size());
	for (unsigned int i = 0; i < eigenvalues.size(); i++) {
		tmp[i] = eigenvalues[i].real();
	}
	
	// Build isolation
	buildIsolation(tmp, interval, finder);
}

}

}
}

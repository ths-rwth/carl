/* 
 * File:   IncrementalRootFinder.tpp
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>

#include "AbstractRootFinder.h"
#include "RootFinder.h"

namespace carl {
namespace rootfinder {

template<typename Number, typename C>
IncrementalRootFinder<Number, C>::IncrementalRootFinder(
		const UnivariatePolynomial<Number>& polynomial,
		const ExactInterval<Number>& interval,
		SplittingStrategy splittingStrategy,
		bool tryTrivialSolver
		) :
		AbstractRootFinder<Number>(polynomial, interval, tryTrivialSolver),
		splittingStrategy(splittingStrategy),
		nextRoot(this->roots.end())
{
	this->addQueue(this->interval, this->splittingStrategy);
}

template<typename Number, typename C>
void IncrementalRootFinder<Number, C>::findRoots() {
	RealAlgebraicNumberPtr<Number> root = this->next();
	while (root != nullptr) {
		this->addRoot(root);
		root = this->next();
	}
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

	ExactInterval<Number> interval = std::get<0>(item);
	SplittingStrategy strategy = std::get<1>(item);

	LOGMSG_DEBUG("carl.core.rootfinder", "Processing interval " << interval);

	if (strategy == SplittingStrategy::EIGENVALUES) {
		splittingStrategies::EigenValueStrategy<Number>::instance()(interval, *this);
		return true;
	} else if (strategy == SplittingStrategy::ABERTH) {
		//AberthStrategy<Number>::instance()(interval, *this);
		//return true;
	}

	if (interval.contains(0)) {
		if (this->polynomial.evaluate(0) == 0) this->addRoot(RealAlgebraicNumberNR<Number>::create(0));
		this->addQueue(ExactInterval<Number>(interval.left(), 0, BoundType::STRICT), strategy);
		this->addQueue(ExactInterval<Number>(0, interval.right(), BoundType::STRICT), strategy);
		return true;
	}

	unsigned int variations;
	variations = this->polynomial.signVariations(interval);

	if (variations == 0) return true;
	if (variations == 1) {
		this->addRoot(interval);
		return true;
	}

	//std::cerr << "calling strategy " << strategy << std::endl;
	switch (strategy) {
		case SplittingStrategy::GENERIC: splittingStrategies::GenericStrategy<Number>::instance()(interval, *this);
			break;
		case SplittingStrategy::EIGENVALUES:	// Should not happen, safe fallback anyway
		case SplittingStrategy::ABERTH:		// Should not happen, safe fallback anyway
		case SplittingStrategy::BINARYSAMPLE: splittingStrategies::BinarySampleStrategy<Number>::instance()(interval, *this);
			break;
		case SplittingStrategy::BINARYNEWTON: splittingStrategies::BinaryNewtonStrategy<Number>::instance()(interval, *this);
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
void buildIsolation(std::vector<double>& roots, const ExactInterval<Number>& interval, RootFinder<Number>& finder) {
	std::sort(roots.begin(), roots.end());
	auto it = std::unique(roots.begin(), roots.end());
	roots.resize((size_t)std::distance(roots.begin(), it));

	std::vector<Number> res;
	res.reserve(roots.size() + 3);
	
	res.push_back(interval.left());
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
			tmp = carl::rationalize<Number>(ExactInterval<double>(roots[i], roots[i+1], BoundType::STRICT).sample());
			if (interval.contains(tmp)) res.push_back(tmp);
		}
		tmp = carl::rationalize<Number>(roots.back());
		if (interval.contains(tmp) && finder.getPolynomial().evaluate(tmp) == 0) {
			res.push_back(tmp);
		}
		tmp = carl::rationalize<Number>(2 * roots.back() - roots[roots.size()-2]);
		if (interval.contains(tmp)) res.push_back(tmp);
	}
	res.push_back(interval.right());
	
	finder.addQueue(ExactInterval<Number>(res[0], res[1], BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
	for (unsigned int i = 1; i < res.size()-1; i++) {
		if (finder.getPolynomial().evaluate(res[i]) == 0) {
			finder.addRoot(RealAlgebraicNumberNR<Number>::create(res[i]));
		}
		finder.addQueue(ExactInterval<Number>(res[i], res[i+1], BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
	}	
}

namespace splittingStrategies {

template<typename Number>
void GenericStrategy<Number>::operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.midpoint();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(RealAlgebraicNumberNR<Number>::create(pivot));
	}
	finder.addQueue(ExactInterval<Number>(interval.left(), pivot, BoundType::STRICT), SplittingStrategy::GENERIC);
	finder.addQueue(ExactInterval<Number>(pivot, interval.right(), BoundType::STRICT), SplittingStrategy::GENERIC);
}

template<typename Number>
void BinarySampleStrategy<Number>::operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.sample();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(RealAlgebraicNumberNR<Number>::create(pivot));
	}
	finder.addQueue(ExactInterval<Number>(interval.left(), pivot, BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
	finder.addQueue(ExactInterval<Number>(pivot, interval.right(), BoundType::STRICT), SplittingStrategy::BINARYSAMPLE);
}

template<typename Number>
void BinaryNewtonStrategy<Number>::operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.sample();
	Number diff = finder.getPolynomial().derivative().evaluate(pivot);
	if (diff != 0) {
		pivot -= finder.getPolynomial().evaluate(pivot) / diff;
	}
	if (!interval.contains(pivot)) pivot = interval.sample();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(RealAlgebraicNumberNR<Number>::create(pivot));
	}
	finder.addQueue(ExactInterval<Number>(interval.left(), pivot, BoundType::STRICT), SplittingStrategy::BINARYNEWTON);
	finder.addQueue(ExactInterval<Number>(pivot, interval.right(), BoundType::STRICT), SplittingStrategy::BINARYNEWTON);
}

template<typename Number>
void EigenValueStrategy<Number>::operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder) {
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
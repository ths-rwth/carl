/* 
 * File:   IncrementalRootFinder.tpp
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "AbstractRootFinder.h"
#include "RootFinder.h"

namespace carl {
namespace core {

template<typename Number, typename C>
IncrementalRootFinder<Number, C>::IncrementalRootFinder(
		UnivariatePolynomial<Number>& polynomial,
		SplittingStrategy splittingStrategy, 
		const ExactInterval<Number>& interval,
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
	RealAlgebraicNumber<Number>* root = this->next();
	while (root != nullptr) {
		this->addRoot(root);
		root = this->next();
	}
	this->setFinished();
}

template<typename Number, typename C>
RealAlgebraicNumber<Number>* IncrementalRootFinder<Number, C>::next() {
	while (this->nextRoot == this->roots.end()) {
		LOGMSG_DEBUG("carl.core.rootfinder", "No new root in cache, attempting to find a new one...");
		if (!this->processQueueItem()) {
			this->setFinished();
			return nullptr;
		}
	}
	return *(this->nextRoot++);
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
		//EigenValueStrategy<Number>::instance()(interval, *this);
		//return true;
	} else if (strategy == SplittingStrategy::ABERTH) {
		//AberthStrategy<Number>::instance()(interval, *this);
		//return true;
	}

	if (interval.contains(0)) {
		if (this->polynomial.evaluate(0) == 0) this->addRoot(new RealAlgebraicNumberNR<Number>(0));
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
		case SplittingStrategy::GENERIC: GenericStrategy<Number>::instance()(interval, *this);
			break;
		case SplittingStrategy::EIGENVALUES:	// Should not happen, safe fallback anyway
		case SplittingStrategy::ABERTH:		// Should not happen, safe fallback anyway
		case SplittingStrategy::BINARYSAMPLE: BinarySampleStrategy<Number>::instance()(interval, *this);
			break;
		case SplittingStrategy::BINARYNEWTON: BinaryNewtonStrategy<Number>::instance()(interval, *this);
			break;
		case SplittingStrategy::GRID: //GridStrategy<Number>::instance()(interval, *this);
			break;
	}
	return true;
}

template<typename Number>
void GenericStrategy<Number>::operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.midpoint();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(new RealAlgebraicNumberNR<Number>(pivot));
	}
	finder.addQueue(ExactInterval<Number>(interval.left(), pivot, BoundType::STRICT), SplittingStrategy::GENERIC);
	finder.addQueue(ExactInterval<Number>(pivot, interval.right(), BoundType::STRICT), SplittingStrategy::GENERIC);
}

template<typename Number>
void BinarySampleStrategy<Number>::operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder) {
	Number pivot = interval.sample();
	if (finder.getPolynomial().evaluate(pivot) == 0) {
		finder.addRoot(new RealAlgebraicNumberNR<Number>(pivot));
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
		finder.addRoot(new RealAlgebraicNumberNR<Number>(pivot));
	}
	finder.addQueue(ExactInterval<Number>(interval.left(), pivot, BoundType::STRICT), SplittingStrategy::BINARYNEWTON);
	finder.addQueue(ExactInterval<Number>(pivot, interval.right(), BoundType::STRICT), SplittingStrategy::BINARYNEWTON);
}

}
}
/**
 * @file IncrementalRootFinder.h
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <queue>
#include "../logging.h"

namespace carl {
namespace rootfinder {

/*!
 * Orders QueueItems by the size of their intervals.
 *
 * Heuristic for order of queue items to be considered, comparing only the size of the intervals.
 */
struct IntervalSizeComparator {
	template <typename QueueItem>
	bool operator()(const QueueItem& a, const QueueItem& b) const {
		return std::get<0>(a).diameter() > std::get<0>(b).diameter();
	}
};

template<typename Number, typename Comparator = IntervalSizeComparator>
class IncrementalRootFinder;

}
}

#include "AbstractRootFinder.h"

namespace carl {
namespace rootfinder {

/*!
 * Enum of all strategies for splitting some interval.
 */
enum class SplittingStrategy : unsigned int {
	/// Uses GenericStrategy
	GENERIC,
	/// Uses BinarySampleStrategy
	BINARYSAMPLE,
	/// Uses BinaryNewtonStrategy
	BINARYNEWTON,
	/// Uses GridStrategy
	GRID,
	/// Uses EigenValueStrategy for first step, BinarySampleStrategy afterwards
	EIGENVALUES,
	/// Uses AberthStrategy for first step, BinarySampleStrategy afterwards
	ABERTH,
	/// Defaults to EIGENVALUES
	DEFAULT = EIGENVALUES
};

inline std::ostream& operator<<(std::ostream& os, const SplittingStrategy& s) {
	switch (s) {
		case SplittingStrategy::GENERIC: return os << "Generic";
		case SplittingStrategy::BINARYSAMPLE: return os << "BinarySample";
		case SplittingStrategy::BINARYNEWTON: return os << "BinaryNewton";
		case SplittingStrategy::GRID: return os << "Grid";
		case SplittingStrategy::EIGENVALUES: return os << "Eigenvalues";
		case SplittingStrategy::ABERTH: return os << "Aberth";
	}
}

/**
 * Interface of a RootFinder such that the Strategies can access the IncrementalRootFinder.
 * @return
 */
template<typename Number>
class RootFinder {
public:
	virtual const UnivariatePolynomial<Number>& getPolynomial() const = 0;
	virtual void addQueue(const ExactInterval<Number>& interval, SplittingStrategy strategy) = 0;
	virtual void addRoot(RealAlgebraicNumberPtr<Number> root, bool reducePolynomial = true) = 0;
	virtual void addRoot(const ExactInterval<Number>& interval) = 0;
};

namespace splittingStrategies {

template<typename Strategy, typename Number>
struct AbstractStrategy: Singleton<Strategy> {
	virtual void operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder) = 0;
};

template<typename Number>
struct GenericStrategy : public AbstractStrategy<GenericStrategy<Number>, Number> {
	virtual void operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder);
};
template<typename Number>
struct BinarySampleStrategy : public AbstractStrategy<BinarySampleStrategy<Number>, Number> {
	virtual void operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder);
};
template<typename Number>
struct BinaryNewtonStrategy : public AbstractStrategy<BinaryNewtonStrategy<Number>, Number> {
	virtual void operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder);
};
template<typename Number>
struct GridStrategy : public AbstractStrategy<GridStrategy<Number>, Number> {
	virtual void operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder);
};
template<typename Number>
struct EigenValueStrategy: public AbstractStrategy<EigenValueStrategy<Number>, Number> {
	virtual void operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder);
};
template<typename Number>
struct AberthStrategy : public AbstractStrategy<AberthStrategy<Number>, Number> {
	std::vector<double> teruiSasaki(const UnivariatePolynomial<Number>& p, const unsigned int rootCount);
	double step(std::vector<double>& roots, const UnivariatePolynomial<Number>& p, const UnivariatePolynomial<Number>& pd);
	Number step(std::vector<Number>& roots, const UnivariatePolynomial<Number>& p, const UnivariatePolynomial<Number>& pd);
	bool Aberth(const UnivariatePolynomial<Number>& polynomial, std::vector<double>& roots, const double epsilon = 0.01);

	virtual void operator()(const ExactInterval<Number>& interval, RootFinder<Number>& finder);
};

}

template<typename Number, typename Comparator>
class IncrementalRootFinder : public AbstractRootFinder<Number>, RootFinder<Number> {

public:
	typedef std::tuple<ExactInterval<Number>, SplittingStrategy> QueueItem;

private:
	std::list<UnivariatePolynomial<Number>> sturmSequence;

	/*!
	 * The current default strategy.
	 */
	SplittingStrategy splittingStrategy;
	/*!
	 * Interval queue containing all items that must still be processed.
	 */
	std::priority_queue<QueueItem, std::vector<QueueItem>, IntervalSizeComparator> queue;
	/*!
	 * Iterator pointing to the next root within the root list that should be returned.
	 */
	typename std::list<RealAlgebraicNumberPtr<Number>>::iterator nextRoot;

public:

	IncrementalRootFinder(
			const UnivariatePolynomial<Number>& polynomial,
			const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedExactInterval(),
			SplittingStrategy splittingStrategy = SplittingStrategy::DEFAULT,
			bool tryTrivialSolver = true
			);

	virtual ~IncrementalRootFinder() {
	}

	const UnivariatePolynomial<Number>& getPolynomial() const {
		return this->polynomial;
	}

	/**
	 * Computes some new isolating root of the polynomial.
	 * If all roots have already been found, a nullptr will be returned.
	 * @return A new root, nullptr if all roots have been found.
	 */
	RealAlgebraicNumberPtr<Number> next();

	/**
	 * Adds a new item to the internal interval queue.
	 * Convenience routine for splitting heuristics.
	 * @param interval Interval to add.
	 * @param strategy Strategy to add.
	 */
	void addQueue(const ExactInterval<Number>& interval, SplittingStrategy strategy) {
	   this->queue.push(std::make_tuple(interval, strategy));
	}

protected:

	virtual void addRoot(RealAlgebraicNumberPtr<Number> root, bool reducePolynomial = true) {
		if (this->nextRoot == this->roots.end()) this->nextRoot--;
		AbstractRootFinder<Number>::addRoot(root, reducePolynomial);
	}
	virtual void addRoot(const ExactInterval<Number>& interval) {
		if (this->nextRoot == this->roots.end()) this->nextRoot--;
		AbstractRootFinder<Number>::addRoot(interval);
	}

	/**
	 * Overrides method from AbstractRootFinder.
     */
	virtual void findRoots();

	/**
	 * Tries to work on the next item in queue to produce a new root.
	 * If the queue is empty, it will return false.
	 * Otherwise, the first item is taken and processed, the return value will be true.
	 * @return False, if the queue is empty, i.e. we have found all roots.
	 */
	bool processQueueItem();
};

}
}

#include "IncrementalRootFinder.tpp"

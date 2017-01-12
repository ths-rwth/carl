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

/**
 * Enum of all strategies for splitting some interval.
 */
enum class SplittingStrategy {
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

/**
 * Streaming operator for a splitting strategy.
 * @param os Output stream.
 * @param s Splitting strategy.
 * @return os.
 */
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
 */
template<typename Number>
class RootFinder {
public:
	virtual ~RootFinder() {}
	/**
	 * Retrieves the polynomial that is processed.
	 * @return Polynomial.
	 */
	virtual const UnivariatePolynomial<Number>& getPolynomial() const = 0;
	/**
	 * Adds a new interval together with a splitting strategy to the queue of intervals that must be processed.
	 * @param interval Interval.
	 * @param strategy Strategy.
	 */
	virtual void addQueue(const Interval<Number>& interval, SplittingStrategy strategy) = 0;
	/**
	 * Add a root to the list of found roots.
	 * If the root was calculated exactly, the polynomial can be divided by \f$(x - r)\f$ where \f$x\f$ is the main variable of the polynomial and \f$r\f$ is the root.
	 * @param root Real root.
	 * @param reducePolynomial Flag if polynomial should be reduced.
	 */
	virtual void addRoot(const RealAlgebraicNumber<Number>& root, bool reducePolynomial = true) = 0;
	/**
	 * Add a root to the list of found roots.
	 * @param interval Interval that contains the real root.
	 */
	virtual void addRoot(const Interval<Number>& interval) = 0;
};

namespace splittingStrategies {

/**
 * Abstract base class for all splitting strategies.
 */
template<typename Strategy, typename Number>
struct AbstractStrategy: Singleton<Strategy> {
	virtual void operator()(const Interval<Number>& interval, RootFinder<Number>& finder) = 0;
};

/**
 * Implements a generic splitting strategy.
 */
template<typename Number>
struct GenericStrategy : public AbstractStrategy<GenericStrategy<Number>, Number> {
	/**
	 * Given an interval \f$(a,b)\f$, it uses the center \f$p = (a+b)/2\f$ as pivot.
	 * The resulting intervals are \f$(a,p),[p],(p,b)\f$.
	 * @param interval Interval.
	 * @param finder Finder object.
	 */
	virtual void operator()(const Interval<Number>& interval, RootFinder<Number>& finder);
};

/**
 * Implements a binary sample splitting strategy.
 */
template<typename Number>
struct BinarySampleStrategy : public AbstractStrategy<BinarySampleStrategy<Number>, Number> {
	/**
	 * Given an interval \f$(a,b)\f$, it uses some sample point \f$p \in (a,b)\f$ as pivot.
	 * We try to select an easy (i.e. integer or small fraction) point as sample point.
	 * The resulting intervals are \f$(a,p),[p],(p,b)\f$.
	 * @param interval Interval.
	 * @param finder Finder object.
	 */
	virtual void operator()(const Interval<Number>& interval, RootFinder<Number>& finder);
};

/**
 * Implements a binary splitting strategy using newton.
 */
template<typename Number>
struct BinaryNewtonStrategy : public AbstractStrategy<BinaryNewtonStrategy<Number>, Number> {
	/**
	 * Given an interval \f$(a,b)\f$, it uses some point \f$p \in (a,b)\f$ as pivot.
	 * The pivot is determined using Newtons method starting from the center of the interval.
	 * The resulting intervals are \f$(a,p),[p],(p,b)\f$.
	 * @param interval Interval.
	 * @param finder Finder object.
	 */
	virtual void operator()(const Interval<Number>& interval, RootFinder<Number>& finder);
};

/**
 * Implements a n-ary splitting strategy using a grid.
 */
template<typename Number>
struct GridStrategy : public AbstractStrategy<GridStrategy<Number>, Number> {
	virtual void operator()(const Interval<Number>& interval, RootFinder<Number>& finder);
};

/**
 * Implements a n-ary splitting strategy based on the eigenvalues of the companion matrix.
 */
template<typename Number>
struct EigenValueStrategy: public AbstractStrategy<EigenValueStrategy<Number>, Number> {
	/**
	 * Given an interval \f$(a,b)\f$, it uses several pivot points \f$p_i \in (a,b)\f$ as pivots.
	 * In theory, the eigenvalues of the companion matrix of a polynomial are equal to the (complex) roots of a univariate polynomial.
	 * However, due to numerical instability, we use them only for splitting.
	 * Given the eigenvalues \f$e_1, ..., e_k\f$ and the interval \f$(a,b)\f$, the resulting intervals are \f$(a, e_1), [e_1], ..., [e_k], (e_k, b)\f$.
	 * @param interval Interval.
	 * @param finder Finder object.
	 */
	virtual void operator()(const Interval<Number>& interval, RootFinder<Number>& finder);
};

/**
 * Implements a n-ary splitting strategy based on Aberths method.
 */
template<typename Number>
struct AberthStrategy : public AbstractStrategy<AberthStrategy<Number>, Number> {
	std::vector<double> teruiSasaki(const UnivariatePolynomial<Number>& p, uint rootCount);
	double step(std::vector<double>& roots, const UnivariatePolynomial<Number>& p, const UnivariatePolynomial<Number>& pd);
	Number step(std::vector<Number>& roots, const UnivariatePolynomial<Number>& p, const UnivariatePolynomial<Number>& pd);
	bool Aberth(const UnivariatePolynomial<Number>& polynomial, std::vector<double>& roots, const double epsilon = 0.01);

	virtual void operator()(const Interval<Number>& interval, RootFinder<Number>& finder);
};

}

/**
 * This class implements an AbstractRootFinder that has an interface to calculate the real roots incrementally.
 *
 * Using the method next(), the next real root can be obtained.
 * The root finder uses a bisection approach internally and implements multiple strategies how the bisection is performed.
 */
template<typename Number, typename Comparator>
class IncrementalRootFinder : public AbstractRootFinder<Number>, RootFinder<Number> {

public:
	/**
	 * Type of a queue item, consisting of an interval to be searched and the strategy to be used.
	 */
	typedef std::tuple<Interval<Number>, SplittingStrategy> QueueItem;

private:
	/**
	 * Caches the sturm sequence of the polynomial
	 */
	std::list<UnivariatePolynomial<Number>> sturmSequence;

	/**
	 * The current default strategy.
	 */
	SplittingStrategy splittingStrategy;
	/**
	 * Interval queue containing all items that must still be processed.
	 */
	std::priority_queue<QueueItem, std::vector<QueueItem>, IntervalSizeComparator> queue;
	/**
	 * Iterator pointing to the next root within the root list that should be returned.
	 */
	typename std::list<RealAlgebraicNumber<Number>>::iterator nextRoot;

public:

	/**
	 * Constructor for a root finder that searches for the real roots of a polynomial in an interval using a strategy.
	 * @param polynomial Polynomial.
	 * @param interval Interval, unbounded if none is given.
	 * @param splittingStrategy Strategy.
	 * @param tryTrivialSolver Flag is the trivial solver shall be used.
	 */
	IncrementalRootFinder(
			const UnivariatePolynomial<Number>& polynomial,
			const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
			SplittingStrategy splittingStrategy = SplittingStrategy::DEFAULT,
			bool tryTrivialSolver = true
			);

	/**
	 * Desctructor.
	 */
	virtual ~IncrementalRootFinder() = default;

	const UnivariatePolynomial<Number>& getPolynomial() const {
		return this->polynomial;
	}

	/**
	 * Computes some new isolating root of the polynomial.
	 * If all roots have already been found, returns false.
	 * @return True if a new root is available, false otherwise.
	 */
	bool hasNext();

	/**
	 * Returns the next root as computed by hasNext().
	 * Asserts that there actually is a new root.
	 * @return A new root.
	 */
	RealAlgebraicNumber<Number> next();

	/**
	 * Adds a new item to the internal interval queue.
	 * Convenience routine for splitting heuristics.
	 * @param interval Interval to add.
	 * @param strategy Strategy to add.
	 */
	void addQueue(const Interval<Number>& interval, SplittingStrategy strategy) {
	   this->queue.push(std::make_tuple(interval, strategy));
	}

protected:

	virtual void addRoot(const RealAlgebraicNumber<Number>& root, bool reducePolynomial = true) {
		if (this->nextRoot == this->roots.end()) this->nextRoot--;
		AbstractRootFinder<Number>::addRoot(root, reducePolynomial);
	}
	virtual void addRoot(const Interval<Number>& interval) {
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

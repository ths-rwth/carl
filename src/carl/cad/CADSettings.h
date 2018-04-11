/**
 * @file CADSettings.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <algorithm>

#include "../core/logging.h"
#include "../core/carlLogging.h"
#include "../core/rootfinder/RootFinder.h"

namespace carl {
namespace cad {

/**
 * Defines possible orderings for sample points.
 */
enum class SampleOrdering : unsigned {
	IntRatRoot,
	IntRatSize,
	RatRoot,
	Interval,
	Root,
	NonRoot,
	Value,
	Default = SampleOrdering::IntRatRoot
};

/// Defines how integers are handled.
enum class IntegerHandling {
    /// Generate a full real solution and split on non-integral assignments in this solution.
    SPLIT_ASSIGNMENT,
	/// Generate a full real solution and split on integral assignments that lead to a non-integral assignment.
	SPLIT_PATH,
    /// Backtrack within the sampling phase.
    BACKTRACK,
	/// Do not care about integers
	NONE,
	SPLIT_SOLUTION,
	SPLIT_LAZY,
	SPLIT_EARLY,
	GUESS_AND_SPLIT
};
inline std::ostream& operator<<(std::ostream& os, const IntegerHandling& ih) {
	switch (ih) {
		case IntegerHandling::SPLIT_ASSIGNMENT: return os << "Split assignment";
		case IntegerHandling::SPLIT_PATH: return os << "Split path";
		case IntegerHandling::BACKTRACK: return os << "Backtrack";
		case IntegerHandling::NONE: return os << "None";
		case IntegerHandling::SPLIT_SOLUTION: return os << "Split on solution";
		case IntegerHandling::SPLIT_LAZY: return os << "Split lazy";
		case IntegerHandling::SPLIT_EARLY: return os << "Split early";
		case IntegerHandling::GUESS_AND_SPLIT: return os << "Guess";
	}
	return os;
}
inline std::istream& operator>>(std::istream& in, IntegerHandling&) {
    return in;
}

/**
 * Streaming operator for SampleOrdering.
 * @param os Output stream.
 * @param so SampleOrdering.
 * @return Resulting output stream.
 */
inline std::ostream& operator<<(std::ostream& os, SampleOrdering so) {
	switch (so) {
		case SampleOrdering::IntRatRoot: return os << "Integer-Rational-Root";
		case SampleOrdering::RatRoot: return os << "Rational-Root";
		default: return os << "Unknown ordering";
	}
}

template<typename Coeff>
using MultiPolynomial = carl::MultivariatePolynomial<Coeff>;
template<typename Coeff>
using UniPolynomial = carl::UnivariatePolynomial<MultiPolynomial<Coeff>>;

/** Predefined settings for the CAD procedure.
 * Implementation of the types is located in CAD.h. Each setting is defined as a power of two in order to use several flags at a time.
 * Note that the order of the flags plays a role: If, e.g., A and B are set, then the last one (B) is used.
 */
enum CADSettingsType {
   /// generic setting: low-degree first polynomial order, nothing more
   GENERIC = 1,
   /// setting avoiding computations with interval-represented samples (low- and odd-degree order + preference of numeric samples)
   RATIONALSAMPLE = 2,
   /// setting preferring computations with interval-represented samples (low- and even-degree order + preference of root samples)
   IRRATIONALSAMPLE = 4,
   /// setting for an equation-only input to CAD::check
   EQUATIONSONLY = 8,
   /// setting for an inequality-only input to CAD::check
   INEQUALITIESONLY = 16,
   /// equation- and inequality parts are treated separately, the equation part is solved first and checked against the other constraints
   EQUATIONDETECT = 32,
   /// like EQUATIONDETECT, but the equation part is assumed to be zero-dimensional
   ZERODIM = 64,
   /// alternative polynomial ordering
   ALTERNATIVEORDER = 128,
   /// everything optimized for the use with bounds
   BOUNDED = 256,
   /// bounds-related optimizations explicitely deactivated
   NOTBOUNDED = 512,
   
   EXPLOREINTEGER = 1024,
   
   DEFAULT = BOUNDED
};

static const std::string DEFAULT_CAD_OUTPUTFILE = "cad_constraints.smt2";

/**
 * Central storage component for CAD strategies and options.
 */
struct CADSettings {
public:
	/// Ordering used for samples.
	SampleOrdering sampleOrdering;
	/// flag indicating that the elimination uses real root counting to simplify the bottom-most level
	bool simplifyByRootcounting;
	/// flag indicating that the elimination uses factorization of polynomials in every level
	bool simplifyByFactorization;
	/// elimination following [McCallum - "On projection in CAD-based quantifier elimination with equational constraint"] and [McCallum - "On Propagation of Equational Constraints in CAD-Based Quantifier Elimination"], and no intermediate points are considered for lifting
	bool equationsOnly;
	/// only intermediate points are considered for lifting [McCallum - "Solving Polynomial Strict Inequalities Using Cylindrical Algebraic Decomposition"]
	bool inequalitiesOnly;
	/// during one elimination operation, all polynomials which are just copied to the next level are removed from the previous
	bool removeConstants;
	/// treat equations separately by tuning the cad object to equations
	bool autoSeparateEquations;
	/// compute a conflict graph after determining unsatisfiability of a set of constraints via CAD::check
	bool computeConflictGraph;
	/// given bounds to the check method, these bounds are used to solve the constraints just by interval arithmetic
	bool preSolveByBounds;
	/// given bounds to the check method, these bounds are used wherever possible to reduce the sample sets during the lifting and to reduce the elimination polynomials if simplifyEliminationByBounds is set
	bool earlyLiftingPruningByBounds;
	/// given bounds to the check method, these bounds are used to cancel out elimination polynomials
	bool simplifyEliminationByBounds;
	/// given bounds to the check method, the bounds are widened after determining unsatisfiability by check, or shrunk after determining satisfiability by check
	bool improveBounds;
	bool exploreInteger;
	bool splitInteger;
	bool ignoreRoots;
	IntegerHandling integerHandling;
	/// the order in which the polynomials in each elimination level are sorted
	PolynomialComparisonOrder order;
	/// standard strategy to be used for real root isolation
	rootfinder::SplittingStrategy splittingStrategy;

	/**
	 * Generate a CADSettings instance of the respective preset type.
	 * @param setting preset
	 * @param isolationStrategy strategy for isolating real roots
	 * @param cadSettings setting to be enhanced (default: CADSettings())
	 * @return a CADSettings instance of the respective preset type
	 */
	static CADSettings getSettings(
			unsigned setting = DEFAULT,
			rootfinder::SplittingStrategy isolationStrategy = rootfinder::SplittingStrategy::DEFAULT,
			CADSettings cadSettings = CADSettings() )
	{
		cadSettings.splittingStrategy = isolationStrategy;
		if (setting & RATIONALSAMPLE) {
			cadSettings.autoSeparateEquations = false;
			cadSettings.sampleOrdering = SampleOrdering::RatRoot;
			cadSettings.order = PolynomialComparisonOrder::CauchyBound;
		}
		if (setting & IRRATIONALSAMPLE) {
			cadSettings.autoSeparateEquations = false;
			cadSettings.sampleOrdering = SampleOrdering::Interval;
			cadSettings.order = PolynomialComparisonOrder::CauchyBound;
		}
		if (setting & EQUATIONDETECT) {
			cadSettings.autoSeparateEquations = true;
			cadSettings.order = PolynomialComparisonOrder::CauchyBound;
		}
		if (setting & BOUNDED) {
			cadSettings.autoSeparateEquations       = true;
			cadSettings.computeConflictGraph        = false;
			cadSettings.earlyLiftingPruningByBounds = true;
			cadSettings.improveBounds               = true;
			cadSettings.preSolveByBounds            = false;
			cadSettings.removeConstants             = true;
			cadSettings.simplifyByFactorization     = true;
			cadSettings.simplifyByRootcounting      = false;
			cadSettings.simplifyEliminationByBounds = true;
			cadSettings.order = PolynomialComparisonOrder::CauchyBound;
		}
		if (setting & NOTBOUNDED) {
			cadSettings.autoSeparateEquations       = true;
			cadSettings.computeConflictGraph        = false;
			cadSettings.earlyLiftingPruningByBounds = false;
			cadSettings.improveBounds               = false;
			cadSettings.preSolveByBounds            = false;
			cadSettings.removeConstants             = true;
			cadSettings.simplifyByFactorization     = true;
			cadSettings.simplifyByRootcounting      = false;
			cadSettings.simplifyEliminationByBounds = false;
			cadSettings.order = PolynomialComparisonOrder::CauchyBound;
		}
		if (setting & EQUATIONSONLY) {
			cadSettings.autoSeparateEquations = false;
			cadSettings.sampleOrdering = SampleOrdering::Root;
			cadSettings.equationsOnly         = true;
			cadSettings.inequalitiesOnly      = false;
			cadSettings.order = PolynomialComparisonOrder::CauchyBound;
		}
		if (setting & INEQUALITIESONLY) {
			cadSettings.sampleOrdering		  = SampleOrdering::NonRoot;
			cadSettings.equationsOnly         = false;
			cadSettings.inequalitiesOnly      = true;
			cadSettings.order = PolynomialComparisonOrder::CauchyBound;
		}
		if (setting & ALTERNATIVEORDER) {
			cadSettings.order = PolynomialComparisonOrder::LowDegree;
		}
		if (setting & EXPLOREINTEGER) {
			cadSettings.exploreInteger = true;
			cadSettings.splitInteger = true;
		}

		return cadSettings;
	}

	friend std::ostream& operator<<(std::ostream& os, const CADSettings& settings) {
		std::list<std::string> settingStrs;
		if (settings.simplifyByRootcounting)
			settingStrs.push_back( "Simplify the base elimination level by real root counting." );
		if (settings.simplifyByFactorization)
			settingStrs.push_back( "Simplify the elimination by factorization of polynomials in every level (using GiNaC::factor)." );
		if (settings.equationsOnly)
			settingStrs.push_back( "Simplify elimination for equation-only use (currently disabled) + do not use intermediate points for lifting." );
		if (settings.inequalitiesOnly)
			settingStrs.push_back( "Use only intermediate points for lifting." );
		if (settings.removeConstants)
			settingStrs.push_back( "During elimination, all polynomials which are just copied to the next level are removed from the previous." );
		if (settings.autoSeparateEquations)
			settingStrs.push_back( "Treat equations separately by tuning the cad object to equations." );
		if (settings.computeConflictGraph)
			settingStrs.push_back( "Compute a conflict graph after determining unsatisfiability of a set of constraints via CAD::check." );
		if (settings.preSolveByBounds)
			settingStrs.push_back( "Given bounds to the check method, these bounds are used to solve the constraints just by interval arithmetic." );
		if (settings.earlyLiftingPruningByBounds)
			settingStrs.push_back( "Given bounds to the check method, these bounds are used to reduce the sample sets during the lifting and to reduce the elimination polynomials if simplifyEliminationByBounds is set." );
		if (settings.simplifyEliminationByBounds)
			settingStrs.push_back( "Given bounds to the check method, these bounds are used to cancel out elimination polynomials." );
		if (settings.improveBounds)
			settingStrs.push_back( "Given bounds to the check method, the bounds are widened after determining unsatisfiability by check, or shrunk after determining satisfiability by check." );
		std::string orderStr = "Polynomial order: ";

		if (settings.order == PolynomialComparisonOrder::CauchyBound)
			settingStrs.push_back( orderStr + "Take polynomial with small cauchy bound first." );
		if (settings.order == PolynomialComparisonOrder::LowDegree)
			settingStrs.push_back( orderStr + "Take polynomial with small degree first." );
		if (settings.order == PolynomialComparisonOrder::Memory)
			settingStrs.push_back( orderStr + "Take polynomial with small memory address first." );

		os << "+------------------------------------ CAD Setting -----------------------------------";
		if (settingStrs.empty()) {
			os << std::endl << "| Default ";
		} else {
			for (auto i: settingStrs) {
				os << std::endl << "↳ " << i;
			}
		}
		return os << std::endl << "+------------------------------------------------------------------------------------";
	}

private:

	/**
	 * Constructor initiating a standard settings object.
	 */
	CADSettings():
		sampleOrdering( SampleOrdering::Default ),
		simplifyByRootcounting( false ),
		simplifyByFactorization( true ),
		equationsOnly( false ),
		inequalitiesOnly( false ),
		removeConstants( true ),
		autoSeparateEquations( false ),
		computeConflictGraph( true ),
		preSolveByBounds( false ),
		earlyLiftingPruningByBounds( true ),
		simplifyEliminationByBounds( true ),
		improveBounds( true ),
		exploreInteger(false),
		splitInteger(true),
		ignoreRoots(false),
		integerHandling(IntegerHandling::SPLIT_ASSIGNMENT),
		order(PolynomialComparisonOrder::Default),
		splittingStrategy(rootfinder::SplittingStrategy::DEFAULT)
	{}

public:

	/**
	 * Copy constructor.
	 */
	CADSettings( const CADSettings& s ):
		sampleOrdering( s.sampleOrdering ),
		simplifyByRootcounting( s.simplifyByRootcounting ),
		simplifyByFactorization( s.simplifyByFactorization ),
		equationsOnly( s.equationsOnly ),
		inequalitiesOnly( s.inequalitiesOnly ),
		removeConstants( s.removeConstants ),
		autoSeparateEquations( s.autoSeparateEquations ),
		computeConflictGraph( s.computeConflictGraph ),
		preSolveByBounds( s.preSolveByBounds ),
		earlyLiftingPruningByBounds( s.earlyLiftingPruningByBounds ),
		simplifyEliminationByBounds( s.simplifyEliminationByBounds ),
		improveBounds( s.improveBounds ),
		exploreInteger(s.exploreInteger),
		splitInteger(s.splitInteger),
		ignoreRoots(s.ignoreRoots),
		integerHandling(s.integerHandling),
		order(PolynomialComparisonOrder::Default),
		splittingStrategy(rootfinder::SplittingStrategy::DEFAULT)
	{}
};

}
}

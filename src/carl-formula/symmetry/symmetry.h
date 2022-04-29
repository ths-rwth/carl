#pragma once

#include <carl/core/Variable.h>

#include <vector>

namespace carl {
namespace formula {

/**
 * A symmetry \f$\sigma\f$ represents a bijection on a set of variables.
 * For every entry in the vector we have \f$\sigma(e.first) = e.second\f$.
 */
using Symmetry = std::vector<std::pair<Variable,Variable>>;

/**
 * Represents a list of symmetries.
 */
using Symmetries = std::vector<Symmetry>;

}
}

#ifdef USE_BLISS

#include "SymmetryFinder.h"
#include "SymmetryBreaker.h"

namespace carl {
namespace formula {

/**
 * Find syntactic symmetries in the given formula.
 * Builds a graph that syntactically represents the formula and find automorphisms on its vertices.
 */
template<typename Poly>
Symmetries findSymmetries(const Formula<Poly>& f) {
	symmetry::GraphBuilder<Poly> g(f);
	return g.symmetries();
}

/// Construct symmetry breaking formulae for the given symmetries.
template<typename Poly>
Formula<Poly> breakSymmetries(const Symmetries& symmetries, bool onlyFirst = true) {
	Formulas<Poly> res;
	for (const auto& s: symmetries) {
		res.emplace_back(symmetry::lexLeaderConstraint<Poly>(s));
		if (onlyFirst) return res.back();
	}
	return Formula<Poly>(FormulaType::AND, std::move(res));
}

/// Construct symmetry breaking formulae for symmtries from the given formula.
template<typename Poly>
Formula<Poly> breakSymmetries(const Formula<Poly>& f, bool onlyFirst = true) {
	return breakSymmetries<Poly>(findSymmetries(f), onlyFirst);
}

}
}

#else

namespace carl {
namespace formula {

template<typename Poly>
Symmetries findSymmetries(const Formula<Poly>& f) {
	return Symmetries();
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Symmetries& symmetries, bool onlyFirst = true) {
	return Formula<Poly>(FormulaType::TRUE);
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Formula<Poly>& f, bool onlyFirst = true) {
	return Formula<Poly>(FormulaType::TRUE);
}

}
}


#endif

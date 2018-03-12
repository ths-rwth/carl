#pragma once

namespace carl {
namespace formula {

using Symmetry = std::vector<std::pair<Variable,Variable>>;
using Symmetries = std::vector<Symmetry>;

}
}

#include "SymmetryFinder.h"
#include "SymmetryBreaker.h"

namespace carl {
namespace formula {

template<typename Poly>
Symmetries findSymmetries(const Formula<Poly>& f) {
	symmetry::GraphBuilder<Poly> g(f);
	return g.symmetries();
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Symmetries& symmetries) {
	Formulas<Poly> res;
	for (const auto& s: symmetries) {
		res.emplace_back(symmetry::lexLeaderConstraint<Poly>(s));
	}
	return Formula<Poly>(FormulaType::AND, std::move(res));
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Formula<Poly>& f) {
	return breakSymmetries<Poly>(findSymmetries(f));
}

}
}

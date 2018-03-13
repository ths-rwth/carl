#pragma once


namespace carl {
namespace formula {

using Symmetry = std::vector<std::pair<Variable,Variable>>;
using Symmetries = std::vector<Symmetry>;

}
}

#ifdef USE_BLISS

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
Formula<Poly> breakSymmetries(const Symmetries& symmetries, bool onlyFirst = true) {
	Formulas<Poly> res;
	for (const auto& s: symmetries) {
		res.emplace_back(symmetry::lexLeaderConstraint<Poly>(s));
		if (onlyFirst) return res.back();
	}
	return Formula<Poly>(FormulaType::AND, std::move(res));
}

template<typename Poly>
Formula<Poly> breakSymmetries(const Formula<Poly>& f, bool onlyFirst = true) {
	return breakSymmetries<Poly>(findSymmetries(f));
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

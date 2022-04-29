#pragma once

#include "../Model.h"

namespace carl {
namespace model {

template<typename Rational, typename Poly>
ran::RANMap<Rational> collectRANIR(const std::set<Variable>& vars, const Model<Rational,Poly>& model) {
	ran::RANMap<Rational> map;
	for (auto var: vars) {
		if (model.find(var) == model.end()) continue;
		const ModelValue<Rational,Poly>& mv = model.evaluated(var);
		if (mv.isRAN()) {
			map.emplace(var, mv.asRAN());
		}
	}
	return map;
}

}
}

#pragma once

#include "../Model.h"

namespace carl {
namespace model {

template<typename Rational, typename Poly>
Assignment<typename Poly::RootType> collectRANIR(const std::set<Variable>& vars, const Model<Rational,Poly>& model) {
	Assignment<typename Poly::RootType> map;
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

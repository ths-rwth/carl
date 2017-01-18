#pragma once

#include "../Formula.h"

#include "../../core/logging.h"
#include "../../core/Relation.h"

#include <iostream>
#include <map>
#include <tuple>
#include <vector>

namespace carl {

using OPBPolynomial = std::vector<std::pair<int,carl::Variable>>;
using OPBConstraint = std::tuple<OPBPolynomial, Relation, int>;

struct OPBFile {
	OPBPolynomial objective;
	std::vector<OPBConstraint> constraints;
	
	OPBFile() {}
	OPBFile(const OPBPolynomial& obj): objective(obj) {}
	OPBFile(const OPBPolynomial& obj, const std::vector<OPBConstraint>& cons): objective(obj), constraints(cons) {}
};

OPBFile parseOPBFile(std::ifstream& in);

template<typename Pol>
class OPBImporter {
private:
	std::ifstream mIn;

public:
	OPBImporter(const std::string& filename):
		mIn(filename)
	{}
	
	Formula<Pol> parse() {
		auto file = parseOPBFile(mIn);
		Formulas<Pol> constraints;
		for (const auto& cons: file.constraints) {
			PBConstraint pbc(std::move(std::get<0>(cons)), std::get<1>(cons), std::get<2>(cons));
			constraints.emplace_back(std::move(pbc));
		}
		return Formula<Pol>(FormulaType::AND, std::move(constraints));
	}
};

}

#pragma once

#include "../../core/logging.h"
#include "../../core/Relation.h"
#include "../Formula.h"

#include <boost/optional.hpp>

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
	
	OPBFile() = default;
	explicit OPBFile(OPBPolynomial obj): objective(std::move(obj)) {}
	OPBFile(OPBPolynomial obj, std::vector<OPBConstraint> cons): objective(std::move(obj)), constraints(std::move(cons)) {}
};

boost::optional<OPBFile> parseOPBFile(std::ifstream& in);

template<typename Pol>
class OPBImporter {
private:
	std::ifstream mIn;

public:
	explicit OPBImporter(const std::string& filename):
		mIn(filename)
	{}
	
	boost::optional<std::pair<Formula<Pol>,Pol>> parse() {
		auto file = parseOPBFile(mIn);
		if (!file) return boost::none;
		Formulas<Pol> constraints;
		for (const auto& cons: file->constraints) {
			PBConstraint<Pol> pbc(std::move(std::get<0>(cons)), std::get<1>(cons), std::get<2>(cons));
			constraints.emplace_back(std::move(pbc));
		}
		Formula<Pol> resC(FormulaType::AND, std::move(constraints));
		Pol objective;
		for (const auto& term: file->objective) {
			objective += typename Pol::CoeffType(term.first) * term.second;
		}
		return std::make_pair(std::move(resC), std::move(objective));
	}
};

}

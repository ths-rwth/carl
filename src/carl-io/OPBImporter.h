#pragma once

#include <carl-logging/carl-logging.h>
#include <carl-arith/core/Relation.h>
#include <carl-formula/formula/Formula.h>

#include <iostream>
#include <fstream>
#include <map>
#include <optional>
#include <tuple>
#include <vector>

namespace carl::io {

using OPBPolynomial = std::vector<std::pair<int,carl::Variable>>;
using OPBConstraint = std::tuple<OPBPolynomial, Relation, int>;

struct OPBFile {
	OPBPolynomial objective;
	std::vector<OPBConstraint> constraints;
	
	OPBFile() = default;
	explicit OPBFile(OPBPolynomial obj): objective(std::move(obj)) {}
	OPBFile(OPBPolynomial obj, std::vector<OPBConstraint> cons): objective(std::move(obj)), constraints(std::move(cons)) {}
};

std::optional<OPBFile> parseOPBFile(std::ifstream& in);

template<typename Pol>
class OPBImporter {
private:
	using Number = typename UnderlyingNumberType<Pol>::type;
	std::ifstream mIn;

	std::map<carl::Variable, carl::Variable> variableCache; // maps old int variables to bool

	carl::MultivariatePolynomial<Number> convert(const std::vector<std::pair<int,carl::Variable>>& poly) {
		Pol lhs;
		for (const auto& term: poly) {
			auto it = variableCache.find(term.second);
			if (it == variableCache.end()) {
				// We haven't seen this variable, yet. Create a new map entry for it.
				carl::Variable boolVar = carl::fresh_boolean_variable();
				variableCache[term.second] = boolVar;
			}

			const carl::Variable& booleanVariable = variableCache[term.second];
			lhs += Pol(Number(term.first)) * Pol(booleanVariable);
		}

		return lhs;
	}

public:
	explicit OPBImporter(const std::string& filename):
		mIn(filename)
	{}
	
	std::optional<std::pair<Formula<Pol>,Pol>> parse() {
		if (!mIn.is_open()) return std::nullopt;

		auto file = parseOPBFile(mIn);
		if (!file) return std::nullopt;
		Formulas<Pol> constraints;
		for (const auto& cons: file->constraints) {
			auto lhs = convert(std::get<0>(cons));
			Relation rel = std::get<1>(cons);
			Number rhs = std::get<2>(cons);
			Constraint<Pol> pbc(lhs - Pol(rhs), rel);
			constraints.emplace_back(std::move(pbc));
		}
		Formula<Pol> resC(FormulaType::AND, std::move(constraints));
		Pol objective;
		for (const auto& term: file->objective) {
			objective += Number(term.first) * term.second;
		}
		return std::make_pair(std::move(resC), std::move(objective));
	}
};

}

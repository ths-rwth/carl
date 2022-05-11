#pragma once

#include <carl-common/config.h>
#include "../formula/Formula.h"

#include <bliss/graph.hh>

#include <map>

namespace carl {
namespace formula {
namespace symmetry {

/**
 * Special colors for structure nodes.
 * - If: condition from ite
 * - Then: first case from ite
 * - Else: second case from ite
 * - VarExp: pair of variable and exponent in terms
 */
enum class SpecialColors { If, Then, Else, VarExp };

/**
 * Provides unique ids (colors) for all kinds of different objects in the formula:
 * variable types, relations, formula types, numbers, special colors and indexes.
 */
template<typename Number>
class ColorGenerator {
public:
private:
	unsigned nextID = 0;
	std::map<carl::VariableType, unsigned> mVT;
	std::map<carl::Relation, unsigned> mRel;
	std::map<carl::FormulaType, unsigned> mFT;
	std::map<Number, unsigned> mConst;
	std::map<SpecialColors, unsigned> mSpecial;
	std::map<std::size_t, unsigned> mIndexes;
	
	template<typename T>
	unsigned findOrInsert(std::map<T,unsigned>& container, const T& value) {
		auto it = container.emplace(value, nextID);
		if (it.second) nextID++;
		return it.first->second;
	}
public:
	unsigned next() const {
		return nextID;
	}
	unsigned operator()(carl::VariableType v) {
		return findOrInsert(mVT, v);
	}
	unsigned operator()(carl::Relation v) {
		return findOrInsert(mRel, v);
	}
	unsigned operator()(carl::FormulaType v) {
		return findOrInsert(mFT, v);
	}
	unsigned operator()(const Number& v) {
		return findOrInsert(mConst, v);
	}
	unsigned operator()(SpecialColors v) {
		return findOrInsert(mSpecial, v);
	}
	unsigned operator()(std::size_t v) {
		return findOrInsert(mIndexes, v);
	}
};

struct Permutation {
	std::vector<std::vector<unsigned>> data;
};

void addGenerator(void* p, const unsigned int n, const unsigned int* aut) {
	static_cast<Permutation*>(p)->data.emplace_back(aut, aut+n);
}

template<typename Poly>
class GraphBuilder {
	using Number = typename Poly::NumberType;
	bliss::Digraph mGraph;
	ColorGenerator<Number> mColor;
	std::map<carl::Variable,unsigned> mVariableIDs;
	std::vector<carl::Variable> mVariables;
	
	void gatherVariables(const Formula<Poly>& f) {
		assert(mColor.next() == 0);
		carlVariables vars;
		carl::variables(f,vars);
		for (const auto& v: vars) {
			auto res = mVariableIDs.emplace(v, mGraph.add_vertex(mColor(v.type())));
			assert(res.first->second == mVariables.size());
			mVariables.emplace_back(v);
		}
	}
	unsigned addTerm(const Term<Number>& t) {
		unsigned vert = mGraph.add_vertex(mColor(t.coeff()));
		if (t.monomial()) {
			for (const auto& ve: *t.monomial()) {
				for (std::size_t i = 0; i < ve.second; ++i) {
					unsigned tmp = mGraph.add_vertex(mColor(SpecialColors::VarExp));
					mGraph.add_edge(vert, tmp);
					mGraph.add_edge(tmp, mVariableIDs[ve.first]);
				}
			}
		}
		return vert;
	}
	unsigned addConstraint(const Constraint<Poly>& c) {
		unsigned vert = mGraph.add_vertex(mColor(c.relation()));
		for (const auto& term: c.lhs()) {
			mGraph.add_edge(vert, addTerm(term));
		}
		return vert;
	}
	unsigned addFormula(const Formula<Poly>& f) {
		unsigned vert = mGraph.add_vertex(mColor(f.getType()));
		switch (f.getType()) {
			case carl::FormulaType::ITE: {
				unsigned ifvert = mGraph.add_vertex(mColor(SpecialColors::If));
				mGraph.add_edge(vert, ifvert);
				mGraph.add_edge(ifvert, addFormula(f.condition()));
				unsigned thenvert = mGraph.add_vertex(mColor(SpecialColors::Then));
				mGraph.add_edge(vert, thenvert);
				mGraph.add_edge(thenvert, addFormula(f.firstCase()));
				unsigned elsevert = mGraph.add_vertex(mColor(SpecialColors::Else));
				mGraph.add_edge(vert, elsevert);
				mGraph.add_edge(elsevert, addFormula(f.secondCase()));
				break;
			}
			case carl::FormulaType::EXISTS:
			case carl::FormulaType::FORALL:
			case carl::FormulaType::TRUE:
			case carl::FormulaType::FALSE:
				break;
			case carl::FormulaType::BOOL:
				mGraph.add_edge(vert, mVariableIDs[f.boolean()]);
				break;
			case carl::FormulaType::NOT:
				mGraph.add_edge(vert, addFormula(f.subformula()));
				break;
			case carl::FormulaType::IMPLIES:
			{
				std::size_t cur = 1;
				for (const auto& sf: f.subformulas()) {
					unsigned idvert = mGraph.add_vertex(mColor(cur));
					mGraph.add_edge(vert, idvert);
					mGraph.add_edge(idvert, addFormula(sf));
					cur++;
				}
				break;
			}
			case carl::FormulaType::AND:
			case carl::FormulaType::OR:
			case carl::FormulaType::XOR:
			case carl::FormulaType::IFF:
				for (const auto& sf: f.subformulas()) {
					mGraph.add_edge(vert, addFormula(sf));
				}
				break;
			case carl::FormulaType::CONSTRAINT:
				mGraph.add_edge(vert, addConstraint(f.constraint()));
				break;
			case carl::FormulaType::VARCOMPARE:
			case carl::FormulaType::VARASSIGN:
			case carl::FormulaType::BITVECTOR:
			case carl::FormulaType::UEQ:
				break;
		}
		return vert;
	}
public:
	GraphBuilder(const Formula<Poly>& f) {
		gatherVariables(f);
		addFormula(f);
	}
	Symmetries symmetries() {
		bliss::Stats stats;
		Permutation p;
		mGraph.find_automorphisms(stats, &addGenerator, &p);
		Symmetries varsymms;
		for (const auto& s: p.data) {
			bool foundChange = false;
			Symmetry tmp(mVariables.size());
			for (unsigned i = 0; i < mVariables.size(); ++i) {
				assert(s[i] < mVariables.size());
				tmp[i] = std::make_pair(mVariables[i], mVariables[s[i]]);
				if (s[i] != i) foundChange = true;
			}
			if (foundChange) {
				varsymms.emplace_back(tmp);
			}
		}
		return varsymms;
	}
};

}
}
}

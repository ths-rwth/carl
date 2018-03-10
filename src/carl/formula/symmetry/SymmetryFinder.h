#pragma once

#include "../../config.h"

#ifdef USE_BLISS

#include "../Formula.h"

#include <bliss/graph.hh>

#include <map>

namespace carl {
namespace formula {
namespace symmetry {

enum class SpecialColors { VarExp };

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
	
	void collectVariables(const Formula<Poly>& f) {
		assert(mColor.next() == 0);
		Variables var;
		f.allVars(var);
		for (auto v: var) {
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
		std::cout << "Adding " << f << std::endl;
		unsigned vert = mGraph.add_vertex(mColor(f.getType()));
		switch (f.getType()) {
			case carl::FormulaType::ITE:
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
			case carl::FormulaType::PBCONSTRAINT:
				break;
		}
		return vert;
	}
public:
	GraphBuilder(const Formula<Poly>& f) {
		collectVariables(f);
		addFormula(f);
		mGraph.write_dot("test2.dot");
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

#endif

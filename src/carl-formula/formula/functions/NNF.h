#pragma once

#include "Negations.h"
#include "aux.h"

namespace carl {

template<typename Poly>
Formula<Poly> to_nnf(const Formula<Poly>& formula) {
    if(formula.type() == carl::FormulaType::TRUE || formula.type() == carl::FormulaType::FALSE){
        return formula;
    }
    if(formula.is_atom()){
        return formula;
    }
    if(formula.is_literal()){
        return resolve_negation(formula, false);
    }

    switch(formula.type()){
        case carl::FormulaType::NOT:
            return to_nnf(resolve_negation(formula, false));
        case carl::FormulaType::IMPLIES: {
            auto premise = to_nnf(formula.premise().negated());
            auto conclusion = to_nnf(formula.conclusion());
            return Formula<Poly>(carl::FormulaType::OR, Formulas<Poly>{ premise, conclusion});
        }
        case carl::FormulaType::IFF: {
            Formulas<Poly> poss;
            Formulas<Poly> negs;
            for (const auto& f : formula.subformulas()) {
                poss.emplace_back(to_nnf(f));
                negs.emplace_back(to_nnf(f.negated()));
            }
            Formula<Poly> pos(carl::FormulaType::AND, std::move(poss));
            Formula<Poly> neg(carl::FormulaType::AND, std::move(negs));
            return Formula<Poly>(carl::FormulaType::OR, Formulas<Poly>({ pos, neg }));
        }
        case carl::FormulaType::XOR: {
            auto lhs = formula::aux::connectPrecedingSubformulas(formula);
            const auto& rhs = formula.subformulas().back();
            Formula<Poly> pos(FormulaType::OR, { to_nnf(lhs), to_nnf(rhs) });
            Formula<Poly> neg(FormulaType::OR, { to_nnf(!lhs), to_nnf(!rhs) });
            return Formula<Poly>(carl::FormulaType::AND, Formulas<Poly>({ pos, neg }));
        }
        case carl::FormulaType::ITE: {
			Formula<Poly> first(FormulaType::OR, {to_nnf(!formula.condition()), to_nnf(formula.first_case())});
			Formula<Poly> second(FormulaType::OR, {to_nnf(formula.condition()), to_nnf(formula.second_case())});
            return Formula<Poly>(carl::FormulaType::AND, Formulas<Poly>({ first, second }));
        }
        case carl::FormulaType::OR:{
            Formulas<Poly> disjunctions;
            for(auto subformula : formula.subformulas()){
                disjunctions.push_back(to_nnf(subformula));
            }
            return Formula<Poly>(carl::FormulaType::OR, std::move(disjunctions));
        }
        case carl::FormulaType::AND:{
            Formulas<Poly> conjunctions;
            for(auto subformula : formula.subformulas()){
                conjunctions.push_back(to_nnf(subformula));
            }
            return Formula<Poly>(carl::FormulaType::AND, std::move(conjunctions));
        }
        default:
            assert(false);
            return Formula<Poly>(carl::FormulaType::FALSE);
    }

}

}
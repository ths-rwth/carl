#pragma once

#include "Negations.h"

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
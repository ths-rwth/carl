#pragma once

#include "Visit.h"
#include <carl/core/Variables.h>

namespace carl{

template<typename Pol>
void variables(const Formula<Pol>& f, carlVariables& vars) {
    carl::visit(f,
        [&vars](const Formula<Pol>& f) {
            switch (f.type()) {
                case FormulaType::BOOL:
                    vars.add(f.boolean());
                    break;
                case FormulaType::CONSTRAINT:
                    carl::variables(f.constraint(), vars);
                    break;
                case FormulaType::VARCOMPARE:
                    carl::variables(f.variableComparison(), vars);
                    break;
                case FormulaType::VARASSIGN:
                    carl::variables(f.variableAssignment(), vars);
                    break;
                case FormulaType::BITVECTOR:
                    f.bvConstraint().gatherVariables(vars);
                    break;
                case FormulaType::UEQ:
                    f.uequality().gatherVariables(vars);
                    break;
                case FormulaType::EXISTS:
                case FormulaType::FORALL:
                    vars.add(f.quantifiedVariables().begin(),f.quantifiedVariables().end());
                    break;
                default: break;
            }
        }
    );
}

template<typename Pol>
void uninterpreted_functions(const Formula<Pol>& f, std::set<UninterpretedFunction>& ufs) {
    carl::visit(f,
        [&ufs](const Formula<Pol>& f) {
            if (f.type() == FormulaType::UEQ) {
                f.uequality().gatherUFs(ufs);
            }
        }
    );
}

template<typename Pol>
void uninterpreted_variables(const Formula<Pol>& f, std::set<UVariable>& uvs) {
    carl::visit(f,
        [&uvs](const Formula<Pol>& f) {
            if (f.type() == FormulaType::UEQ) {
                f.uequality().gatherUVariables(uvs);
            }
        }
    );
}

template<typename Pol>
void bitvector_variables(const Formula<Pol>& f, std::set<BVVariable>& bvvs) {
    carl::visit(f,
        [&bvvs](const Formula<Pol>& f) {
            if (f.type() == FormulaType::BITVECTOR) {
                f.bvConstraint().gatherBVVariables(bvvs);
            }
        }
    );
}

/**
 * Collects all constraint occurring in this formula.
 * @param constraints The container to insert the constraint into.
 */
template<typename Pol>
void arithmetic_constraints(const Formula<Pol>& f, std::vector<Constraint<Pol>>& constraints ) {
    carl::visit(f,
        [&constraints](const Formula<Pol>& f) {
            switch (f.type()) {
                case FormulaType::CONSTRAINT:
                    constraints.push_back(f.constraint());
                    break;
                default: break;
            }
        }
    );
}

/**
 * Collects all constraint occurring in this formula.
 * @param constraints The container to insert the constraint into.
 */
template<typename Pol>
void arithmetic_constraints(const Formula<Pol>& f, std::vector<Formula<Pol>>& constraints ) {
    carl::visit(f,
        [&constraints](const Formula<Pol>& f) {
            switch (f.type()) {
                case FormulaType::CONSTRAINT:
                    constraints.push_back(f);
                    break;
                default: break;
            }
        }
    );
}

}

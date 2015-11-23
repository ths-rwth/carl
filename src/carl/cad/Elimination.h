#pragma once

#include "../core/logging.h"
#include "../core/Variable.h"

namespace carl {
namespace cad {

    enum class ProjectionType: unsigned {
        McCallum, Hong
    };

    template<typename Poly>
    struct ProjectionOperator {
        template<typename Inserter>
        void operator()(ProjectionType pt, const Poly& p, Variable::Arg variable, Inserter i) const {
            switch (pt) {
                case ProjectionType::McCallum: return McCallum(p, variable, i);
                default:
                    CARL_LOG_ERROR("carl.cad", "Selected a projection operator that is not implemented.");
                    return;
            }
        }
        template<typename Inserter>
        void operator()(ProjectionType pt, const Poly& p, const Poly& q, Variable::Arg variable, Inserter i) const {
            switch (pt) {
                case ProjectionType::McCallum: return McCallum(p, q, variable, i);
                default:
                    CARL_LOG_ERROR("carl.cad", "Selected a projection operator that is not implemented.");
                    return;
            }
        }

        /**
         * Tries to determine whether the given polynomial vanishes for some assignment.
         * Returns true if the polynomial is guaranteed not to vanish.
         * Returns false otherwise.
         */
        template<typename PolyCoeff>
        bool doesNotVanish(const PolyCoeff& p) const {
            if (p.isZero()) return false;
            if (p.isConstant()) return true;
            auto def = p.definiteness();
            if (def == Definiteness::POSITIVE) return true;
            if (def == Definiteness::NEGATIVE) return true;
            return false;
        }

        template<typename Inserter>
        void McCallum(const Poly& p, const Poly& q, Variable::Arg variable, Inserter i) const {
            i.insert(p->resultant(*q).switchVariable(variable), {p, q}, false);
        }
        template<typename Inserter>
        void McCallum(const Poly& p, Variable::Arg variable, Inserter i) const {
            // Insert discriminant
            i.insert(p->discriminant().switchVariable(variable), {p}, false);
            if (doesNotVanish(p->lcoeff())) return;
            for (const auto& coeff: p->coefficients()) {
                if (doesNotVanish(coeff)) {
                    i.insert(p->lcoeff().toUnivariatePolynomial(variable), {p}, false);
                    return;
                }
            }
            for (const auto& coeff: p->coefficients()) {
                i.insert(coeff.toUnivariatePolynomial(variable), {p}, false);
            }
        }
    };

}
}

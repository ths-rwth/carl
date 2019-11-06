#pragma once

#include <carl/core/logging.h>
#include <carl/core/Variable.h>
#include <carl/core/polynomialfunctions/Resultant.h>

namespace carl {
namespace cad {

    enum class ProjectionType: unsigned {
        Brown, McCallum, Hong
    };

    template<typename Poly>
    struct ProjectionOperator {
        template<typename Inserter>
        void operator()(ProjectionType pt, const Poly& p, Variable::Arg variable, Inserter& i) const {
            switch (pt) {
				case ProjectionType::Brown: return Brown(p, variable, i);
                case ProjectionType::McCallum: return McCallum(p, variable, i);
                default:
                    CARL_LOG_ERROR("carl.cad", "Selected a projection operator that is not implemented.");
                    return;
            }
        }
        template<typename Inserter>
        void operator()(ProjectionType pt, const Poly& p, const Poly& q, Variable::Arg variable, Inserter& i) const {
            switch (pt) {
				case ProjectionType::Brown: return Brown(p, q, variable, i);
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
            if (carl::isZero(p)) return false;
            if (p.isConstant()) return true;
            auto def = p.definiteness();
            if (def == Definiteness::POSITIVE) return true;
            if (def == Definiteness::NEGATIVE) return true;
            return false;
        }

		template<typename Inserter>
		void Brown(const Poly& p, const Poly& q, Variable::Arg variable, Inserter& i) const {
			CARL_LOG_DEBUG("carl.cad.projection", "resultant(" << p << ", " << q << ")");
			i.insert(switch_main_variable(carl::resultant(*p, *q), variable), {p, q}, false);
		}
		template<typename Inserter>
		void Brown(const Poly& p, Variable::Arg variable, Inserter& i) const {
			// Insert discriminant
			CARL_LOG_DEBUG("carl.cad.projection", "discriminant(" << p << ")");
			i.insert(switch_main_variable(carl::discriminant(*p), variable), {p}, false);
			if (doesNotVanish(p->lcoeff())) {
				CARL_LOG_DEBUG("carl.cad.projection", "lcoeff = " << p->lcoeff() << " does not vanish. No further polynomials needed.");
				return;
			}
			for (const auto& coeff: p->coefficients()) {
				if (doesNotVanish(coeff)) {
					CARL_LOG_DEBUG("carl.cad.projection", "coeff " << coeff << " does not vanish. We only need the lcoeff()");
					i.insert(carl::to_univariate_polynomial(p->lcoeff(), variable), {p}, false);
					return;
				}
			}
			CARL_LOG_DEBUG("carl.cad.projection", "All coefficients might vanish, we need all of them.");
			for (const auto& coeff: p->coefficients()) {
				if (coeff.isConstant()) continue;
				CARL_LOG_DEBUG("carl.cad.projection", "\t-> " << coeff);
				i.insert(carl::to_univariate_polynomial(coeff, variable), {p}, false);
			}
		}
        template<typename Inserter>
        void McCallum(const Poly& p, const Poly& q, Variable::Arg variable, Inserter& i) const {
			CARL_LOG_DEBUG("carl.cad.projection", "resultant(" << p << ", " << q << ")");
            i.insert(switch_main_variable(carl::resultant(*p, *q), variable), {p, q}, false);
        }
        template<typename Inserter>
        void McCallum(const Poly& p, Variable::Arg variable, Inserter& i) const {
            // Insert discriminant
			CARL_LOG_DEBUG("carl.cad.projection", "discriminant(" << p << ")");
            i.insert(switch_main_variable(carl::discriminant(*p), variable), {p}, false);
            for (const auto& coeff: p->coefficients()) {
				if (coeff.isConstant()) continue;
				CARL_LOG_DEBUG("carl.cad.projection", "\t-> " << coeff);
                i.insert(carl::to_univariate_polynomial(coeff, variable), {p}, false);
            }
        }
    };

}
}

#pragma once

#include "LazardEvaluation.h"
#include "AlgebraicSubstitution.h"

namespace carl::ran::interval {
    template<typename Number, typename Coeff>
    std::optional<UnivariatePolynomial<Number>> substitute_rans_into_polynomial(
            const UnivariatePolynomial<Coeff>& p,
            const OrderedAssignment<IntRepRealAlgebraicNumber<Number>>& m,
            bool use_lazard = false // TODO revert
    ) {
        std::vector<MultivariatePolynomial<Number>> polys;
        std::vector<Variable> varOrder;

        if (use_lazard) {
            CARL_LOG_TRACE("carl.ran.interval", "Substituting using Lazard evaluation");
            auto le = LazardEvaluation<Number, MultivariatePolynomial<Number>>(MultivariatePolynomial<Number>(p));
            for (const auto& vic: m) {
                varOrder.emplace_back(vic.first);
                auto res = le.substitute(vic.first, vic.second, true);
                if (res.first) {
                    polys.emplace_back(vic.first - res.second);
                } else {
                    polys.emplace_back(res.second);
                }
                CARL_LOG_TRACE("carl.ran.interval", vic.first << " -> " << vic.second << " is now " << polys.back());
            }
            polys.emplace_back(le.getLiftingPoly());
            varOrder.emplace_back(p.main_var());
            CARL_LOG_TRACE("carl.ran.interval", "main poly " << p << " in " << p.main_var() << " is now " << polys.back());
        } else {
            CARL_LOG_TRACE("carl.ran.interval", "Substituting using field extensions only");
            FieldExtensions<Number, MultivariatePolynomial<Number>> fe;
            for (const auto& vic: m) {
                varOrder.emplace_back(vic.first);
                auto res = fe.extend(vic.first, vic.second);
                if (res.first) {
                    polys.emplace_back(vic.first - res.second);
                } else {
                    polys.emplace_back(res.second);
                }
                CARL_LOG_TRACE("carl.ran.interval", vic.first << " -> " << vic.second << " is now " << polys.back());
            }
            polys.emplace_back(p);
            varOrder.emplace_back(p.main_var());
            CARL_LOG_TRACE("carl.ran.interval", "main poly " << p << " in " << p.main_var() << " is now " << polys.back());
        }

        CARL_LOG_TRACE("carl.ran.interval", "Perform algebraic substitution on " << polys << " wrt " << varOrder);
        return algebraic_substitution(polys, varOrder);
    }
}
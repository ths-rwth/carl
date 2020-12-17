#pragma once

#include "LazardEvaluation.h"
#include "AlgebraicSubstitution.h"

namespace carl::ran::interval {
    template<typename Number, typename Coeff>
    std::optional<UnivariatePolynomial<Number>> substitute_rans_into_polynomial(
            const UnivariatePolynomial<Coeff>& p,
            const ordered_ran_assignment_t<real_algebraic_number_interval<Number>>& m,
            bool use_lazard = false // TODO revert
    ) {
        std::vector<MultivariatePolynomial<Number>> polys;
        std::vector<Variable> varOrder;

        if (use_lazard) {
            CARL_LOG_TRACE("carl.ran", "Substituting using Lazard evaluation");
            auto le = LazardEvaluation<Number, MultivariatePolynomial<Number>>(MultivariatePolynomial<Number>(p));
            for (const auto& vic: m) {
                varOrder.emplace_back(vic.first);
                auto res = le.substitute(vic.first, vic.second, true);
                if (res.first) {
                    polys.emplace_back(vic.first - res.second);
                } else {
                    polys.emplace_back(res.second);
                }
                CARL_LOG_TRACE("carl.ran", vic.first << " -> " << vic.second << " is now " << polys.back());
            }
            polys.emplace_back(le.getLiftingPoly());
            varOrder.emplace_back(p.mainVar());
            CARL_LOG_TRACE("carl.ran", "main poly " << p << " in " << p.mainVar() << " is now " << polys.back());
        } else {
            CARL_LOG_TRACE("carl.ran", "Substituting using field extensions only");
            FieldExtensions<Number, MultivariatePolynomial<Number>> fe;
            for (const auto& vic: m) {
                varOrder.emplace_back(vic.first);
                auto res = fe.extend(vic.first, vic.second);
                if (res.first) {
                    polys.emplace_back(vic.first - res.second);
                } else {
                    polys.emplace_back(res.second);
                }
                CARL_LOG_TRACE("carl.ran", vic.first << " -> " << vic.second << " is now " << polys.back());
            }
            polys.emplace_back(p);
            varOrder.emplace_back(p.mainVar());
            CARL_LOG_TRACE("carl.ran", "main poly " << p << " in " << p.mainVar() << " is now " << polys.back());
        }

        CARL_LOG_TRACE("carl.ran", "Perform algebraic substitution on " << polys << " wrt " << varOrder);
        return algebraic_substitution(polys, varOrder);
    }
}
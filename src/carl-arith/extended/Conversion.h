#pragma once

#include <carl-common/config.h>
#include "../poly/Conversion.h"

namespace carl {

template<typename ToPoly, typename FromPoly, typename = std::enable_if_t<needs_context_type<ToPoly>::value>>
inline VariableComparison<ToPoly> convert(const typename ToPoly::ContextType& context, const VariableComparison<FromPoly>& c) {
    auto k = [&](){
        if (std::holds_alternative<MultivariateRoot<FromPoly>>(c.value())) {
            return std::get<MultivariateRoot<FromPoly>>(c.value()).k();
        } else {
            auto& ran = std::get<typename FromPoly::RootType>(c.value());
            if (ran.is_numeric()) {
                return (std::size_t)1;
            } else {
                // TODO circumvent this (i.e. store k inside the RAN, or store variable comparison always as MultivariateRoot combined with a concrete value)
                auto roots = carl::real_roots(ran.polynomial());
                auto it = std::find(roots.roots().begin(), roots.roots().end(), ran);
                assert(it != roots.roots().end());
                return (std::size_t)std::distance(roots.roots().begin(), it)+1;
            }
        }
    }();
    MultivariateRoot<ToPoly> mv(convert<ToPoly>(context, defining_polynomial(c)), k, c.var());
    return VariableComparison<ToPoly>(c.var(), mv, c.relation(), c.negated());
}
template<typename ToPoly, typename FromPoly, typename = std::enable_if_t<!needs_context_type<ToPoly>::value>>
inline VariableComparison<ToPoly> convert(const VariableComparison<FromPoly>& c) {
    assert((std::holds_alternative<MultivariateRoot<FromPoly>>(c.value())));
    const auto& m = std::get<MultivariateRoot<FromPoly>>(c.value());
    MultivariateRoot<ToPoly> mv(convert<ToPoly>(m.poly()), m.k(), m.var());
    return VariableComparison<ToPoly>(c.var(), mv, c.relation(), c.negated());
}

}
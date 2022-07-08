#pragma once

#include <carl-common/config.h>
#include "../poly/Conversion.h"

namespace carl {

template<typename ToPoly, typename FromPoly, typename = std::enable_if_t<needs_context_type<ToPoly>::value>>
VariableComparison<ToPoly> convert(const typename ToPoly::ContextType& context, const VariableComparison<FromPoly>& c) {
    auto k = std::holds_alternative<MultivariateRoot<FromPoly>>(c.value()) ? std::get<MultivariateRoot<FromPoly>>(c.value()).k() : 1;
    MultivariateRoot<ToPoly> mv(convert(context, defining_polynomial(c)), k, c.var());
    return VariableComparison<ToPoly>(c.var(), mv, c.relation(), c.negated());
}
template<typename ToPoly, typename FromPoly, typename = std::enable_if_t<!needs_context_type<ToPoly>::value>>
VariableComparison<ToPoly> convert(const VariableComparison<FromPoly>& c) {
    assert((std::holds_alternative<MultivariateRoot<FromPoly>>(c.value())));
    const auto& m = std::get<MultivariateRoot<FromPoly>>(c.value());
    MultivariateRoot<ToPoly> mv(convert(m.poly()), m.k(), m.var());
    return VariableComparison<ToPoly>(c.var(), mv, c.relation(), c.negated());
}

}
#pragma once

#include <carl-common/config.h>
#include "../poly/Conversion.h"

namespace carl {

template<typename ToPoly, typename FromPoly, typename = std::enable_if_t<needs_context_type<ToPoly>::value>>
inline BasicConstraint<ToPoly> convert(const typename ToPoly::ContextType& context, const BasicConstraint<FromPoly>& c) {
    return BasicConstraint<ToPoly>(convert<ToPoly>(context, c.lhs()), c.relation());
}
template<typename ToPoly, typename FromPoly, typename = std::enable_if_t<!needs_context_type<ToPoly>::value>>
inline BasicConstraint<ToPoly> convert(const BasicConstraint<FromPoly>& c) {
    return BasicConstraint<ToPoly>(convert<ToPoly>(c.lhs()), c.relation());
}

}
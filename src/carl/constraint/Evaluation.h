#pragma once

#include "BasicConstraint.h"
#include <carl/core/Common.h>
#include "../poly/umvpoly/functions/Substitution.h"

namespace carl {

template<typename Number, typename Poly, typename = std::enable_if_t<is_number<Number>::value>>
bool evaluate(const BasicConstraint<Poly>& c, const Assignment<Number>& m) {
	auto res = evaluate(c.lhs(), m);
	return evaluate(res, c.relation());
}

/**
 * Checks whether the given assignment satisfies this constraint.
 * @param _assignment The assignment.
 * @return 1, if the given assignment satisfies this constraint.
 *          0, if the given assignment contradicts this constraint.
 *          2, otherwise (possibly not defined for all variables in the constraint,
 *                       even then it could be possible to obtain the first two results.)
 */
template<typename Pol>
unsigned satisfied_by(const BasicConstraint<Pol>& c, const Assignment<typename Pol::NumberType>& _assignment) {
	unsigned result = 2;
	Pol tmp = carl::substitute(c.lhs(), _assignment);
	if (tmp.isConstant()) {
		result = carl::evaluate((isZero(tmp) ? typename Pol::NumberType(0) : tmp.trailingTerm().coeff()), c.relation()) ? 1 : 0;
	}
	return result;
}

}
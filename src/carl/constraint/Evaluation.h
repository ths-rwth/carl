#pragma once

#include "BasicConstraint.h"
#include "../util/Common.h"
#include "../core/polynomialfunctions/Substitution.h"

namespace carl {

/**
 * Checks whether the given assignment satisfies this constraint.
 * @param _assignment The assignment.
 * @return 1, if the given assignment satisfies this constraint.
 *          0, if the given assignment contradicts this constraint.
 *          2, otherwise (possibly not defined for all variables in the constraint,
 *                       even then it could be possible to obtain the first two results.)
 */
template<typename Pol>
unsigned satisfiedBy(const BasicConstraint<Pol>& c, const EvaluationMap<typename Pol::NumberType>& _assignment) {
	unsigned result = 2;
	Pol tmp = carl::substitute(c.lhs(), _assignment);
	if (tmp.isConstant()) {
		result = carl::evaluate((isZero(tmp) ? typename Pol::NumberType(0) : tmp.trailingTerm().coeff()), c.relation()) ? 1 : 0;
	}
	return result;
}

}
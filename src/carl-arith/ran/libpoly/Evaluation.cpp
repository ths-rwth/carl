#include "Evaluation.h"

#ifdef USE_LIBPOLY

#include "LPAssignment.h"


namespace carl {

std::optional<LPRealAlgebraicNumber> evaluate(
	const LPPolynomial& polynomial,
	const std::map<Variable, LPRealAlgebraicNumber>& evalMap) {
	lp_assignment_t& assignment = LPAssignment::getInstance().get(evalMap);
	auto result = lp_polynomial_evaluate(polynomial.get_internal(), &assignment);

	if (result->type == LP_VALUE_NONE) {
		CARL_LOG_DEBUG("carl.ran.libpoly", " Result: NONE");
		return std::nullopt;
	}

	auto ran = LPRealAlgebraicNumber(std::move(*result));
	CARL_LOG_DEBUG("carl.ran.libpoly", " Result: " << ran);
	return ran;
}

inline auto lp_sign(carl::Relation rel) {
	switch (rel) {
	case Relation::EQ:
		return LP_SGN_EQ_0;
	case Relation::NEQ:
		return LP_SGN_NE_0;
	case Relation::LESS:
		return LP_SGN_LT_0;
	case Relation::LEQ:
		return LP_SGN_LE_0;
	case Relation::GREATER:
		return LP_SGN_GT_0;
	case Relation::GEQ:
		return LP_SGN_GE_0;
	default:
		assert(false);
		return LP_SGN_EQ_0;
	}
}

boost::tribool evaluate(const BasicConstraint<LPPolynomial>& constraint, const std::map<Variable, LPRealAlgebraicNumber>& evalMap) {
	CARL_LOG_DEBUG("carl.ran.libpoly", " Evaluation constraint " << constraint << " for assignment " << evalMap);

	if (is_constant(constraint.lhs())) {
		return carl::evaluate(constraint.lhs().constant_part(), constraint.relation());
	}

	auto poly_pol = constraint.lhs().get_internal();
	lp_assignment_t& assignment = LPAssignment::getInstance().get(evalMap);

	for (const auto& v : carl::variables(constraint)) {
		if (evalMap.find(v) == evalMap.end()) {
			int result = lp_polynomial_constraint_evaluate_subs(poly_pol, lp_sign(constraint.relation()), &assignment);
			if (result == -1) return boost::indeterminate;
			else return result;
		}
	}
	
	return lp_polynomial_constraint_evaluate(poly_pol, lp_sign(constraint.relation()), &assignment);
}

}


#endif
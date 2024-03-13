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

boost::tribool evaluate(const BasicConstraint<LPPolynomial>& constraint, const std::map<Variable, LPRealAlgebraicNumber>& evalMap) {

	CARL_LOG_DEBUG("carl.ran.libpoly", " Evaluation constraint " << constraint << " for assignment " << evalMap);

	//Easy checks of lhs of constraint is constant
	if (is_constant(constraint.lhs())) {
		auto num = constraint.lhs().constant_part();
		switch (constraint.relation()) {
		case Relation::EQ:
			return num == 0;
		case Relation::NEQ:
			return num != 0;
		case Relation::LESS:
			return num < 0;
		case Relation::LEQ:
			return num <= 0;
		case Relation::GREATER:
			return num > 0;
		case Relation::GEQ:
			return num >= 0;
		default:
			assert(false);
			return false;
		}
	}

	for (const auto& v : carl::variables(constraint)) {
		if (evalMap.find(v) == evalMap.end()) return boost::indeterminate;
	}

	//denominator can be omitted
	auto poly_pol = constraint.lhs().get_internal();

	lp_assignment_t& assignment = LPAssignment::getInstance().get(evalMap);

	bool result = false;
	switch (constraint.relation()) {
	case Relation::EQ:
		result = lp_polynomial_constraint_evaluate(poly_pol, LP_SGN_EQ_0, &assignment);
		break;
	case Relation::NEQ:
		result = lp_polynomial_constraint_evaluate(poly_pol, LP_SGN_NE_0, &assignment);
		break;
	case Relation::LESS:
		result = lp_polynomial_constraint_evaluate(poly_pol, LP_SGN_LT_0, &assignment);
		break;
	case Relation::LEQ:
		result = lp_polynomial_constraint_evaluate(poly_pol, LP_SGN_LE_0, &assignment);
		break;
	case Relation::GREATER:
		result = lp_polynomial_constraint_evaluate(poly_pol, LP_SGN_GT_0, &assignment);
		break;
	case Relation::GEQ:
		result = lp_polynomial_constraint_evaluate(poly_pol, LP_SGN_GE_0, &assignment);
		break;
	default:
		assert(false);
	}
	return result;
}

}


#endif
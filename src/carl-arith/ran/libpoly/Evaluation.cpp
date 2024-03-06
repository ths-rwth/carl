#include "Evaluation.h"

#ifdef USE_LIBPOLY

namespace carl {

std::optional<LPRealAlgebraicNumber> evaluate(
	const LPPolynomial& polynomial,
	const std::map<Variable, LPRealAlgebraicNumber>& evalMap) {
	lp_assignment_t& assignment = LPVariables::getInstance().get_assignment();
	for (const auto& entry : evalMap) {
		lp_value_t val;
		lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, entry.second.get_internal());
		lp_assignment_set_value(&assignment, LPVariables::getInstance().lp_variable(entry.first), &val);
		lp_value_destruct(&val);
	}
	auto result = lp_polynomial_evaluate(polynomial.get_internal(), &assignment);

	if (result->type == LP_VALUE_NONE) {
		CARL_LOG_DEBUG("carl.ran.libpoly", " Result: NONE");
		return std::nullopt;
	}

	auto ran = LPRealAlgebraicNumber::create_from_value(result);
	CARL_LOG_DEBUG("carl.ran.libpoly", " Result: " << ran);
	lp_value_destruct(result);
	free(result);
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
	const poly::Polynomial& poly_pol = constraint.lhs().get_polynomial();

	lp_assignment_t& assignment = LPVariables::getInstance().get_assignment();
	for (const auto& entry : evalMap) {
		lp_value_t val; 
		lp_value_construct(&val, lp_value_type_t::LP_VALUE_ALGEBRAIC, entry.second.get_internal());
		lp_assignment_set_value(&assignment, LPVariables::getInstance().lp_variable(entry.first), &val);
		lp_value_destruct(&val);
	}

	bool result = false;
	switch (constraint.relation()) {
	case Relation::EQ:
		result = lp_polynomial_constraint_evaluate(
        poly_pol.get_internal(), LP_SGN_EQ_0, &assignment);
		break;
	case Relation::NEQ:
		result = lp_polynomial_constraint_evaluate(
        poly_pol.get_internal(), LP_SGN_NE_0, &assignment);
		break;
	case Relation::LESS:
		result = lp_polynomial_constraint_evaluate(
        poly_pol.get_internal(), LP_SGN_LT_0, &assignment);
		break;
	case Relation::LEQ:
		result = lp_polynomial_constraint_evaluate(
        poly_pol.get_internal(), LP_SGN_LE_0, &assignment);
		break;
	case Relation::GREATER:
		result = lp_polynomial_constraint_evaluate(
        poly_pol.get_internal(), LP_SGN_GT_0, &assignment);
		break;
	case Relation::GEQ:
		result = lp_polynomial_constraint_evaluate(
        poly_pol.get_internal(), LP_SGN_GE_0, &assignment);
		break;
	default:
		assert(false);
	}
	return result;
}

}


#endif
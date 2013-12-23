/* 
 * File:   Constraint.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <cassert>
#include <set>
#include <vector>

#include "../core/logging.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/Sign.h"
#include "../core/Variable.h"
#include "../interval/IntervalEvaluation.h"
#include "CADSettings.h"

namespace carl {
namespace cad {

template<typename Number>
class Constraint {
private:
	cad::UPolynomial<Number> polynomial;
	Sign sign;
	std::vector<Variable> variables;
	bool negated;
	
public:
	//////////////////////////
	// Con- and destructors //
	//////////////////////////

	/**
	 * Standard constructor.
	 */
	Constraint():
		polynomial(),
		sign(),
		variables(),
		negated(false)
	{}

	/**
	 * Constructs a constraint represented as a sign condition <code>p.sgn(...) == s</code> or a negated sign condition <code>p.sgn(...) != s</code>.
	 * @param p polynomial compared
	 * @param s sign comparing the sign of the polynomial to GiNaCRA::ZERO_SIGN, GiNaCRA::POSITVIE_SIGN, or GiNaCRA::NEGATIVE_SIGN.
	 * @param v the variables of the polynomial
	 * @param negated if set to <code>true</code>, <code>satisfiedBy</code> checks the negation of the specified sign condition. If otherwise <code>false</code> is specified (standard value), <code>satisfiedBy</code> checks the sign condition as specified.
	 */
	Constraint(const cad::UPolynomial<Number>& p, const Sign& s, const std::vector<Variable> v, bool negated = false):
		polynomial(p),
		sign(s),
		variables(checkVariables(p, v)),
		negated(negated)
	{}

	///////////////
	// Selectors //
	///////////////

	/**
	 * @return the polynomial of the constraint
	 */
	const cad::UPolynomial<Number>& getPolynomial() const {
		return this->polynomial;
	}

	/**
	 * @return the polynomial of the constraint
	 */
	const Sign& getSign() const {
		return this->sign;
	}

	/**
	 * @return the list of variables of this constraint
	 */
	const std::vector<Variable>& getVariables() const {
		return this->variables;
	}

	/**
	 * @return true if the constraint is negated
	 */
	bool isNegated() const {
		return this->negated;
	}

	////////////////
	// Operations //
	////////////////

	/**
	 * Test if the given point satisfies this constraint. The variables of this constraint are substituted by the components of r in ascending order until all variables are substituted, even if r is larger.
	 * @param r test point
	 * @return false if the constraint was not satisfied by the given point, true otherwise.
	 */
	bool satisfiedBy(const RealAlgebraicPoint<Number>& r) const {
		assert(this->variables.size() <= r.dim());
		
		std::map<Variable, ExactInterval<Number>> varMap;
		cad::UPolynomial<Number> p = this->polynomial;
		
		for (unsigned int i = 0; i < this->variables.size(); i++) {
			if (r[i]->isNumeric()) {
				p.substituteIn(this->variables[i], cad::MPolynomial<Number>(r[i]->value()));
			} else {
				varMap[this->variables[i]] = static_cast<const RealAlgebraicNumberIR<Number>*>(r[i])->getInterval();
			}
		}
		if (p.isNumber()) {
			if (this->negated) {
				return carl::sgn(p.numericContent()) != this->sign;
			} else {
				return carl::sgn(p.numericContent()) == this->sign;
			}
		}
		if (this->negated) {
			return carl::sgn(IntervalEvaluation::evaluate(p, varMap)) != this->sign;
		} else {
			return carl::sgn(IntervalEvaluation::evaluate(p, varMap)) == this->sign;
		}
	}

	/**
	 * Changes the variables of this constraint to start with v, where all other variables are being dropped.
	 * @param v
	 */
	void unifyVariables(const std::vector<Variable>& v) {
		this->variables.assign(v.begin(), v.end());
	}
	
	friend std::ostream& operator<<(std::ostream& os, const Constraint<Number>& s) {
		if (!s.negated) {
			switch (s.sign) {
				case Sign::NEGATIVE: os << s.polynomial << " < 0";
					break;
				case Sign::POSITIVE: os << s.polynomial << " > 0";
					break;
				case Sign::ZERO: os << s.polynomial << " = 0";
					break;
			}
		} else {
			switch (s.sign) {
				case Sign::NEGATIVE: os << s.polynomial << " >= 0";
					break;
				case Sign::POSITIVE: os << s.polynomial << " <= 0";
					break;
				case Sign::ZERO: os << s.polynomial << " != 0";
					break;
			}
		}
		return os;
	}
		
private:
	
	/**
	 * Verifies whether this constraint is constructed over no more than the given variables.
	 * The check is performed by an assert.
	 * @param p polynomial
	 * @param v list of variables
	 * @return v if the check was successful
	 */
	const std::vector<Variable> checkVariables(const cad::UPolynomial<Number>& p, const std::vector<Variable>& v) const {
		std::set<Variable> occuring = p.gatherVariables();
		for (Variable var: v) {
			occuring.erase(var);
		}
		assert(occuring.size() == 0);
		return v;
	}
};

}
}
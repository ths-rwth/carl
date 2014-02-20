/**
 * @file Constraint.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * Contains the Constraint class.
 */

#pragma once

#include <cassert>
#include <set>
#include <vector>

#include "../core/logging.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/RealAlgebraicPoint.h"
#include "../core/RealAlgebraicNumberEvaluation.h"
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
	{
		this->polynomial.checkConsistency();
	}

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
	bool satisfiedBy(RealAlgebraicPoint<Number>& r) const {
		assert(this->variables.size() <= r.dim());
		
		auto res = RealAlgebraicNumberEvaluation::evaluate(this->polynomial, r, this->variables);
		if (this->negated) {
			return res->sgn() != this->sign;
		} else {
			return res->sgn() == this->sign;
		}
	}

	/**
	 * Changes the variables of this constraint to start with v, where all other variables are being dropped.
	 * @param v
	 */
	void unifyVariables(const std::vector<Variable>& v) {
		this->variables.assign(v.begin(), v.end());
	}
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const Constraint<Num>& s);

	bool operator==(const Constraint<Number>& c) {
		if (this->polynomial != c.polynomial) return false;
		if (this->sign != c.sign) return false;
		if (this->negated != c.negated) return false;
		if (this->variables.size() != c.variables.size()) return false;
		for (unsigned i = 0; i < this->variables.size(); i++) {
			if (this->variables[i] != c.variables[i]) return false;
		}
		return true;
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

/**
 * Outputs the given constraint to an ostream.
 * 
 * The Constraint is displayed as polynomial inequality.
 * @param os output stream
 * @param c Constraint to be printed.
 * @return Output stream os.
 */
template<typename Number>
std::ostream& operator<<(std::ostream& os, const Constraint<Number>& c) {
	if (!c.negated) {
		switch (c.sign) {
			case Sign::NEGATIVE: os << c.polynomial << " < 0";
				break;
			case Sign::POSITIVE: os << c.polynomial << " > 0";
				break;
			case Sign::ZERO: os << c.polynomial << " = 0";
				break;
		}
	} else {
		switch (c.sign) {
			case Sign::NEGATIVE: os << c.polynomial << " >= 0";
				break;
			case Sign::POSITIVE: os << c.polynomial << " <= 0";
				break;
			case Sign::ZERO: os << c.polynomial << " != 0";
				break;
		}
	}
	return os;
}

}
}
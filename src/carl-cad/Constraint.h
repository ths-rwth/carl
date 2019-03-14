/**
 * @file cad/Constraint.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * Contains the Constraint class.
 */

#pragma once

#include <cassert>
#include <set>
#include <vector>

#include <carl/core/logging.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/Sign.h>
#include <carl/core/Variable.h>
#include <carl/formula/model/ran/RealAlgebraicPoint.h>
#include <carl/formula/model/ran/RealAlgebraicNumberEvaluation.h>
#include <carl/interval/IntervalEvaluation.h>
#include "CADSettings.h"
#include "Variables.h"

namespace carl {
namespace cad {

/**
 * Representation of a constraint of the form \f$p \sim 0\f$ where \f$p\f$ is some polynomial and \f$\sim \in \{ <, \leq, =, >, \geq, > \}\f$.
 */
template<typename Number>
class Constraint {
private:
	cad::MultiPolynomial<Number> polynomial;
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
	Constraint(const cad::MultiPolynomial<Number>& p, const Sign& s, const std::vector<Variable> v, bool _negated = false):
		polynomial(p),
		sign(s),
		variables(checkVariables(p, v)),
		negated(_negated)
	{
		assert(this->polynomial.isConsistent());
	}

	///////////////
	// Selectors //
	///////////////

	/**
	 * @return the polynomial of the constraint
	 */
	const cad::MultiPolynomial<Number>& getPolynomial() const {
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
	bool satisfiedBy(const RealAlgebraicPoint<Number>& r, const std::vector<Variable>& _variables) const {
		assert(_variables.size() == r.dim());
		
		auto res = RealAlgebraicNumberEvaluation::evaluate(this->polynomial, r, _variables);
		CARL_LOG_DEBUG("carl.cad.constraint", *this << " evaluates to " << res << " on " << r);
		if (this->negated) {
			return res.sgn() != this->sign;
		} else {
			return res.sgn() == this->sign;
		}
	}

	/**
	 * Changes the variables of this constraint to start with v, where all other variables are being dropped.
	 * @param v
	 */
	void unifyVariables(const cad::Variables& v) {
		this->variables.assign(v.begin(), v.end());
	}
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const Constraint<Num>& s);

	bool operator==(const Constraint<Number>& c) const {
		if (this->polynomial != c.polynomial) return false;
		if (this->sign != c.sign) return false;
		if (this->negated != c.negated) return false;
		if (this->variables.size() != c.variables.size()) return false;
		for (unsigned i = 0; i < this->variables.size(); i++) {
			if (this->variables[i] != c.variables[i]) return false;
		}
		return true;
	}
	bool operator<(const Constraint<Number>& c) const {
		if (this->sign != c.sign) return this->sign < c.sign;
		if (this->negated != c.negated) return this->negated < c.negated;
		if (this->variables != c.variables) return this->variables < c.variables;
		if (this->polynomial != c.polynomial) return this->polynomial < c.polynomial;
		return false;
	}
private:
	
	/**
	 * Verifies whether this constraint is constructed over no more than the given variables.
	 * The check is performed by an assert.
	 * @param p polynomial
	 * @param v list of variables
	 * @return v if the check was successful
	 */
	const std::vector<Variable> checkVariables(const cad::MultiPolynomial<Number>& p, const std::vector<Variable>& v) const {
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

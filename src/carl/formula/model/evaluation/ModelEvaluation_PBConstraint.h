#pragma once

#include "../Model.h"
#include "../../pseudoboolean/PBConstraint.h"

namespace carl {
namespace model {

	/**
	 * Substitutes all variables from a model within a constraint.
	 * May fail to substitute some variables, for example if the values are RANs or SqrtEx.
	 */
	template<typename Rational, typename Poly>
	void substituteIn(PBConstraint<Poly>& c, const Model<Rational,Poly>& m) {
	    std::vector<std::pair<Rational,Variable>> newLHS;
	    for(auto it = c.getLHS().begin(); it != c.getLHS().end(); it++){
			auto element = m.find(it->second);
	        if(element != m.end()) {
				assert(element->second.isRational());
				const Rational& r = element->second.asRational();
				assert(isZero(r) || isOne(r));
	            if (isOne(r)){
	                c.setRHS(c.getRHS() - it->first);
	            }
	        } else {
	            newLHS.push_back(*it);
	        }
	    }
	    c.setLHS(newLHS);
	}
	
	/**
	 * Evaluates a constraint to a ModelValue over a Model.
	 * If evaluation can not be done for some variables, the result may actually be a Constraint again.
	 */
	template<typename Rational, typename Poly>
	void evaluate(ModelValue<Rational,Poly>& res, PBConstraint<Poly>& c, const Model<Rational,Poly>& m) {
		substituteIn(c, m);
		res = carl::evaluate(-c.getRHS(), c.getRelation());
	}
}
}

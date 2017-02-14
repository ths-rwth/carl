/* 
 * File:   GroebnerBase.tpp
 * Author: tobias
 *
 * Created on 8. August 2016, 16:15
 */

#pragma once

namespace carl {
        
             
template<typename Number>
bool GroebnerBase<Number>::hasFiniteMon() const {
        std::set<Variable> vars = this->gatherVariables();
        std::vector<typename GroebnerBase<Number>::Monomial> lmons = this->cor();
		for (const auto& v: vars) {
			bool found = true;
			for (const auto& m : lmons) {
				assert(isOne(m.coeff()));
				if (m.hasNoOtherVariable(v)) {
					found = false;
					break;
				}
			}
			if (found) return false;
		}
		return true;
}
        
        
template<typename Number>
std::vector<typename GroebnerBase<Number>::Monomial> GroebnerBase<Number>::cor() const {
        std::vector<typename GroebnerBase<Number>::Monomial> res;
        for(const auto& p : this->get()) {
                res.push_back(GroebnerBase<Number>::Monomial(p.lmon()));
        }
        return res;
}

// general helper function
// returns true iff any of the monomials in list divides m
template<typename Number>
bool anyDivides(const std::vector<typename GroebnerBase<Number>::Monomial>& list, const typename GroebnerBase<Number>::Monomial& m) {
        CARL_LOG_ASSERT("carl.thom.groebner", isOne(m.coeff()), "invalid monomial!");
        for(const auto& m_prime : list) {
                CARL_LOG_ASSERT("carl.thom.groebner", isOne(m_prime.coeff()), "invalid monomial!");
                if(m.divisible(m_prime)) {
                        return true;
                }
        }
        return false;
}

}

// this is a helper function for mon
template<typename Number> // i dont know why this has to be a template but i get a linker error if its not
bool nextTuple(std::vector<carl::uint>& tuple, const std::vector<carl::uint>& maxTuple) {
        assert(tuple.size() == maxTuple.size());
        if(tuple[0] < maxTuple[0]) {
                tuple[0]++;
                return true;
        }
        else {
                assert(tuple[0] == maxTuple[0]);
                bool succes = false;
                for(carl::uint i = 1; i < tuple.size(); i++) {
                        if(tuple[i] < maxTuple[i]) {
                                for(carl::uint j = 0; j < i; j++) tuple[j] = 0;
                                tuple[i]++;
                                succes = true;
                                break;
                        }
                }
                return succes;
        }
}

namespace carl {

template<typename Number>
std::vector<typename GroebnerBase<Number>::Monomial> GroebnerBase<Number>::mon() const {
        CARL_LOG_FUNC("carl.thom.groebner", "groebner base: " << this->get());
        CARL_LOG_ASSERT("carl.thom.groebner", this->hasFiniteMon(), "tried to compute mon of non-zerodimensional system"); 
        std::set<Variable> varsset = this->gatherVariables();
        std::vector<Variable> vars;
        for(const auto& v : varsset) vars.push_back(v);
        std::vector<typename GroebnerBase<Number>::Monomial> lmons = this->cor();
        std::vector<uint> degrees;
        for(const auto& v : vars) {
                for(const auto& m : lmons) {
                        CARL_LOG_ASSERT("carl.thom.groebner", isOne(m.coeff()), "invalid monomial!");
                        if(m.hasNoOtherVariable(v)) {
                                degrees.push_back(m.monomial()->tdeg());
                                break;
                        }
                }
        }
        CARL_LOG_ASSERT("carl.thom.groebner", degrees.size() == vars.size(), "");
        CARL_LOG_TRACE("carl.thom.groebner", "variables: " << vars);
        CARL_LOG_TRACE("carl.thom.groebner", "vector of degrees: " << degrees);
        
        std::vector<typename GroebnerBase<Number>::Monomial> res;
        std::vector<uint> currTuple(degrees.size(), 0);
        do {
                typename GroebnerBase<Number>::Monomial candidate(Number(1));
                for(uint i = 0; i < currTuple.size(); i++) {
                        for(uint d = 0; d < currTuple[i]; d++) {
                                candidate = candidate * vars[i];
                        }
                }
                if(!anyDivides<Number>(lmons, candidate)) {
                        res.push_back(candidate);
                }
        }
        while(::nextTuple<Number>(currTuple, degrees));
        CARL_LOG_TRACE("carl.thom.groebner", "result: " << res);
        return res;
}

template<typename Number>
std::vector<typename GroebnerBase<Number>::Monomial> GroebnerBase<Number>::bor() const {
        std::vector<typename GroebnerBase<Number>::Monomial> res;
        std::vector<typename GroebnerBase<Number>::Monomial> lmons = this->cor();
        std::set<Variable> vars = this->gatherVariables();
        std::vector<typename GroebnerBase<Number>::Monomial> _mon = this->mon();
        for(const Variable& v : vars) {
                for(const auto& m : _mon) {
                        assert(isOne(m.coeff()));
                        typename GroebnerBase<Number>::Monomial currMon = m * v;
                        if(std::find(res.begin(), res.end(), currMon) == res.end()) { // not already contained in res
                                if(std::find(_mon.begin(), _mon.end(), currMon) == _mon.end()) { // not contained in _mon
                                        res.push_back(currMon);
                                }
                        }
                }
        }
        return res;
}

template<typename Number>
std::set<Variable> GroebnerBase<Number>::gatherVariables() const {
        std::set<Variable> vars;
        for(const auto& p : this->get()) {
                p.gatherVariables(vars);
        }
        return vars;
}

        
} // namespace carl

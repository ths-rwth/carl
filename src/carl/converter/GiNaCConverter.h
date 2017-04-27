/**
 * @file GiNaCConverter.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "carl/numbers/numbers.h"
#include "ginac.h"
#ifdef USE_GINAC

namespace carl {

class GiNaCConverter {
private:
	std::map<carl::Variable, GiNaC::symbol> vars;
public:
	GiNaC::numeric operator()(const cln::cl_RA& n) {
		return GiNaC::numeric(n);
	}
	GiNaC::numeric operator()(const mpq_class& n) {
		std::stringstream ss;
		ss << n;
		return GiNaC::numeric(ss.str().c_str());
	}
	GiNaC::symbol operator()(const carl::Variable& v) {
		auto it = vars.find(v);
		if (it == vars.end()) {
			it = vars.insert(std::make_pair(v, GiNaC::symbol(carl::VariablePool::getInstance().getName(v)))).first;
		}
		return it->second;
	}
	GiNaC::ex operator()(const std::pair<carl::Variable, carl::exponent>& p) {
		return GiNaC::pow((*this)(p.first), p.second);
	}
	GiNaC::ex operator()(const carl::Monomial& m) {
		GiNaC::ex res = 1;
		for (auto it: m) res *= (*this)(it);
		return res;
	}
	template<typename Coeff>
	GiNaC::ex operator()(const carl::Term<Coeff>& t) {
		GiNaC::ex res = (*this)(t.coeff());
		if (t.monomial()) return res * (*this)(*t.monomial());
		else return res;
	}
	template<typename Coeff>
	GiNaC::ex operator()(const carl::MultivariatePolynomial<Coeff>& p) {
		GiNaC::ex res;
		for (auto t: p) res += (*this)(t);
		return GiNaC::expand(res);
	}
	template<typename Coeff>
	GiNaC::ex operator()(const carl::UnivariatePolynomial<Coeff>& p) {
		GiNaC::ex res;
		unsigned exp = 0;
		GiNaC::symbol mainvar = (*this)(p.mainVar());
		for (auto c: p.coefficients()) {
			res += GiNaC::pow(mainvar, exp) * (*this)(c);
			exp++;
		}
		return GiNaC::expand(res);
	}
        /**
         * Compute all carl variables mapped in this converter whose GiNaC variables are also in the given variable vector.
         * The returned vector respects the order of the given variables.
         * @param cadVariables
         * @return carl variables corresponding to given GiNaC variables respecting the given order
         */
        std::vector<carl::Variable> variables(const std::vector<GiNaC::symbol>& cadVariables) {
            std::vector<carl::Variable> v = std::vector<carl::Variable>();
            for(std::vector<GiNaC::symbol>::const_iterator g_it = cadVariables.begin(); g_it != cadVariables.end(); ++g_it)
            {
                for(std::map<carl::Variable, GiNaC::symbol>::const_iterator it = vars.begin(); it != vars.end(); ++it)
                {
                    if(*g_it == it->second)
                        v.push_back(it->first);
                }
            }
            assert(cadVariables.size() == v.size());
            return v;
	}
};

}

#endif

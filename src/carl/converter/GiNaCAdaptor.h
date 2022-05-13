/**
 * @file GiNaCAdaptor.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {

class GiNaCConversion {
public:
	static std::map<carl::Variable, GiNaC::symbol> vars;
};

class ToGiNaC {
public:
	typedef GiNaC::numeric Number;
	typedef GiNaC::symbol Variable;
	typedef GiNaC::ex VariablePower;
	typedef GiNaC::ex Monomial;
	typedef GiNaC::ex Term;
	typedef GiNaC::ex MPolynomial;
	typedef GiNaC::ex UPolynomial;
	
	Number operator()(const cln::cl_RA& n) {
		return GiNaC::numeric(n);
	}
	Number operator()(const mpq_class& n) {
		std::stringstream ss;
		ss << n;
		return GiNaC::numeric(ss.str().c_str());
	}
	Variable operator()(carl::Variable::Arg v) {
		auto it = GiNaCConversion::vars.find(v);
		if (it == GiNaCConversion::vars.end()) {
			it = GiNaCConversion::vars.insert(std::make_pair(v, GiNaC::symbol(carl::VariablePool::getInstance().get_name(v)))).first;
		}
		return it->second;
	}
	VariablePower operator()(GiNaC::symbol v, const carl::exponent& exp) {
		return GiNaC::pow(v, exp);
	}
	Monomial operator()(const std::vector<GiNaC::ex>& vp) {
		return std::accumulate(vp.begin(), vp.end(), GiNaC::ex(1), std::multiplies<GiNaC::ex>());
	}
	template<typename Coeff>
	Term operator()(const GiNaC::numeric& n, const GiNaC::ex& mon) {
		return n * mon;
	}
	template<typename Coeff>
	MPolynomial operator()(const std::vector<GiNaC::ex>& terms) {
		return std::accumulate(terms.begin(), terms.end(), GiNaC::ex(0), std::plus<GiNaC::ex>());
	}
};

template<typename C>
class FromGiNaC {
public:
	typedef C Number;
};

}
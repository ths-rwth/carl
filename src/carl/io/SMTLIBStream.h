#pragma once

#include "../core/Monomial.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/Relation.h"
#include "../core/Term.h"
#include "../core/UnivariatePolynomial.h"
#include "../core/Variable.h"
#include "../formula/Constraint.h"
#include "../formula/Formula.h"
#include "../formula/Logic.h"
#include "../formula/model/Model.h"
#include "../formula/Sort.h"
#include "../numbers/numbers.h"
#include "../util/tuple_util.h"

#include <iostream>
#include <sstream>
#include <type_traits>

namespace carl {

class SMTLIBStream {
private:
	std::stringstream mStream;

	void write(const mpz_class& n) { *this << carl::toString(n, false); }
	void write(const mpq_class& n) { *this << carl::toString(n, false); }
#ifdef USE_CLN_NUMBERS
	void write(const cln::cl_I& n) { *this << carl::toString(n, false); }
	void write(const cln::cl_RA& n) { *this << carl::toString(n, false); }
#endif

	template<typename Pol>
	void write(const Constraint<Pol>& c) {
		if (c.relation() == Relation::NEQ) {
			*this << "(not (= " << c.lhs() << " 0))";
		} else {
			*this << "(" << c.relation() << " " << c.lhs() << " 0)";
		}
	}
	
	template<typename Pol>
	void write(const Formula<Pol>& f) {
		switch (f.getType()) {
			case FormulaType::AND:
			case FormulaType::OR:
			case FormulaType::IFF:
			case FormulaType::XOR:
			case FormulaType::IMPLIES:
			case FormulaType::ITE:
			{
				*this << "(" << f.getType();
				for (const auto& cur: f.subformulas()) {
					*this << " " << cur;
				}
				*this << ")";
				break;
			}
			case FormulaType::NOT:
				*this << "(" << f.getType() << " " << f.subformula() << ")";
				break;
			case FormulaType::BOOL:
				*this << f.boolean();
				break;
			case FormulaType::CONSTRAINT:
				*this << f.constraint();
				break;
			case FormulaType::VARCOMPARE:
				*this << f.variableComparison();
				break;
			case FormulaType::VARASSIGN:
				*this << f.variableAssignment();
				break;
			case FormulaType::BITVECTOR:
				*this << f.bvConstraint();
				break;
			case FormulaType::TRUE:
			case FormulaType::FALSE:
				*this << f.getType();
				break;
			case FormulaType::UEQ:
				*this << f.uequality();
				break;
			case FormulaType::EXISTS:
			case FormulaType::FORALL:
				CARL_LOG_ERROR("carl.smtlibstream", "Printing exists or forall is not implemented yet.");
				break;
		}
	}
	
	template<typename Rational, typename Poly>
	void write(const Model<Rational,Poly>& model) {
		*this << "(model" << std::endl;
		for (const auto& m: model) {
			auto value = m.second;
			value = model.evaluated(m.first);
			*this << "\t(define-fun " << m.first << " () ";
			if (m.first.isVariable()) {
				*this << m.first.asVariable().getType() << std::endl;
			} else if (m.first.isBVVariable()) {
				*this << m.first.asBVVariable().sort() << std::endl;
			} else if (m.first.isUVariable()) {
				*this << m.first.asUVariable().domain() << std::endl;
			} else if (m.first.isFunction()) {
				*this << value;
			} else {
				CARL_LOG_ERROR("carl.smtlibstream", "Encountered an unknown type of ModelVariable: " << m.first);
				assert(false);
			}
			*this << "\t\t";
			value.visit([this](const auto& v){ this->write(v); });
			*this << std::endl << "\t)" << std::endl;
		}
		*this << ")" << std::endl;
	}
	
	template<typename Rational, typename Poly>
	void write(const ModelValue<Rational,Poly>& mv) {
		if (mv.isBool()) {
			*this << mv.asBool();
		} else if (mv.isRational()) {
			*this << mv.asRational();
		}
	}

	void write(const Monomial::Arg& m) {
		if (m) *this << *m;
		else *this << "1";
	}
	void write(const Monomial::Content::value_type& m) {
		if (m.second == 0) *this << "1";
		else if (m.second == 1) *this << m.first;
		else {
			*this << "(*";
			for (std::size_t i = 0; i < m.second; i++) *this << " " << m.first;
			*this << ")";
		}
	}
	void write(const Monomial& m) {
		if (m.exponents().empty()) *this << "1";
		else if (m.exponents().size() == 1) *this << m.exponents().front();
		else {
			*this << "(*";
			for (const auto& e: m.exponents()) *this << " " << e;
			*this << ")";
		}
	}
	
	template<typename Coeff>
	void write(const MultivariatePolynomial<Coeff>& mp) {
		if (mp.isZero()) *this << "0";
		else if (mp.nrTerms() == 1) *this << mp.lterm();
		else {
			*this << "(+";
			for (auto it = mp.rbegin(); it != mp.rend(); it++) {
				*this << " " << *it;
			}
			*this << ")";
		}
	}
	
	void write(Relation r) {
		switch (r) {
			case Relation::EQ:		*this << "="; break;
			case Relation::NEQ:		*this << "<>"; break;
			case Relation::LESS:	*this << "<"; break;
			case Relation::LEQ:		*this << "<="; break;
			case Relation::GREATER:	*this << ">"; break;
			case Relation::GEQ:		*this << ">="; break;
		}
	}

	template<typename Coeff>
	void write(const Term<Coeff>& t) {
		if (!t.monomial()) *this << t.coeff();
		else {
			if (carl::isOne(t.coeff())) {
				*this << t.monomial();
			} else {
				*this << "(* " << t.coeff() << " " << t.monomial() << ")";
			}
		}
	}
	
	template<typename Coeff>
	void write(const UnivariatePolynomial<Coeff>& up) {
		if (up.isConstant()) *this << up.constantPart();
		else {
			*this << "(+";
			for (std::size_t i = 0; i < up.coefficients().size(); i++) {
				std::size_t exp = up.coefficients().size() - i - 1;
				const auto& coeff = up.coefficients()[exp];
				if (exp == 0) *this << " " << coeff;
				else *this << " (* " << coeff << " " << Monomial(up.mainVar(), exp) << ")";
			}
			*this << ")";
		}
	}

	void write(const Variable& v) {
		*this << v.name();
	}
	void write(const VariableType& vt) {
		switch (vt) {
			case VariableType::VT_BOOL:				*this << "Bool"; break;
			case VariableType::VT_REAL:				*this << "Real"; break;
			case VariableType::VT_INT:				*this << "Int"; break;
			case VariableType::VT_UNINTERPRETED:	*this << "?_Uninterpreted"; break;
			case VariableType::VT_BITVECTOR:		*this << "?_Bitvector"; break;
			default:								*this << "?"; break;
		}
	}
	
	template<typename T>
	void write(T&& t) {
		mStream << t;
	}
	
public:
	void declare(Logic l) {
		*this << "(set-logic " << l << ")" << std::endl;
	}
	void declare(Sort s) {
		*this << "(declare-sort " << s << " " << s.arity() << ")" << std::endl;
	}
	void declare(Variable v) {
		*this << "(declare-fun " << v << " () " << v.type() << ")" << std::endl;
	}
	void declare(const Variables& vars) {
		for (auto v: vars) declare(v);
	}
	void initialize(Logic l, const Variables& vars) {
		declare(l);
		declare(vars);
	}
	
	template<typename Pol>
	void initialize(Logic l, std::initializer_list<Formula<Pol>> formulas) {
		Variables vars;
		for (const auto& f: formulas) {
			f.collectVariables(vars, true, true, true, true, true);
		}
		initialize(l, vars);
	}

	void setInfo(const std::string& name, const std::string& value) {
		*this << "(set-info :" << name << " " << value << ")" << std::endl;
	}
	
	template<typename Pol>
	void assertFormula(const Formula<Pol>& formula) {
		*this << "(assert " << formula << ")" << std::endl;
	}
	
	template<typename Pol>
	void minimize(const Pol& objective) {
		*this << "(minimize " << objective << ")" << std::endl;
	}
	
	void checkSat() {
		*this << "(check-sat)" << std::endl;
	}
	
	void getAssertions() {
		*this << "(get-assertions)" << std::endl;
	}

	void getModel() {
		*this << "(get-model)" << std::endl;
	}
	
	template<typename T>
	SMTLIBStream& operator<<(T&& t) {
		write(static_cast<const std::decay_t<T>&>(t));
		return *this;
	}
	
	SMTLIBStream& operator<<(std::ostream& (*os)(std::ostream&)) {
		write(os);
		return *this;
	}

	auto str() const {
		return mStream.str();
	}
	
	auto content() const {
		return mStream.rdbuf();
	}
};

inline std::ostream& operator<<(std::ostream& os, const SMTLIBStream& ss) {
	return os << ss.content();
}

namespace detail {

template<typename Pol>
struct SMTLIBScriptContainer {
	Logic mLogic;
	std::initializer_list<Formula<Pol>> mFormulas;
	bool mGetModel;
	Pol mObjective;
	SMTLIBScriptContainer(Logic l, std::initializer_list<Formula<Pol>> f, bool getModel = false): mLogic(l), mFormulas(f), mGetModel(getModel) {}
	SMTLIBScriptContainer(Logic l, std::initializer_list<Formula<Pol>> f, const Pol& objective, bool getModel = false): mLogic(l), mFormulas(f), mGetModel(getModel), mObjective(objective) {}
};
template<typename Pol>
std::ostream& operator<<(std::ostream& os, const SMTLIBScriptContainer<Pol>& sc) {
	SMTLIBStream sls;
	sls.initialize(sc.mLogic, sc.mFormulas);
	for (const auto& f: sc.mFormulas) sls.assertFormula(f);
	if (!sc.mObjective.isZero()) sls.minimize(sc.mObjective);
	sls.checkSat();
	if (sc.mGetModel) sls.getModel();
	return os << sls;
}

}

template<typename Pol, typename... Args>
detail::SMTLIBScriptContainer<Pol> outputSMTLIB(Logic l, std::initializer_list<Formula<Pol>> formulas, Args&&... args) {
	return detail::SMTLIBScriptContainer<Pol>(l, formulas, std::forward<Args>(args)...);
}

namespace detail {
	template<typename... Args>
	struct SMTLIBOutputContainer {
		std::tuple<Args...> mData;
		SMTLIBOutputContainer(Args&&... args): mData(std::forward<Args>(args)...) {}
	};
	template<typename... Args>
	std::ostream& operator<<(std::ostream& os, const SMTLIBOutputContainer<Args...>& soc) {
		SMTLIBStream sls;
		carl::tuple_accumulate(soc.mData, sls, [](auto& sls, const auto& t) -> auto& { return sls << t; });
		return os << sls;
	}
}

template<typename... Args>
detail::SMTLIBOutputContainer<Args...> asSMTLIB(Args&&... args) {
	return detail::SMTLIBOutputContainer<Args...>(std::forward<Args>(args)...);
}

}

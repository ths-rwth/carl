#pragma once

#include <carl-arith/poly/umvpoly/Monomial.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/core/Relation.h>
#include <carl-arith/poly/umvpoly/Term.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/core/Variable.h>
#include <carl-formula/arithmetic/Constraint.h>
#include <carl-arith/extended/VariableComparison.h>
#include <carl-formula/formula/Formula.h>
#include <carl-formula/formula/Logic.h>
#include <carl-formula/model/Model.h>
#include <carl-formula/sort/Sort.h>
#include <carl-arith/numbers/numbers.h>
#include <carl-common/util/tuple_util.h>
#include <carl-common/util/streamingOperators.h>

#include <iostream>
#include <sstream>
#include <type_traits>

namespace carl::io {

/**
 * Allows to print carl data structures in SMTLIB syntax.
 */
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
	void write(const VariableComparison<Pol>& c) {
		auto constraint = carl::as_constraint(c);
		if (constraint) {
			*this << *constraint;
		} else {
			auto iroot = [&]() {
				if (std::holds_alternative<MultivariateRoot<Pol>>(c.value())) {
					return std::get<MultivariateRoot<Pol>>(c.value());
				} else {
					return convert_to_mvroot<Pol>(std::get<typename MultivariateRoot<Pol>::RAN>(c.value()), c.var());
				}
			}();
			*this << "(" << c.relation() << " " << (c.negated() ? "true" : "false") << " " << c.var() << " (root " << iroot.poly() << " " << iroot.k() << " " << iroot.var() << "))";
		}
	}

	template<typename Pol>
	void write(const Formula<Pol>& f) {
		switch (f.type()) {
			case FormulaType::AND:
			case FormulaType::OR:
			case FormulaType::IFF:
			case FormulaType::XOR:
			case FormulaType::IMPLIES:
			case FormulaType::ITE:
				// 	*this << "(" << f.type() << " " << stream_joined(" ", f.subformulas()) << ")";
				*this << "(" << f.type();
				for (const auto& f : f.subformulas()) {
					*this << " " << f;
				}
				*this << ")";
				break;
			case FormulaType::NOT:
				*this << "(" << f.type() << " " << f.subformula() << ")";
				break;
			case FormulaType::BOOL:
				*this << f.boolean();
				break;
			case FormulaType::CONSTRAINT:
				*this << f.constraint();
				break;
			case FormulaType::VARCOMPARE:
				*this << f.variable_comparison();
				break;
			case FormulaType::VARASSIGN:
				*this << f.variable_assignment();
				break;
			case FormulaType::BITVECTOR:
				*this << f.bv_constraint();
				break;
			case FormulaType::TRUE:
			case FormulaType::FALSE:
				*this << f.type();
				break;
			case FormulaType::UEQ:
				*this << f.u_equality();
				break;
			case FormulaType::EXISTS:
			case FormulaType::FORALL:
				CARL_LOG_ERROR("carl.smtlibstream", "Printing exists or forall is not implemented yet.");
				break;
			default:
				CARL_LOG_ERROR("carl.smtlibstream", "Not supported formula type: " << f.type());
		}
	}
	
	template<typename Rational, typename Poly>
	void write(const Model<Rational,Poly>& model) {
		*this << "(model" << std::endl;
		for (const auto& m: model) {
			auto value = m.second;
			value = model.evaluated(m.first);
			*this << "\t(define-fun " << m.first << " () ";
			if (m.first.is_variable()) {
				*this << m.first.asVariable().type() << std::endl;
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
			for (std::size_t i = 0; i < m.second; ++i) *this << " " << m.first;
			*this << ")";
		}
	}
	void write(const Monomial& m) {
		if (m.exponents().empty()) *this << "1";
		else if (m.exponents().size() == 1) *this << m.exponents().front();
		else {
			// *this << "(* " << stream_joined(" ", m.exponents()) << ")";
			*this << "(* "; 
			for (const auto& e : m.exponents()) {
				*this << " " << e; 
			}
			*this << ")";
		}
	}
	
	template<typename Coeff>
	void write(const MultivariatePolynomial<Coeff>& mp) {
		if (is_zero(mp)) *this << "0";
		else if (mp.nr_terms() == 1) *this << mp.lterm();
		else {
			*this << "(+";
			for (auto it = mp.rbegin(); it != mp.rend(); ++it) {
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
			if (carl::is_one(t.coeff())) {
				*this << t.monomial();
			} else {
				*this << "(* " << t.coeff() << " " << t.monomial() << ")";
			}
		}
	}

	void write(const UEquality& ueq) {
		if (ueq.negated()) {
			*this << "(not (= " << ueq.lhs() << " " << ueq.rhs() << "))";
		} else {
			*this << "(= " << ueq.lhs() << " " << ueq.rhs() << ")";
		}
	}

	void write(const UFInstance& ufi) {
		// *this << "(" << ufi.uninterpretedFunction().name() << " " << stream_joined(" ", ufi.args()) << ")";
		*this << "(" << ufi.uninterpretedFunction().name();
		for (const auto& a : ufi.args()) {
			*this << " " << a;
		}
		*this << ")";
	}
	
	template<typename Coeff>
	void write(const UnivariatePolynomial<Coeff>& up) {
		if (up.is_constant()) *this << up.constant_part();
		else {
			*this << "(+";
			for (std::size_t i = 0; i < up.coefficients().size(); ++i) {
				std::size_t exp = up.coefficients().size() - i - 1;
				const auto& coeff = up.coefficients()[exp];
				if (exp == 0) *this << " " << coeff;
				else *this << " (* " << coeff << " " << Monomial(up.main_var(), exp) << ")";
			}
			*this << ")";
		}
	}

	void write(const UTerm& t) {
		std::visit(overloaded {
			[this](UVariable v) { *this << v; },
			[this](UFInstance ufi) { *this << ufi; },
		}, t.asVariant());
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
	/// Writes a comment
	void comment(const std::string& c) {
		*this << "; " << c << std::endl;
	}
	/// Declare a logic via `set-logic`.
	void declare(Logic l) {
		*this << "(set-logic " << l << ")" << std::endl;
	}
	/// Declare a sort via `declare-sort`.
	void declare(Sort s) {
		*this << "(declare-sort " << s << " " << s.arity() << ")" << std::endl;
	}
	/// Declare a fresh function via `declare-fun`.
	void declare(UninterpretedFunction uf) {
		// *this << "(declare-fun " << uf.name() << " (" << stream_joined(" ", uf.domain()) << ") ";
		*this << "(declare-fun " << uf.name() << " (";
		for (const auto& d : uf.domain()) {
			*this << " " << d;
		}
		*this  << ") ";
		*this << uf.codomain() << ")" << std::endl;
	}
	/// Declare a fresh variable via `declare-fun`.
	void declare(Variable v) {
		*this << "(declare-fun " << v << " () " << v.type() << ")" << std::endl;
	}
	/// Declare a bitvector variable via `declare-fun`.
	void declare(BVVariable v) {
		*this << "(declare-fun " << v.variable() << " () " << v.variable().type() << ")" << std::endl;
	}
	/// Declare an uninterpreted variable via `declare-fun`.
	void declare(UVariable v) {
		*this << "(declare-fun " << v << " () " << v.domain() << ")" << std::endl;
	}
	/// Declare a set of functions.
	void declare(const std::set<UninterpretedFunction>& ufs) {
		for (const auto& uf: ufs) {
			declare(uf);
		}
	}
	/// Declare a set of variables.
	void declare(const carlVariables& vars) {
		for (const auto& v: vars) {
			declare(v);
		}
	}
	/// Declare a set of bitvector variables.
	void declare(const std::set<BVVariable>& bvvs) {
		for (const auto& bv: bvvs) {
			declare(bv);
		}
	}
	/// Declare a set of uninterpreted variables.
	void declare(const std::set<UVariable>& uvs) {
		for (const auto& uv: uvs) {
			declare(uv);
		}
	}
	/// Generic initializer including the logic, a set of variables and a set of functions.
	void initialize(Logic l, const carlVariables& vars, const std::set<UninterpretedFunction>& ufs = {}, const std::set<BVVariable>& bvvs = {}, const std::set<UVariable>& uvs = {}) {
		declare(l);
		std::set<Sort> sorts;
		for (const auto& v: bvvs) {
			sorts.insert(v.sort());
		}
		for (const auto& v: uvs) {
			sorts.insert(v.domain());
		}
		for (const auto& s: sorts) {
			declare(s);
		}
		declare(ufs);
		declare(vars.filter(carl::variable_type_filter::excluding({VariableType::VT_BITVECTOR, VariableType::VT_UNINTERPRETED})));
		declare(bvvs);
		declare(uvs);
	}
	
	/// Generic initializer including the logic and variables and functions from a set of formulas.
	template<typename Pol>
	void initialize(Logic l, std::initializer_list<Formula<Pol>> formulas) {
		carlVariables vars;
		std::set<UninterpretedFunction> ufs;
		std::set<BVVariable> bvvs;
		std::set<UVariable> uvs;
		for (const auto& f: formulas) {
			carl::variables(f,vars);
			carl::uninterpreted_functions(f,ufs);
			carl::bitvector_variables(f,bvvs);
			carl::uninterpreted_variables(f,uvs);
		}
		initialize(l, vars, ufs, bvvs, uvs);
	}

	/// Set information via `set-info`.
	void setInfo(const std::string& name, const std::string& value) {
		*this << "(set-info :" << name << " " << value << ")" << std::endl;
	}

	/// Set option via `set-option`.
	void setOption(const std::string& name, const std::string& value) {
		*this << "(set-option :" << name << " " << value << ")" << std::endl;
	}
	
	/// Assert a formula via `assert`.
	template<typename Pol>
	void assertFormula(const Formula<Pol>& formula) {
		*this << "(assert " << formula << ")" << std::endl;
	}
	
	/// Minimize an objective via custom `minimize`.
	template<typename Pol>
	void minimize(const Pol& objective) {
		*this << "(minimize " << objective << ")" << std::endl;
	}
	
	/// Check satisfiability via `check-sat`.
	void checkSat() {
		*this << "(check-sat)" << std::endl;
	}
	
	/// Print assertions via `get-assertions`.
	void getAssertions() {
		*this << "(get-assertions)" << std::endl;
	}

	/// Print model via `get-model`.
	void getModel() {
		*this << "(get-model)" << std::endl;
	}

	/// Echo via `echo`.
	void echo(const std::string& str) {
		*this << "(echo \"" << str << "\")" << std::endl;
	}

	/// Reset via `reset`.
	void reset() {
		*this << "(reset)" << std::endl;
	}

	/// Exit via `exit`.
	void exit() {
		*this << "(exit)" << std::endl;
	}
	
	/// Write some data to this stream.
	template<typename T>
	SMTLIBStream& operator<<(T&& t) {
		write(static_cast<const std::decay_t<T>&>(t));
		return *this;
	}
	
	/// Write io operators (like `std::endl`) directly to the underlying stream.
	SMTLIBStream& operator<<(std::ostream& (*os)(std::ostream&)) {
		write(os);
		return *this;
	}

	/// Return the written data as a string.
	auto str() const {
		return mStream.str();
	}
	
	/// Return the underlying stream buffer.
	auto content() const {
		return mStream.rdbuf();
	}
};

/// Write the written data to some `std::ostream`.
inline std::ostream& operator<<(std::ostream& os, const SMTLIBStream& ss) {
	return os << ss.content();
}

namespace detail {

/// Shorthand to allow writing SMTLIB scripts in one line.
template<typename Pol>
struct SMTLIBScriptContainer {
	Logic mLogic;
	std::initializer_list<Formula<Pol>> mFormulas;
	bool mGetModel;
	Pol mObjective;
	SMTLIBScriptContainer(Logic l, std::initializer_list<Formula<Pol>> f, bool getModel = false): mLogic(l), mFormulas(f), mGetModel(getModel) {}
	SMTLIBScriptContainer(Logic l, std::initializer_list<Formula<Pol>> f, const Pol& objective, bool getModel = false): mLogic(l), mFormulas(f), mGetModel(getModel), mObjective(objective) {}
};
/// Actually write an SMTLIBScriptContainer to an std::ostream.
template<typename Pol>
std::ostream& operator<<(std::ostream& os, const SMTLIBScriptContainer<Pol>& sc) {
	SMTLIBStream sls;
	sls.initialize(sc.mLogic, sc.mFormulas);
	for (const auto& f: sc.mFormulas) sls.assertFormula(f);
	if (!is_zero(sc.mObjective)) sls.minimize(sc.mObjective);
	sls.checkSat();
	if (sc.mGetModel) sls.getModel();
	return os << sls;
}

}

/// Shorthand to allow writing SMTLIB scripts in one line.
template<typename Pol, typename... Args>
detail::SMTLIBScriptContainer<Pol> outputSMTLIB(Logic l, std::initializer_list<Formula<Pol>> formulas, Args&&... args) {
	return detail::SMTLIBScriptContainer<Pol>(l, formulas, std::forward<Args>(args)...);
}

namespace detail {
	template<typename... Args>
	struct SMTLIBOutputContainer {
		std::tuple<Args...> mData;
		explicit SMTLIBOutputContainer(Args&&... args): mData(std::forward<Args>(args)...) {}
	};
	template<typename... Args>
	std::ostream& operator<<(std::ostream& os, const SMTLIBOutputContainer<Args...>& soc) {
		SMTLIBStream sls;
		carl::tuple_accumulate(soc.mData, sls, [](auto& sls, const auto& t) -> auto& { return sls << t; });
		return os << sls;
	}
}

/// Generic shorthand to write arbitrary data to an SMTLIBStream and return the result.
template<typename... Args>
detail::SMTLIBOutputContainer<Args...> asSMTLIB(Args&&... args) {
	return detail::SMTLIBOutputContainer<Args...>(std::forward<Args>(args)...);
}

}

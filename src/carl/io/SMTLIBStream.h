#pragma once

#include "../core/Monomial.h"
#include "../core/MultivariatePolynomial.h"
#include "../core/Relation.h"
#include "../core/Term.h"
#include "../core/UnivariatePolynomial.h"
#include "../core/Variable.h"
#include "../formula/Constraint.h"
#include "../formula/Sort.h"

#include <iostream>
#include <sstream>
#include <type_traits>

namespace carl {

class SMTLIBStream {
private:
	std::stringstream mStream;
	
	void declare(const Sort& s) {
		*this << "(declare-sort " << s << " " << s.arity() << ")" << std::endl;
	}
	void declare(const Variable& v) {
		*this << "(declare-fun " << v << " () " << v.getType() << ")" << std::endl;
	}

	template<typename Pol>
	void write(const Constraint<Pol>& c) {
		*this << "(" << c.relation() << " " << c.lhs() << " 0)";
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
			for (auto i = 0; i < m.second; i++) *this << " " << m.first;
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
		*this << v.getName();
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
	template<typename T>
	SMTLIBStream& operator<<(T&& t) {
		write(static_cast<const std::decay_t<T>&>(t));
		return *this;
	}
	//
	SMTLIBStream& operator<<(std::ostream& (*os)(std::ostream&)) {
		write(os);
		return *this;
	}
	
	const auto& content() const {
		return mStream.rdbuf();
	}
};

std::ostream& operator<<(std::ostream& os, const SMTLIBStream& ss) {
	return os << ss.content();
}

}

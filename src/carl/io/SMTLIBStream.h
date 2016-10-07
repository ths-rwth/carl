#pragma once

#include "../core/Monomial.h"
#include "../core/Relation.h"
#include "../core/Term.h"
#include "../core/Variable.h"

#include <iostream>
#include <type_traits>

namespace carl {

class SMTLIBStream: public std::ostream {

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

	void write(Variable::Arg v) {
		*this << v.getName();
	}
	
	template<typename T>
	void write(T&& t) {
		static_cast<std::ostream&>(*this) << t;
	}
	
public:
	SMTLIBStream(std::ostream& base) {
		rdbuf(base.rdbuf());
	}
	
	template<typename T>
	SMTLIBStream& operator<<(T&& t) {
		write(static_cast<const std::decay_t<T>&>(t));
		return *this;
	}
};


}

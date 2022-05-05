#pragma once

#include "VariableComparison.h"
#include <carl/core/Relation.h>
#include <carl/core/Variable.h>

namespace carl {
	template<typename Poly>
	class VariableAssignment {
	private:
		using Base = VariableComparison<Poly>;
	public:
		using Number = typename Base::Number;
		using MR = typename Base::MR;
		using RAN = typename Base::RAN;
	private:
		Base m_comparison;
	public:	
		VariableAssignment(Variable v, const RAN& value, bool negated = false): m_comparison(v, value, Relation::EQ, negated) {}
		VariableAssignment(Variable v, const Number& value, bool negated = false): m_comparison(v, RAN(value), Relation::EQ, negated) {}
		
		Variable var() const {
			return m_comparison.var();
		}
		const RAN& value() const {
			assert(std::holds_alternative<RAN>(m_comparison.value()));
			return std::get<RAN>(m_comparison.value());
		}
		const auto& base_value() const {
			return m_comparison.value();
		}
		bool negated() const {
			return m_comparison.negated();
		}
		VariableAssignment negation() const {
			return VariableAssignment(var(), value(), !negated());
		}
		operator const VariableComparison<Poly>&() const {
			return m_comparison;
		}

		template<typename Pol>
    	friend inline void variables(const VariableAssignment<Pol>& f, carlVariables& vars);
	};

	template<typename Pol>
    inline void variables(const VariableAssignment<Pol>& f, carlVariables& vars) {
		variables(f.m_comparison, vars);
	}
	
	template<typename Poly>
	bool operator==(const VariableAssignment<Poly>& lhs, const VariableAssignment<Poly>& rhs) {
		return static_cast<VariableComparison<Poly>>(lhs) == static_cast<VariableComparison<Poly>>(rhs);
	}
	template<typename Poly>
	bool operator<(const VariableAssignment<Poly>& lhs, const VariableAssignment<Poly>& rhs) {
		return static_cast<VariableComparison<Poly>>(lhs) < static_cast<VariableComparison<Poly>>(rhs);
	}
	template<typename Poly>
	std::ostream& operator<<(std::ostream& os, const VariableAssignment<Poly>& va) {
		return os << "(" << va.var() << (va.negated() ? " -!> " : " -> ") << va.base_value() << ")";
	}
}

namespace std {
	template<typename Pol>
	struct hash<carl::VariableAssignment<Pol>> {
		std::size_t operator()(const carl::VariableAssignment<Pol>& va) const {
			return std::hash<carl::VariableComparison<Pol>>()(va);
		}
	};
}

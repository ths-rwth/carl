namespace carl::vs {

template<typename Poly>
bool substitution_term<Poly>::operator==(const substitution_term<Poly>& other) const {
	return m_type == other.m_type && m_sqrt_ex == other.m_sqrt_ex;
}

template<typename Poly>
std::ostream& operator<<(std::ostream& os, const substitution_term<Poly>& s) {
    if (s.is_normal()) {
        os << s.sqrt_ex();
    } else if (s.is_plus_eps()) {
        os << s.sqrt_ex() << " + epsilon";
    } else if (s.is_minus_infty()) {
        os << "-infinity";
    } else {
        assert(s.is_plus_infty());
        os << "+infinity";
    }
	return os;
}

template<typename Poly>
substitution<Poly>::substitution(const Variable& _variable, const substitution_term<Poly>& _term, Constraints<Poly>&& _sideCondition)
	: m_variable(_variable),
	  m_term(_term),
	  m_side_condition(std::move(_sideCondition)) {
	if (m_term.is_plus_eps() && m_variable.type() == carl::VariableType::VT_INT) {
		m_term = substitution_term<Poly>::normal(m_term.sqrt_ex() + smtrat::SqrtEx(Poly(1)));
	}
}

template<typename Poly>
bool substitution<Poly>::operator==(const substitution<Poly>& other) const {
    return m_variable == other.m_variable && m_term == other.m_term && m_side_condition == other.m_side_condition;
}

template<typename Poly>
std::ostream& operator<<(std::ostream& os, const substitution<Poly>& s) {
	os << "[" << s.variable() << " -> " << s.term() << "]";
	return os;
}

} // namespace carl::vs
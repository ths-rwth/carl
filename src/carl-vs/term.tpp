namespace carl::vs {

template<typename Poly>
bool Term<Poly>::operator==(const Term<Poly>& other) const {
	return m_type == other.m_type && m_sqrt_ex == other.m_sqrt_ex;
}

template<typename Poly>
std::ostream& operator<<(std::ostream& os, const Term<Poly>& s) {
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

} // namespace carl::vs
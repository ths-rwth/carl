#pragma once

#include <carl-arith/vs/SqrtEx.h>

namespace carl::vs {

enum class TermType {
	NORMAL,
	PLUS_EPSILON,
	MINUS_INFINITY,
	PLUS_INFINITY
};

template<class Poly>
class Term {
private:                    
	/// The substitution type.
	TermType m_type;
	/// A square root expression.
	std::optional<SqrtEx<Poly>> m_sqrt_ex;

public:
	Term(TermType type, std::optional<SqrtEx<Poly>> sqrt_ex)
		: m_type(type), m_sqrt_ex(sqrt_ex) {}

	static Term normal(const SqrtEx<Poly>& sqrt_ex) {
		return Term(TermType::NORMAL, sqrt_ex);
	}

	static Term plus_eps(const SqrtEx<Poly>& sqrt_ex) {
		return Term(TermType::PLUS_EPSILON, sqrt_ex);
	}

	static Term minus_infty() {
		return Term(TermType::MINUS_INFINITY, std::nullopt);
	}

	static Term plus_infty() {
		return Term(TermType::PLUS_INFINITY, std::nullopt);
	}

	bool is_normal() const {
		return m_type == TermType::NORMAL;
	}

	bool is_plus_eps() const {
		return m_type == TermType::PLUS_EPSILON;
	}

	bool is_minus_infty() const {
		return m_type == TermType::MINUS_INFINITY;
	}

	bool is_plus_infty() const {
		return m_type == TermType::PLUS_INFINITY;
	}

	const SqrtEx<Poly> sqrt_ex() const {
		return *m_sqrt_ex;
	}

    TermType type() const {
        return m_type;
    } 

    bool operator==(const Term&) const;
};

template<class Poly>
std::ostream& operator<<(std::ostream& os, const Term<Poly>& s);

} // namespace carl::vs

namespace std {
template<class Poly>
struct hash<carl::vs::Term<Poly>> {
public:
	size_t operator()(const carl::vs::Term<Poly>& term) const {
		return ((hash<carl::SqrtEx<Poly>>()(term.sqrt_ex()) << 5) ^ term.type());
	}
};
} // namespace std

#include "term.tpp"
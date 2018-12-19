/**
 * @file UTerm.cpp
 * @author Rebecca Haehn <haehn@cs.rwth-aachen.de>
 * @since 2018-08-28
 * @version 2018-08-28
 */


#include "UTerm.h"

#include <iostream>
#include <sstream>

namespace carl
{
	Sort UTerm::domain() const {
		return std::visit(overloaded {
			[](UVariable var) { return var.domain(); },
			[](UFInstance ufi) { return ufi.uninterpretedFunction().codomain(); },
		}, mTerm);
	}

	std::size_t UTerm::complexity() const {
		return std::visit(overloaded {
			[](UVariable) { return static_cast<std::size_t>(1); },
			[](UFInstance ufi) { return ufi.complexity(); },
		}, mTerm);
	}

	void UTerm::gatherVariables(carlVariables& vars) const {
		return std::visit(overloaded {
			[&vars](UVariable v) { vars.add(v.variable()); },
			[&vars](UFInstance ufi) { ufi.gatherVariables(vars); },
		}, mTerm);
	}

	bool operator==(const UTerm& lhs, const UTerm& rhs) {
		return std::visit(overloaded {
			[](UVariable lhs, UVariable rhs) { return lhs == rhs; },
			[](UFInstance lhs, UFInstance rhs) { return lhs.id() == rhs.id(); },
			[](const auto&, const auto&) { return false; },
		}, lhs.asVariant(), rhs.asVariant());
	}

	bool operator!=(const UTerm& lhs, const UTerm& rhs) {
        return !(lhs == rhs);
    }

	bool operator<(const UTerm& lhs, const UTerm& rhs) {
		return std::visit(overloaded {
			[](UVariable lhs, UVariable rhs) { return lhs < rhs; },
			[](UFInstance lhs, UFInstance rhs) { return lhs.id() < rhs.id(); },
			[](UVariable, UFInstance) { return true; },
			[](const auto&, const auto&) { return false; },
		}, lhs.asVariant(), rhs.asVariant());
	}

	std::ostream& operator<<(std::ostream& os, const UTerm& ut) {
		return std::visit(overloaded {
			[&os](UVariable var) -> auto& { return os << var; },
			[&os](UFInstance ufi) -> auto& { return os << ufi; },
		}, ut.asVariant());
	}
}

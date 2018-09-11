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
	bool UTerm::isUVariable() const {
		return std::visit(overloaded {
			[](const UVariable&) { return true; },
			[](const UFInstance&) { return false; },
		}, mTerm);
	}
	bool UTerm::isUFInstance() const {
		return std::visit(overloaded {
			[](const UVariable&) { return false; },
			[](const UFInstance&) { return true; },
		}, mTerm);
	}

	const Sort& UTerm::domain() const {
		return std::visit(overloaded {
			[](const UVariable& var) -> const auto& { return var.domain(); },
			[](const UFInstance& ufi) -> const auto& { return ufi.uninterpretedFunction().codomain(); },
		}, mTerm);
	}

	std::size_t UTerm::complexity() const {
		return std::visit(overloaded {
			[](const UVariable& var) { return static_cast<std::size_t>(1); },
			[](const UFInstance& ufi) { return ufi.complexity(); },
		}, mTerm);
	}

	bool operator==(const UTerm& lhs, const UTerm& rhs) {
		return std::visit(overloaded {
			[](const UVariable& lhs, const UVariable& rhs) { return lhs.variable().id() == rhs.variable().id(); },
			[](const UFInstance& lhs, const UFInstance& rhs) { return lhs.id() == rhs.id(); },
			[](const auto&, const auto&) { return false; },
		}, lhs.asVariant(), rhs.asVariant());
	}

	bool operator<(const UTerm& lhs, const UTerm& rhs) {
		return std::visit(overloaded {
			[](const UVariable& lhs, const UVariable& rhs) { return lhs.variable().id() < rhs.variable().id(); },
			[](const UFInstance& lhs, const UFInstance& rhs) { return lhs.id() < rhs.id(); },
			[](const UVariable&, const UFInstance&) { return true; },
			[](const auto&, const auto&) { return false; },
		}, lhs.asVariant(), rhs.asVariant());
	}

	std::ostream& operator<<(std::ostream& os, const UTerm& ut) {
		return std::visit(overloaded {
			[&os](const UVariable& var) -> auto& { return os << var; },
			[&os](const UFInstance& ufi) -> auto& { return os << ufi; },
		}, ut.asVariant());
	}
}

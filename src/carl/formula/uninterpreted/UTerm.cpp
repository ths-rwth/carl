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
			[](const UVariable& var) { return true; },
			[](const UFInstance& ufi) { return false; },
		}, mTerm);
	}
	bool UTerm::isUFInstance() const {
		return std::visit(overloaded {
			[](const UVariable& var) { return false; },
			[](const UFInstance& ufi) { return true; },
		}, mTerm);
	}

	const Sort& UTerm::domain() const {
		return std::visit(overloaded {
			[](const UVariable& var) -> auto& { return var.domain(); },
			[](const UFInstance& ufi) -> auto& { return ufi.uninterpretedFunction().codomain(); },
		}, mTerm);
	}

	bool operator==(const UTerm& lhs, const UTerm& rhs) {
		return std::visit(overloaded {
			[](const UVariable& varL, const UVariable& varR) { return varL.variable().id() == varR.variable().id(); },
			[](const UFInstance& ufiL, const UFInstance& ufiR) { return ufiL.id() == ufiR.id(); },
			[](const auto& termL, const auto& termR) { return false; },
		}, lhs.asVariant(), rhs.asVariant());
	}

	bool operator<(const UTerm& lhs, const UTerm& rhs) {
		return std::visit(overloaded {
			[](const UVariable& varL, const UVariable& varR) { return varL.variable().id() < varR.variable().id(); },
			[](const UFInstance& ufiL, const UFInstance& ufiR) { return ufiL.id() < ufiR.id(); },
			[](const UVariable& varL, const UFInstance& ufiR) { return true; },
			[](const auto& termL, const auto& termR) { return false; },
		}, lhs.asVariant(), rhs.asVariant());
	}

	std::ostream& operator<<(std::ostream& os, const UTerm& ut) {
		return std::visit(overloaded {
			[&os](const UVariable& var) -> auto& { return os << var; },
			[&os](const UFInstance& ufi) -> auto& { return os << ufi; },
		}, ut.asVariant());
	}
}

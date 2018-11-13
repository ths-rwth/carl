/**
 * @file UFModel.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-24
 * @version 2014-10-24
 */

#include "UFModel.h"

#include "SortValueManager.h"

#include <cassert>

namespace carl {
	
bool UFModel::extend(const std::vector<SortValue>& args, const SortValue& value) {
	auto ret = mValues.emplace(args, value);
	assert(ret.second || ret.first->second == value); // Checks if the same arguments are not tried to map to different values.
	return ret.second;								  // Mainly because of not getting a warning, but maybe some needs this return value.
}
SortValue UFModel::get(const std::vector<SortValue>& args) const {
	auto iter = mValues.find(args);
	if (iter != mValues.end()) {
		return iter->second;
	}
	return defaultSortValue(mFunction.codomain());
}

std::ostream& operator<<(std::ostream& os, const UFModel& ufm) {
	assert(!ufm.values().empty());
	os << "(define-fun " << ufm.function().name() << " (";
	// Print function signature
	std::size_t id = 1;
	for (const auto& arg : ufm.function().domain()) {
		if (id > 1) os << " ";
		os << "(x!" << id << " " << arg << ")";
		id++;
	}
	os << ") " << ufm.function().codomain() << " ";
	// Print implementation
	for (const auto& instance : ufm.values()) {
		os << "(ite (and ";
		std::size_t id = 1;
		for (const auto& param : instance.first) {
			if (id > 0) os << " ";
			os << "(= x!" << id << " " << param << ")";
			id++;
		}
		os << ") " << instance.second << " ";
	}
	os << ufm.values().begin()->second;
	for (std::size_t i = 0; i < ufm.values().size(); i++)
		os << ")";
	os << ")";
	return os;
}
} // namespace carl

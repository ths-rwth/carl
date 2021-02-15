/**
 * @file UEquality.cpp
 * @author Florian Corzilius <corzilius@cs.rwth-aachen.de>
 * @since 2014-10-20
 * @version 2014-10-22
 */

#include "UEquality.h"

namespace carl {

void collectUFVars(std::set<UVariable>& uvars, UFInstance ufi) {
	for (const auto& arg : ufi.args()) {
		if (arg.isUVariable()) {
			uvars.insert(arg.asUVariable());
		} else if (arg.isUFInstance()) {
			collectUFVars(uvars, arg.asUFInstance());
		}
	}
}

void UEquality::gatherUVariables(std::set<UVariable>& uvars) const {
	if (mLhs.isUVariable()) {
		uvars.insert(mLhs.asUVariable());
	} else {
		collectUFVars(uvars, mLhs.asUFInstance());
	}
	if (mRhs.isUVariable()) {
		uvars.insert(mRhs.asUVariable());
	} else {
		collectUFVars(uvars, mRhs.asUFInstance());
	}
}

} // end namespace carl

#pragma once

#include <vector>

namespace carl::ran {

    template<typename RAN>
	using ran_assignment_t = std::map<Variable, RAN>;

    template<typename RAN>
	using ordered_ran_assignment_t = std::vector<std::pair<Variable, RAN>>;

}
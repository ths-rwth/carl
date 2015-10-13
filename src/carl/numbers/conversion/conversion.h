#pragma once

namespace carl {
	template<typename From, typename To>
	inline To convert(const From&);
}

#include "generic.h"
#include "cln_gmp.h"

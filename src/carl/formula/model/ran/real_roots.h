#pragma once

#include "RealAlgebraicNumber.h"

#ifdef RAN_USE_INTERVAL
#include "interval/ran_interval_real_roots.h"
#endif

#ifdef RAN_USE_THOM
#include "thom/ran_thom_real_roots.h"
#endif

#ifdef RAN_USE_Z3
#include "z3/ran_z3_real_roots.h"
#endif

namespace carl {
    #ifdef RAN_USE_INTERVAL
    using carl::ran::interval::realRoots;
    #endif

    #ifdef RAN_USE_THOM
    using carl::ran::thom::realRoots;
    #endif

    #ifdef RAN_USE_Z3
    using carl::ran::z3::realRoots;
    #endif
}
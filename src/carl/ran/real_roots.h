#pragma once

#include "ran.h"

#ifdef RAN_USE_INTERVAL
#include "interval/ran_interval_real_roots.h"
#endif

#ifdef RAN_USE_THOM
#include "thom/ran_thom_real_roots.h"
#endif

#ifdef RAN_USE_Z3
#include "z3/ran_z3_real_roots.h"
#endif

#ifdef RAN_USE_LIBPOLY
#include "libpoly/ran_libpoly_real_roots.h"
#endif

namespace carl::ran {
    #ifdef RAN_USE_INTERVAL
    using carl::ran::interval::real_roots;
    #endif

    #ifdef RAN_USE_THOM
    using carl::ran::thom::real_roots;
    #endif

    #ifdef RAN_USE_Z3
    using carl::ran::z3::real_roots;
    #endif

    #ifdef RAN_USE_LIBPOLY
    using carl::ran::libpoly::real_roots;
    #endif
}

namespace carl {
    using carl::ran::real_roots;
}
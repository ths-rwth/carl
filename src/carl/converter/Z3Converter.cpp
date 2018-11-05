#include "Z3Converter.h"

#if defined(USE_Z3_RANS) || defined(USE_Z3_NUMBERS)

#include "util/gparams.h"

namespace carl {

    Z3Converter& z3() {
        // TODO put z3 init somewhere else...
        static bool initialized = false;
        if (!initialized) {
            initialize_symbols();
            gparams::init();
            rational::initialize();
            initialized = true;
        }
        static Z3Converter m;
        return m;
    }

}

#endif
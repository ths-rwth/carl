#include "Z3Converter.h"

#if defined(USE_Z3_RANS) || defined(USE_Z3_NUMBERS) || defined(COMPARE_WITH_Z3)

namespace carl {

    // FIXME not thread safe
    Z3Converter& z3() {
        static bool initialized = false;
        if (!initialized) {
            Z3Converter::initialize();
            initialized = true;
        }
        static Z3Converter m;
        return m;
    }

}

#endif
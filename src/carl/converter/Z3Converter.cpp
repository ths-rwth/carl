#include "Z3Converter.h"

#if defined(RAN_USE_Z3) || defined(USE_Z3_NUMBERS)

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
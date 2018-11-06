#include "Z3Converter.h"

#if defined(USE_Z3_RANS) || defined(USE_Z3_NUMBERS)

namespace carl {

    Z3Converter& z3() {
        static Z3Converter m;
        return m;
    }

}

#endif
#include "initialization.h"

#include "../config.h"

#ifdef USE_MIMALLOC

#include <mimalloc.h>
#include <gmpxx.h>

namespace carl::numbers {
    void* mi_realloc_adaptor (void *ptr, size_t /*old_size*/, size_t new_size) {
        return mi_realloc(ptr,new_size);
    }

    void mi_free_adaptor (void *ptr, size_t /*size*/) {
        mi_free(ptr);
    }

    void initialize_gmpxx() {
        mp_set_memory_functions (mi_malloc, mi_realloc_adaptor, mi_free_adaptor);
    }
}

#else

namespace carl::numbers {
    void initialize_gmpxx() {}
}

#endif
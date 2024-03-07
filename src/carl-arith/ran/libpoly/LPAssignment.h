#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include <carl-common/memory/Singleton.h>
#include <map>
#include <optional>
#include "../../core/Variable.h"
#include <poly/poly.h>
#include <poly/polynomial_context.h>
#include <poly/polyxx.h>
#include <carl-arith/core/Common.h>
#include "LPRan.h"


namespace carl {

class LPAssignment : public Singleton<LPAssignment> {
    friend Singleton<LPAssignment>;

    lp_assignment_t lp_assignment;
    carl::Assignment<LPRealAlgebraicNumber> last_assignment;

public:
    LPAssignment();
    ~LPAssignment();
    lp_assignment_t& get(const carl::Assignment<LPRealAlgebraicNumber>& ass);
};
    
} // namespace carl

#endif
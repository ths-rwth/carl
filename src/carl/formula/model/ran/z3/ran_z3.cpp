#include "ran_z3.h"

#ifdef RAN_USE_Z3

#include "../../../../converter/Z3Converter.h"

#include "Z3RanContent.h"

namespace carl {

template<typename Number>
real_algebraic_number_z3<Number> sample_below(const real_algebraic_number_z3<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_lt(n.z3_ran().content(), val);
    return real_algebraic_number_z3<Number>(Z3Ran<Number>(Z3RanContent(std::move(val))));
}

template<typename Number>
real_algebraic_number_z3<Number> sample_between(const real_algebraic_number_z3<Number>& lower, const real_algebraic_number_z3<Number>& upper) {
    algebraic_numbers::anum val;
    z3().anumMan().select(lower.z3_ran().content(), upper.z3_ran().content(), val);
    return real_algebraic_number_z3<Number>(Z3Ran<Number>(Z3RanContent(std::move(val))));
}

template<typename Number>
real_algebraic_number_z3<Number> sample_above(const real_algebraic_number_z3<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_gt(n.z3_ran().content(), val);
    return real_algebraic_number_z3<Number>(Z3Ran<Number>(Z3RanContent(std::move(val))));
}

template real_algebraic_number_z3<mpq_class> sample_below(const real_algebraic_number_z3<mpq_class>& n);
template real_algebraic_number_z3<mpq_class> sample_between(const real_algebraic_number_z3<mpq_class>& lower, const real_algebraic_number_z3<mpq_class>& upper);
template real_algebraic_number_z3<mpq_class> sample_above(const real_algebraic_number_z3<mpq_class>& n);

}

#endif
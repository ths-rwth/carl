/*
 * common.h
 *
 *  Created on: 15 Apr 2016
 *      Author: hbruintjes
 */

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace py = pybind11;

namespace pybind11 {
namespace detail {
/**
 * Dummy type caster for handle, so functions can return pybind11 handles directly
 */
template <> class type_caster<handle> {
public:
    bool load(handle src, bool) {
        value = handle(src).inc_ref();
        return true;
    }

    static handle cast(handle src, return_value_policy policy, handle parent) {
        switch(policy) {
            case return_value_policy::automatic:
            case return_value_policy::copy:
            case return_value_policy::take_ownership:
                return handle(src);
            case return_value_policy::reference:
                return handle(src).inc_ref();
            case return_value_policy::reference_internal:
                parent.inc_ref();
                return handle(src);
        }
        return handle(src);
    }
    PYBIND11_TYPE_CASTER(handle, _("handle"));
};

}
}


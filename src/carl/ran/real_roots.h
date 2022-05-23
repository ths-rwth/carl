#pragma once

#include "ran.h"


#include "interval/ran_interval_real_roots.h"



//#include "thom/ran_thom_real_roots.h"



#include "libpoly/ran_libpoly_real_roots.h"


namespace carl::ran {

    
    //sadly cant use this because std::forward discards qualifiersc
    

    //Base real roots always uses interval 
    //template<typename... Args>
    //auto real_roots(const Args&&... args) -> decltype(carl::ran::interval::real_roots(std::forward<Args>(args)...)) {
    //    return carl::ran::interval::real_roots(std::forward<Args>(args)...);
    //}

     //real roots using interval 
    //template<typename... Args>
    //auto real_roots_interval(const Args&&... args) -> decltype(carl::ran::interval::real_roots(std::forward<Args>(args)...)) {
    //    return carl::ran::interval::real_roots(std::forward<Args>(args)...);
    //}

    //real roots using libpoly
    //template<typename... Args>
    //auto real_roots_libpoly(const Args&&... args) -> decltype(carl::ran::libpoly::real_roots(std::forward<Args>(args)...)) {
    //    return carl::ran::libpoly::real_roots(std::forward<Args>(args)...);
    //}

    ////real roots using thom
    //template<typename... Args>
    //auto real_roots_thom(const Args&&... args) -> decltype(carl::ran::thom::real_roots(std::forward<Args>(args)...)) {
    //    return carl::ran::thom::real_roots(std::forward<Args>(args)...);
    //}


    using carl::ran::interval::real_roots;

    using carl::ran::libpoly::real_roots_libpoly ;

}

namespace carl {
    using carl::ran::real_roots;
    using carl::ran::real_roots_libpoly;
}
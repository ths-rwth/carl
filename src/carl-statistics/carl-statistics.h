/**
 * @file
 *
 * A small wrapper that makes using statistics easier in carl.
 * 
 *
 * Note that this header should *not* be included if you want to use the carl
 * statistics facilities yourself. To do that, include Statistics.h and create
 * statistics macros like below for your own application.
 * 
 */

#pragma once

#include "config.h"
#include "Statistics.h"

namespace carl {
namespace statistics {

#ifdef CARL_DEVOPTION_Statistics
    #define CARL_INIT_STATISTICS(class, member, name) class& member = carl::statistics::get<class>(name)
    #define CARL_CALL_STATISTICS(function) function
    #define CARL_TIME_START() carl::statistics::timer::start()
    #define CARL_TIME_FINISH(timer, start) timer.finish(start)
#else
    #define CARL_INIT_STATISTICS(class, member, name)
    #define CARL_CALL_STATISTICS(function)
    #define CARL_TIME_START() carl::statistics::timing::time_point()
    #define CARL_TIME_FINISH(timer, start) static_cast<carl::statistics::timing::time_point>(start)
#endif


}
}
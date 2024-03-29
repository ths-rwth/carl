#pragma once

#include <carl-statistics/carl-statistics.h>
 
#ifdef CARL_DEVOPTION_Statistics
 
namespace carl {
namespace cocoa {
 
class CoCoAAdaptorStatistics : public statistics::Statistics {
public:
    statistics::Timer gcd;
    statistics::Timer factorize;
    statistics::Timer gbasis;
    void collect() {
        Statistics::addKeyValuePair("gcd", gcd);
        Statistics::addKeyValuePair("factorize", factorize);
        Statistics::addKeyValuePair("gbasis", gbasis);
    }
};

static auto& statistics() {
    static CARL_INIT_STATISTICS(CoCoAAdaptorStatistics, stats, "cocoa");
    return stats;
}
 
}
}
#endif

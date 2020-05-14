#include "StatisticsCollector.h"

#include "Statistics.h"

namespace carl {
namespace statistics {

void StatisticsCollector::collect() {
	for (auto& s: mStatistics) {
		if (s->enabled()) {
			s->collect();
		}
	}
}

}
}
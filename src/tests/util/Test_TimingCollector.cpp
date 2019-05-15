#define TIMING
#include <carl/util/TimingCollector.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

TEST(TimingCollector, Basics)
{
	auto start = CARL_TIME_START();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	CARL_TIME_FINISH("dummy", start);
	
	std::cout << carl::TimingCollector::getInstance() << std::endl;
}

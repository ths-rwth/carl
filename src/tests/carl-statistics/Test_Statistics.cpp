#include "../get_output.h"

#include <carl-statistics/Statistics.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

TEST(Statistics, Timer)
{
	auto start = carl::statistics::Timer::start();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	carl::statistics::Timer timer;
	timer.finish(start);
	ASSERT_EQ(timer.count(), 1);
}

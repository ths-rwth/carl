#include <carl-common/debug/Timer.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../get_output.h"

TEST(Timer, Basics)
{
	constexpr std::size_t delay = 50;
	carl::Timer t;
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	EXPECT_TRUE(t.passed() >= delay);
	t.reset();
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	EXPECT_TRUE(t.passed() >= delay);
	EXPECT_TRUE(std::stoll(get_output(t)) >= 50);
}

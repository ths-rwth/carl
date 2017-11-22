#include "../Common.h"

#include <carl/util/Timer.h>

#include <chrono>
#include <thread>

TEST(Timer, Basics)
{
	constexpr std::size_t delay = 50;
	carl::Timer t;
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	EXPECT_TRUE(t.passed() >= delay);
	t.reset();
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	EXPECT_TRUE(t.passed() >= delay);
	EXPECT_TRUE(std::stoll(getOutput(t)) >= 50);
}

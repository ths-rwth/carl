#include "../Common.h"

#include <carl/core/carlLogging.h>
#include <gtest/gtest.h>

#include <sstream>

TEST(Logging, LogLevelOutput)
{
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_ALL) == "ALL  ");
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_TRACE) == "TRACE");
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_DEBUG) == "DEBUG");
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_INFO) == "INFO ");
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_WARN) == "WARN ");
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_ERROR) == "ERROR");
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_FATAL) == "FATAL");
	EXPECT_TRUE(getOutput(carl::logging::LogLevel::LVL_OFF) == "OFF  ");
}

TEST(Logging, StreamSink)
{
	std::stringstream ss;
	carl::logging::StreamSink sink(ss);
	EXPECT_EQ(ss.rdbuf(), sink.log().rdbuf());
}

TEST(Logging, Filter)
{
	carl::logging::Filter filter;
}

TEST(Logging, Formatter)
{
	carl::logging::Filter filter;
	carl::logging::Formatter formatter;
	formatter.configure(filter);
}

TEST(Logging, Logger)
{
	auto& logger = carl::logging::logger();
}

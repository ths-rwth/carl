#include <gtest/gtest.h>


#include <carl-logging/carl-logging.h>
#include <carl-logging/logging-internals.h>

#include "../get_output.h"

#include <sstream>

TEST(Logging, LogLevelOutput)
{
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_ALL) == "ALL  ");
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_TRACE) == "TRACE");
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_DEBUG) == "DEBUG");
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_INFO) == "INFO ");
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_WARN) == "WARN ");
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_ERROR) == "ERROR");
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_FATAL) == "FATAL");
	EXPECT_TRUE(get_output(carl::logging::LogLevel::LVL_OFF) == "OFF  ");
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
	UNUSED(logger);
}

TEST(LoggingHelper, binary)
{
	EXPECT_EQ("00000000 00000000 00110000 00111001", carl::binary(int(12345)));
	EXPECT_EQ("00111111 11110100 00000000 00000000 00000000 00000000 00000000 00000000", carl::binary(double(1.25)));
}

TEST(LoggingHelper, basename)
{
	EXPECT_EQ("abc.de", carl::basename("/foo/bar/abc.de"));
}

#include <gtest/gtest.h>

#include <carl-settings/settings_utils.h>

#include "../Common.h"

TEST(settings_utils, get_proper_suffix) {
	using namespace carl::settings;
	using P = std::pair<std::intmax_t,std::size_t>;

	std::array<std::pair<std::string,long>,7> suffixes = {{
		{"", 1000}, {"K", 1000}, {"M", 1000}, {"G", 1000}
	}};
	EXPECT_EQ(get_proper_suffix(1, suffixes), P(1, 0));
	EXPECT_EQ(get_proper_suffix(10, suffixes), P(10, 0));
	EXPECT_EQ(get_proper_suffix(100, suffixes), P(100, 0));
	EXPECT_EQ(get_proper_suffix(1000, suffixes), P(1, 1));
	EXPECT_EQ(get_proper_suffix(10000, suffixes), P(10, 1));
	EXPECT_EQ(get_proper_suffix(100000, suffixes), P(100, 1));
	EXPECT_EQ(get_proper_suffix(1000000, suffixes), P(1, 2));
	EXPECT_EQ(get_proper_suffix(10000000, suffixes), P(10, 2));
	EXPECT_EQ(get_proper_suffix(100000000, suffixes), P(100, 2));
	EXPECT_EQ(get_proper_suffix(1000000000, suffixes), P(1, 3));
	EXPECT_EQ(get_proper_suffix(10000000000, suffixes), P(10, 3));
}

TEST(settings_utils, duration) {
	using namespace carl::settings;
	static_assert(duration(1) == std::chrono::nanoseconds(1));
	static_assert(duration(1000) == std::chrono::nanoseconds(1000));
	static_assert(duration(std::chrono::microseconds(1)) == std::chrono::nanoseconds(1000));
	static_assert(duration(std::chrono::milliseconds(1)) == std::chrono::nanoseconds(1000000));
	static_assert(duration(std::chrono::seconds(1)) == std::chrono::nanoseconds(1000000000));

	EXPECT_EQ(getOutput(duration(std::chrono::nanoseconds(5))), "5ns");
	EXPECT_EQ(getOutput(duration(std::chrono::microseconds(5))), "5µs");
	EXPECT_EQ(getOutput(duration(std::chrono::milliseconds(5))), "5ms");
	EXPECT_EQ(getOutput(duration(std::chrono::seconds(5))), "5s");
	EXPECT_EQ(getOutput(duration(std::chrono::minutes(5))), "5m");
	EXPECT_EQ(getOutput(duration(std::chrono::hours(5))), "5h");
}

TEST(settings_utils, binary_quantity) {
	using namespace carl::settings;
	{
		constexpr auto b = binary_quantity();
		static_assert(b.n() == 0);
		static_assert(b.kibi() == 0);
		static_assert(b.mebi() == 0);
		static_assert(b.gibi() == 0);
		static_assert(b.tebi() == 0);
		static_assert(b.pebi() == 0);
		static_assert(b.exbi() == 0);
		EXPECT_EQ(getOutput(b), "0");
	}
	{
		constexpr auto b = binary_quantity(5);
		static_assert(b.n() == 5);
		static_assert(b.kibi() == 0);
		static_assert(b.mebi() == 0);
		static_assert(b.gibi() == 0);
		static_assert(b.tebi() == 0);
		static_assert(b.pebi() == 0);
		static_assert(b.exbi() == 0);
		EXPECT_EQ(getOutput(b), "5");
	}
	{
		constexpr auto b = binary_quantity(5000);
		static_assert(b.n() == 5000);
		static_assert(b.kibi() == 4);
		static_assert(b.mebi() == 0);
		static_assert(b.gibi() == 0);
		static_assert(b.tebi() == 0);
		static_assert(b.pebi() == 0);
		static_assert(b.exbi() == 0);
		EXPECT_EQ(getOutput(b), "4Ki");
	}
	{
		constexpr auto b = binary_quantity(5000000);
		static_assert(b.n() == 5000000);
		static_assert(b.kibi() == 4882);
		static_assert(b.mebi() == 4);
		static_assert(b.gibi() == 0);
		static_assert(b.tebi() == 0);
		static_assert(b.pebi() == 0);
		static_assert(b.exbi() == 0);
		EXPECT_EQ(getOutput(b), "4Mi");
	}
	{
		constexpr auto b = binary_quantity(5000000000);
		static_assert(b.n() == 5000000000);
		static_assert(b.kibi() == 4882812);
		static_assert(b.mebi() == 4768);
		static_assert(b.gibi() == 4);
		static_assert(b.tebi() == 0);
		static_assert(b.pebi() == 0);
		static_assert(b.exbi() == 0);
		EXPECT_EQ(getOutput(b), "4Gi");
	}
	{
		constexpr auto b = binary_quantity(5000000000000);
		static_assert(b.n() == 5000000000000);
		static_assert(b.kibi() == 4882812500);
		static_assert(b.mebi() == 4768371);
		static_assert(b.gibi() == 4656);
		static_assert(b.tebi() == 4);
		static_assert(b.pebi() == 0);
		static_assert(b.exbi() == 0);
		EXPECT_EQ(getOutput(b), "4Ti");
	}
	{
		constexpr auto b = binary_quantity(5000000000000000);
		static_assert(b.n() == 5000000000000000);
		static_assert(b.kibi() == 4882812500000);
		static_assert(b.mebi() == 4768371582);
		static_assert(b.gibi() == 4656612);
		static_assert(b.tebi() == 4547);
		static_assert(b.pebi() == 4);
		static_assert(b.exbi() == 0);
		EXPECT_EQ(getOutput(b), "4Pi");
	}
	{
		constexpr auto b = binary_quantity(5000000000000000000);
		static_assert(b.n() == 5000000000000000000);
		static_assert(b.kibi() == 4882812500000000);
		static_assert(b.mebi() == 4768371582031);
		static_assert(b.gibi() == 4656612873);
		static_assert(b.tebi() == 4547473);
		static_assert(b.pebi() == 4440);
		static_assert(b.exbi() == 4);
		EXPECT_EQ(getOutput(b), "4Ei");
	}
}

TEST(settings_utils, metric_quantity) {
	using namespace carl::settings;
	{
		constexpr auto m = metric_quantity();
		static_assert(m.n() == 0);
		static_assert(m.kilo() == 0);
		static_assert(m.mega() == 0);
		static_assert(m.giga() == 0);
		static_assert(m.tera() == 0);
		static_assert(m.peta() == 0);
		static_assert(m.exa() == 0);
		EXPECT_EQ(getOutput(m), "0");
	}
	{
		constexpr auto m = metric_quantity(5);
		static_assert(m.n() == 5);
		static_assert(m.kilo() == 0);
		static_assert(m.mega() == 0);
		static_assert(m.giga() == 0);
		static_assert(m.tera() == 0);
		static_assert(m.peta() == 0);
		static_assert(m.exa() == 0);
		EXPECT_EQ(getOutput(m), "5");
	}
	{
		constexpr auto m = metric_quantity(5000);
		static_assert(m.n() == 5000);
		static_assert(m.kilo() == 5);
		static_assert(m.mega() == 0);
		static_assert(m.giga() == 0);
		static_assert(m.tera() == 0);
		static_assert(m.peta() == 0);
		static_assert(m.exa() == 0);
		EXPECT_EQ(getOutput(m), "5K");
	}
	{
		constexpr auto m = metric_quantity(5000000);
		static_assert(m.n() == 5000000);
		static_assert(m.kilo() == 5000);
		static_assert(m.mega() == 5);
		static_assert(m.giga() == 0);
		static_assert(m.tera() == 0);
		static_assert(m.peta() == 0);
		static_assert(m.exa() == 0);
		EXPECT_EQ(getOutput(m), "5M");
	}
	{
		constexpr auto m = metric_quantity(5000000000);
		static_assert(m.n() == 5000000000);
		static_assert(m.kilo() == 5000000);
		static_assert(m.mega() == 5000);
		static_assert(m.giga() == 5);
		static_assert(m.tera() == 0);
		static_assert(m.peta() == 0);
		static_assert(m.exa() == 0);
		EXPECT_EQ(getOutput(m), "5G");
	}
	{
		constexpr auto m = metric_quantity(5000000000000);
		static_assert(m.n() == 5000000000000);
		static_assert(m.kilo() == 5000000000);
		static_assert(m.mega() == 5000000);
		static_assert(m.giga() == 5000);
		static_assert(m.tera() == 5);
		static_assert(m.peta() == 0);
		static_assert(m.exa() == 0);
		EXPECT_EQ(getOutput(m), "5T");
	}
	{
		constexpr auto m = metric_quantity(5000000000000000);
		static_assert(m.n() == 5000000000000000);
		static_assert(m.kilo() == 5000000000000);
		static_assert(m.mega() == 5000000000);
		static_assert(m.giga() == 5000000);
		static_assert(m.tera() == 5000);
		static_assert(m.peta() == 5);
		static_assert(m.exa() == 0);
		EXPECT_EQ(getOutput(m), "5P");
	}
	{
		constexpr auto m = metric_quantity(5000000000000000000);
		static_assert(m.n() == 5000000000000000000);
		static_assert(m.kilo() == 5000000000000000);
		static_assert(m.mega() == 5000000000000);
		static_assert(m.giga() == 5000000000);
		static_assert(m.tera() == 5000000);
		static_assert(m.peta() == 5000);
		static_assert(m.exa() == 5);
		EXPECT_EQ(getOutput(m), "5E");
	}
}
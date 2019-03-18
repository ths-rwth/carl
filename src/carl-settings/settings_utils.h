#pragma once

#include <boost/any.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <utility>

namespace carl::settings {

template<typename Array>
std::pair<std::intmax_t,std::size_t> get_proper_suffix(std::intmax_t value, const Array& a) {
	std::size_t id = 0;
	std::intmax_t mult = 1;
	for (; id < a.size()-1; ++id) {
		if (value < (a[id].second * mult)) {
			break;
		}
		mult *= a[id].second;
	}
	return std::make_pair(value / mult, id);
}

/**
 * Helper type to parse duration as std::chrono values with boost::program_options.
 * Intended usage:
 * - use boost to parse values as durations
 * - access values with `std::chrono::seconds(d)`
 */
struct duration: std::chrono::nanoseconds {
	constexpr duration() = default;
	template<typename... Args>
	// NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions): we actually want to implicitly convert here.
	constexpr duration(Args&&... args): std::chrono::nanoseconds(std::forward<Args>(args)...) {}
	template<typename R, typename P>
	explicit constexpr operator std::chrono::duration<R,P>() const {
		return std::chrono::duration_cast<std::chrono::duration<R,P>>(std::chrono::nanoseconds(*this));
	}
};

/// Streaming operator for duration. Auto-detects proper time suffix.
inline std::ostream& operator<<(std::ostream& os, const duration& d) {
	std::array<std::pair<std::string,intmax_t>,6> suffixes = {{
		{"ns", 1000}, {"µs", 1000}, {"ms", 1000},
		{"s", 60}, {"m", 60}, {"h", 1},
	}};
	auto res = get_proper_suffix(d.count(), suffixes);
	return os << res.first << suffixes[res.second].first;
}

/**
 * Custom validator for duration that wraps some std::chrono::duration.
 * Accepts the format <number><suffix> where suffix is one of the following:
 * ns, µs, us, ms, s, m, h
 */
void validate(boost::any& v, const std::vector<std::string>& values, carl::settings::duration* /*unused*/, int /*unused*/);

/**
 * Helper type to parse quantities with binary SI-style suffixes.
 * Intended usage:
 * - use boost to parse values as quantity
 * - access values with `q.mibi()`
 */
struct binary_quantity {
private:
	std::size_t mN = 0;
public:
	constexpr binary_quantity() = default;
	explicit constexpr binary_quantity(std::size_t n): mN(n) {}
	constexpr auto n() const { return mN; }
	constexpr auto kibi() const { return mN >> 10U; }
	constexpr auto mebi() const { return mN >> 20U; }
	constexpr auto gibi() const { return mN >> 30U; }
	constexpr auto tebi() const { return mN >> 40U; }
	constexpr auto pebi() const { return mN >> 50U; }
	constexpr auto exbi() const { return mN >> 60U; }
};
/// Compare two binary quantities for equality.
constexpr bool operator==(binary_quantity lhs, binary_quantity rhs) {
	return lhs.n() == rhs.n();
}
/// Compare two binary quantities.
constexpr bool operator<(binary_quantity lhs, binary_quantity rhs) {
	return lhs.n() < rhs.n();
}

/// Streaming operator for binary quantity. Auto-detects proper suffix.
inline std::ostream& operator<<(std::ostream& os, const binary_quantity& q) {
	std::array<std::pair<std::string,long>,7> suffixes = {{
		{"", 1024}, {"Ki", 1024}, {"Mi", 1024},
		{"Gi", 1024}, {"Ti", 1024}, {"Pi", 1024}, {"Ei", 1},
	}};
	auto res = get_proper_suffix(static_cast<std::intmax_t>(q.n()), suffixes);
	return os << res.first << suffixes[res.second].first;
}

/**
 * Custom validator for binary quantities.
 * Accepts the format <number><suffix> where suffix is one of the following:
 * Ki,Mi,Gi,Ti,Pi,Ei
 */
void validate(boost::any& v, const std::vector<std::string>& values, carl::settings::binary_quantity* /*unused*/, int /*unused*/);

/**
 * Helper type to parse quantities with SI-style suffixes.
 * Intended usage:
 * - use boost to parse values as quantity
 * - access values with `q.mega()`
 */
struct metric_quantity {
private:
	std::size_t mN = 0;
public:
	constexpr metric_quantity() = default;
	explicit constexpr metric_quantity(std::size_t n): mN(n) {}
	constexpr auto n() const { return mN; }
	constexpr auto kilo() const { return mN / 1000; }
	constexpr auto mega() const { return mN / 1000000; }
	constexpr auto giga() const { return mN / 1000000000; }
	constexpr auto tera() const { return mN / 1000000000000; }
	constexpr auto peta() const { return mN / 1000000000000000; }
	constexpr auto exa()  const { return mN / 1000000000000000000; }
};
/// Compare two metric quantities for equality.
constexpr bool operator==(metric_quantity lhs, metric_quantity rhs) {
	return lhs.n() == rhs.n();
}
/// Compare two metric quantities.
constexpr bool operator<(metric_quantity lhs, metric_quantity rhs) {
	return lhs.n() < rhs.n();
}

/// Streaming operator for metric quantity. Auto-detects proper suffix.
inline std::ostream& operator<<(std::ostream& os, const metric_quantity& q) {
	std::array<std::pair<std::string,long>,7> suffixes = {{
		{"", 1000}, {"K", 1000}, {"M", 1000},
		{"G", 1000}, {"T", 1000}, {"P", 1000}, {"E", 1},
	}};
	auto res = get_proper_suffix(static_cast<std::intmax_t>(q.n()), suffixes);
	return os << res.first << suffixes[res.second].first;
}

/**
 * Custom validator for metric quantities.
 * Accepts the format <number><suffix> where suffix is one of the following:
 * K,M,G,T,P,E
 */
void validate(boost::any& v, const std::vector<std::string>& values, carl::settings::metric_quantity* /*unused*/, int /*unused*/);

}
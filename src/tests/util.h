#pragma once

#include <boost/variant.hpp>

#include <list>
#include <vector>

template<typename... T>
using ComparisonList = std::vector<boost::variant<T...>>;

template<typename Iterator>
struct ComparisonOperator {
	using result_type = void;
	Iterator it1, it2;
	ComparisonOperator(): it1(), it2() {}
	void set(const Iterator& i1, const Iterator& i2) {
		it1 = i1;
		it2 = i2;
	}
	template<typename T1, typename T2>
	void operator()(const T1& t1, const T2& t2) {
		EXPECT_EQ(it1 < it2, t1 < t2) << t1 << " < " << t2;
		EXPECT_EQ(it1 > it2, t1 > t2) << t1 << " > " << t2;
		EXPECT_EQ(it1 <= it2, t1 <= t2) << t1 << " <= " << t2;
		EXPECT_EQ(it1 >= it2, t1 >= t2) << t1 << " >= " << t2;
		EXPECT_EQ(it1 == it2, t1 == t2) << t1 << " == " << t2;
		EXPECT_EQ(it1 != it2, t1 != t2) << t1 << " != " << t2;
	}
};

template<typename Iterator, typename Less>
struct SingleComparisonOperator {
	using result_type = void;
	Iterator it1, it2;
	const Less& less;
	explicit SingleComparisonOperator(const Less& l): less(l) {}
	void set(const Iterator& i1, const Iterator& i2) {
		it1 = i1;
		it2 = i2;
	}
	template<typename T1, typename T2>
	void operator()(const T1& t1, const T2& t2) {
		EXPECT_EQ(it1 < it2, less(t1, t2)) << t1 << " < " << t2;
	}
};

template<typename List>
void expectRightOrder(const List& list) {
	ComparisonOperator<typename List::const_iterator> c;
	for (auto it1 = list.begin(); it1 != list.end(); it1++) {
		for (auto it2 = list.begin(); it2 != list.end(); it2++) {
			c.set(it1, it2);
			boost::apply_visitor(c, *it1, *it2);
		}
	}
}

template<typename List, typename Less>
void expectRightOrder(const List& list, const Less& less) {
	SingleComparisonOperator<typename List::const_iterator, Less> c(less);
	for (auto it1 = list.begin(); it1 != list.end(); it1++) {
		for (auto it2 = list.begin(); it2 != list.end(); it2++) {
			c.set(it1, it2);
			boost::apply_visitor(c, *it1, *it2);
		}
	}
}

template<typename List>
void checkEqual(const List& list) {
	for (auto it1 = list.begin(); it1 != list.end(); it1++) {
		for (auto it2 = list.begin(); it2 != list.end(); it2++) {
			EXPECT_EQ(it1 == it2, *it1 == *it2) << *it1 << " == " << *it2;
		}
	}
}

template<typename T>
void checkEqual(const std::initializer_list<T>& list) {
	checkEqual(std::vector<T>(list));
}

template<typename List>
void checkLess(const List& list) {
	for (auto it1 = list.begin(); it1 != list.end(); it1++) {
		for (auto it2 = list.begin(); it2 != list.end(); it2++) {
			EXPECT_EQ(it1 < it2, *it1 < *it2) << *it1 << " < " << *it2;
		}
	}
}

template<typename T>
void checkLess(const std::initializer_list<T>& list) {
	checkLess(std::vector<T>(list));
}

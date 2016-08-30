#pragma once

namespace carl {

template<typename T>
class Covering {
	template<typename TT>
	friend std::ostream& operator<<(std::ostream& os, const Covering<TT>& ri);
private:
	std::map<T, carl::Bitset> mData;
	carl::Bitset mOkay;
public:
	Covering(std::size_t intervals) {
		mOkay.resize(intervals, true);
	}
	void add(const T& t, const carl::Bitset& b) {
		mData.emplace(t, b);
		mOkay -= b;
	}
	bool conflicts() const {
		return mOkay.none();
	}
	std::size_t satisfyingInterval() const {
		return mOkay.find_first();
	}
	void buildConflictingCore(std::vector<T>& core) const {
		std::map<T, carl::Bitset> data = mData;
		carl::Bitset covered;
		covered.resize(mOkay.size(), true);
		while (covered.any()) {
			auto maxit = data.begin();
			for (auto it = data.begin(); it != data.end(); it++) {
				if (maxit->second.count() < it->second.count()) maxit = it;
			}
			core.push_back(maxit->first);
			covered -= maxit->second;
			data.erase(maxit);
			for (auto& d: data) {
				d.second &= covered;
			}
		}
	}
};
template<typename TT>
std::ostream& operator<<(std::ostream& os, const Covering<TT>& ri) {
	os << "Covering: " << ri.mOkay << std::endl;
	for (const auto& d: ri.mData) {
		os << "\t" << d.first << " -> " << d.second << std::endl;
	}
	return os;
}

}

/**
 * @file BenchmarkOutput.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {

typedef std::map<std::string, unsigned> BenchmarkResult;

template<typename... Identifier>
class BenchmarkFile {
private:
	std::vector<std::string> names;
	std::vector<std::pair<std::tuple<Identifier...>,BenchmarkResult>> data;
public:
	BenchmarkFile(const std::initializer_list<std::string>& init): names(init) {}
	void push(const BenchmarkResult& res, Identifier... identifier) {
		data.emplace_back(std::make_tuple(identifier...), res);
	}
	template<typename... Ident>
	friend std::ostream& operator<<(std::ostream& os, const BenchmarkFile<Ident...>& file);
};

template<typename... Identifier>
std::ostream& operator<<(std::ostream& os, const BenchmarkFile<Identifier...>& file) {
	os << "\\begin{tabular}{|r";
    auto ns = file.names.size();
	for (unsigned i = 0; i < ns; ++i) os << "|r";
	os << "|}" << std::endl;
	os << "\\hline" << std::endl;
	os << "degree";
	for (auto name: file.names) {
		os << "\t& " << name;
	}
	os << " \\\\" << std::endl << "\\hline" << std::endl;
	for (auto res: file.data) {
		if (sizeof...(Identifier) == 1) os << std::get<0>(res.first);
		else os << res.first;
		for (auto name: file.names) {
			auto it = res.second.find(name);
			if (it == res.second.end()) os << "\t& --";
			else os << "\t& " << it->second;
		}
		os << " \\\\" << std::endl << "\\hline" << std::endl;
	}
	os << "\\end{tabular}" << std::endl;
	return os;
}

}
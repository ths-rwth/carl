/**
 * @file BenchmarkOutput.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <carl-common/datastructures/streamingOperators.h>

namespace carl {

typedef std::map<std::string, std::size_t> BenchmarkResult;

template<typename... Identifier>
class BenchmarkFile {
private:
	std::map<std::string, bool> names;
	std::vector<std::pair<std::tuple<Identifier...>,BenchmarkResult>> data;
	static std::vector<std::string> tikzColors;
	static std::vector<std::string> tikzMarks;
public:
	BenchmarkFile(const std::initializer_list<std::string>& init) {
		for (const auto& name: init) names[name] = false;
	}
	void push(const BenchmarkResult& res, Identifier... identifier) {
		data.emplace_back(std::make_tuple(identifier...), res);
		for (const auto& it: res) names[it.first] = true;
	}
	void writeMain(std::ostream& os, const std::string& benchmark) {
		os << "\\input{benchmark_" << benchmark << "_table.tex" << "}" << std::endl;
		os << "\\input{benchmark_" << benchmark << "_plot.tex" << "}" << std::endl;
		os << "\\newpage" << std::endl;
	}
	void writePlotData(std::ostream& os, const std::string& benchmark) {
		os << "\\begin{filecontents}{benchmark_" << benchmark << ".data}" << std::endl;
		os << "# degree";
		for (const auto& name: names) {
			if (name.second) os << "\t" << name.first;
		}
		os << std::endl;
		for (const auto& res: data) {
			if (sizeof...(Identifier) == 1) os << std::get<0>(res.first);
			else os << res.first;
			for (const auto& name: names) {
				if (name.second) {
					auto it = res.second.find(name.first);
					if (it != res.second.end()) os << "\t" << (it->second / 1000.0);
					else os << "\tnan";
				}
			}
			os << std::endl;
		}
		os << "\\end{filecontents}" << std::endl;
	}
	void writePlot(std::ostream& os, const std::string& benchmark) {
		os << "\\begin{tikzpicture}[scale=0.5]" << std::endl;
		os << "\\begin{axis}[grid=major, ymin=0, legend pos=north west, xtick=data, y tick label style={/pgf/number format/.cd, fixed, precision=2, /tikz/.cd}]" << std::endl;
		unsigned row = 1;
		for (const auto& name: names) {
			if (name.second) {
				os << "\\addplot[mark=" << tikzMarks[row-1] << ", " << tikzColors[row-1] << "] table[x index=0,y index=" << row << "] {benchmark_" << benchmark << ".data};" << std::endl;
				os << "\\addlegendentry{" << name.first << "}" << std::endl;
				row++;
			}
		}
		os << "\\end{axis}" << std::endl;
		os << "\\end{tikzpicture}" << std::endl;
	}
	void writeTable(std::ostream& os) {
		os << "\\begin{tabular}{|r";
		auto ns = names.size();
		for (unsigned i = 0; i < ns; ++i) os << "|r";
		os << "|}" << std::endl;
		os << "\\hline" << std::endl;
		os << "degree";
		for (const auto& name: names) {
			os << "\t& " << name.first;
		}
		os << " \\\\" << std::endl << "\\hline" << std::endl;
		for (const auto& res: data) {
			if (sizeof...(Identifier) == 1) os << std::get<0>(res.first);
			else os << res.first;
			for (const auto& name: names) {
				auto it = res.second.find(name.first);
				if (it == res.second.end()) os << "\t& --";
				else os << "\t& " << it->second;
			}
			os << " \\\\" << std::endl << "\\hline" << std::endl;
		}
		os << "\\end{tabular}" << std::endl;
	}
};

template<typename... I>
std::vector<std::string> BenchmarkFile<I...>::tikzColors = {"black", "blue", "orange", "teal"};
template<typename... I>
std::vector<std::string> BenchmarkFile<I...>::tikzMarks = {"square*", "triangle*", "o", "triangle*"};

}

#pragma once

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include <carl/formula/Formula.h>
#include <carl/core/logging.h>

namespace carl {

template<typename Pol>
class DIMACSImporter {
private:
	std::ifstream in;
	std::vector<Formula<Pol>> variables;
	std::regex headerRegex;
	
	Formula<Pol> parseLine(const std::string& line) const {
		std::vector<Formula<Pol>> vars;
		const char* begin = line.c_str();
		char* end = nullptr;
		long long id;
		while (true) {
			id = std::strtoll(begin, &end, 10);
			begin = end;
			if (id == 0) break;
			Formula<Pol> v = variables.at(std::size_t(std::abs(id)-1));
			if (id > 0) vars.emplace_back(v);
			else vars.emplace_back(NOT, v);
		}
		return std::move(Formula<Pol>(OR, std::move(vars)));
	}
	
	Formula<Pol> parseFormula() {
		std::string line;
		std::vector<Formula<Pol>> formulas;
		while (!in.eof()) {
			std::getline(in, line);
			if (line == "") continue;
			if (line == "reset") break;
			if (line.front() == 'c') continue;
			if (line.front() == 'p') {
				std::smatch m;
				if (!std::regex_match(line, m, headerRegex)) {
					CARL_LOG_ERROR("carl.formula", "DIMACS line starting with \"p\" does not match header format: \"" << line << "\".");
				}
				std::size_t varCount = std::stoull(m[1]);
				variables.reserve(varCount);
				while (variables.size() < varCount) {
					variables.emplace_back(freshBooleanVariable());
				}
				continue;
			}
			formulas.push_back(parseLine(line));
		}
		return Formula<Pol>(AND, std::move(formulas));
	}
	
public:
	DIMACSImporter(const std::string& filename):
		in(filename),
		headerRegex("p cnf (\\d+) (\\d+)")
	{}
	
	bool hasNext() const {
		return !in.eof();
	}
	
	Formula<Pol> next() {
		return parseFormula();
	}
};

}

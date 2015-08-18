#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Formula.h"
#include "../core/logging.h"


#ifdef USE_BOOST_REGEX
#include <boost/regex.hpp>
namespace benchmax {
	using boost::regex;
	using boost::regex_match;
	using boost::smatch;
	using boost::regex_iterator;
}
#else
#include <stdexcept>
#include <regex>
namespace benchmax {
	using std::regex;
	using std::regex_match;
	using std::smatch;
	using std::sregex_iterator;
}
#endif

namespace carl {

template<typename Pol>
class DIMACSImporter {
private:
	std::ifstream in;
	std::vector<Variable> variables;
	regex headerRegex;
	
	Formula<Pol> parseLine(const std::string& line) const {
		std::vector<Formula<Pol>> vars;//(std::count(line.begin(), line.end(), ' '));
		const char* begin = line.c_str();
		char* end = nullptr;
		long long id;
		while (true) {
			id = std::strtoll(begin, &end, 10);
			begin = end;
			if (id == 0) break;
			Variable v = variables.at(std::abs(id)-1);
			if (id > 0) vars.emplace_back(v);
			else vars.emplace_back(NOT, Formula<Pol>(v));
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
				smatch m;
				if (!regex_match(line, m, headerRegex)) {
					CARL_LOG_ERROR("carl.formula", "DIMACS line starting with \"p\" does not match header format: \"" << line << "\".");
				}
				std::size_t varCount = std::stoull(m[1]);
				while (variables.size() < varCount) {
					variables.push_back(freshBooleanVariable());
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

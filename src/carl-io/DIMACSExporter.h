#pragma once

#include <carl-logging/carl-logging.h>

#include <carl-formula/formula/Formula.h>
#include <carl-formula/formula/functions/CNF.h>

#include <iostream>
#include <map>
#include <vector>


namespace carl::io {

/**
 * Write formulas to the DIMAS format.
 */
template<typename Pol>
class DIMACSExporter {
private:
	std::map<carl::Variable, std::size_t> mVariables;
	std::vector<std::vector<long long>> mClauses;
	
	std::size_t id(carl::Variable::Arg v) {
		auto it = mVariables.find(v);
		if (it != mVariables.end()) return it->second;
		std::size_t res = mVariables.size() + 1;
		mVariables.emplace(v, res);
		return res;
	}
	
	long long getLiteral(const Formula<Pol>& f) {
		if (f.type() == BOOL) {
			return (long long)(id(f.boolean()));
		}
		if (f.type() == NOT) {
			return -getLiteral(f.subformula());
		}
		CARL_LOG_ERROR("carl.dimacs", "Formula is not in pure-boolean CNF: " << f);
		return 0;
	}
	
	bool addDisjunction(const Formula<Pol>& f) {
		if (f.type() == BOOL || f.type() == NOT) {
			long long lit = getLiteral(f);
			if (lit == 0) return false;
			mClauses.emplace_back(1, lit);
			return true;
		}
		if (f.type() == OR) {
			std::vector<long long> clause;
			for (const auto& sub: f) {
				if (sub.type() == BOOL || sub.type() == NOT) {
					long long lit = getLiteral(sub);
					if (lit == 0) return false;
					clause.push_back(lit);
				} else {
					CARL_LOG_ERROR("carl.dimacs", "Added formula to DIMACSExporter has a clause that is not pure-boolean: " << f);
					return false;
				}
			}
			mClauses.push_back(std::move(clause));
			return true;
		}
		CARL_LOG_ERROR("carl.dimacs", "Added formula to DIMACSExporter has a clause that is not pure-boolean: " << f);
		return false;
	}
	
public:
	bool operator()(const Formula<Pol>& formula) {
		Formula<Pol> f = carl::to_cnf(formula);
		if (f.type() == TRUE) {
			CARL_LOG_INFO("carl.dimacs", "Added TRUE to DIMACSExporter. Skipping...");
			return true;
		}
		if (f.type() == FALSE) {
			CARL_LOG_WARN("carl.dimacs", "Added FALSE to DIMACSExporter. Skipping...");
			return true;
		}
		if (f.type() == OR || f.type() == BOOL || f.type() == NOT) {
			return addDisjunction(f);
		}
		if (f.type() == AND) {
			for (const auto& sub: f) {
				if (!addDisjunction(sub)) return false;
			}
			return true;
		}
		CARL_LOG_ERROR("carl.dimacs", "Added formula to DIMACSExporter that is not convertible to pure-boolean cnf: " << formula);
		return false;
	}
	void clear() {
		mVariables.clear();
		mClauses.clear();
	}
	template<typename P>
	friend std::ostream& operator<<(std::ostream& os, const DIMACSExporter<P>& de) {
		os << "p cnf " << de.mVariables.size() << " " << de.mClauses.size() << std::endl;
		for (const auto& clause: de.mClauses) {
			for (const auto& l: clause) os << l << " ";
			os << "0" << std::endl;
		}
		return os;
	}
};
	
}

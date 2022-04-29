/**
 * @file CodeWriter.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <iostream>
#include <set>
#include <utility>
#include <vector>


#include "../core/Monomial.h"
#include "../util/debug.h"

namespace carl {

struct ConstructorPrinter {
	void operator()(std::ostream& os, const mpq_class& n) {
		os << "mpq_class(" << carl::getNum(n) << ")";
		os << "/";
		os << "mpq_class(" << carl::getDenom(n) << ")";
	}
	#ifdef USE_CLN_NUMBERS
	void operator()(std::ostream& os, const cln::cl_RA& n) {
		os << "cln::cl_RA(" << carl::getNum(n) << ")";
		os << "/";
		os << "cln::cl_RA(" << carl::getDenom(n) << ")";
	}
    #endif
	void operator()(std::ostream& os, const Variable& v) {
		os << "Variable(" << v.id() << ")";
	}
	void operator()(std::ostream& os, const Monomial::Arg& m) {
		os << "std::make_shared<const Monomial>(std::initializer_list<std::pair<Variable, exponent>>({";
		bool first = true;
		for (const auto& p: *m) {
			if (!first) os << ", ";
			first = false;
			os << "std::make_pair(";
			(*this)(os, p.first);
			os << "," << p.second << ")";
		}
		os << "}))";
	}
	template<typename C>
	void operator()(std::ostream& os, const Term<C>& t) {
		os << "Term<C>(";
		(*this)(os, t.coeff());
		if (t.monomial()) {
			os << ", ";
			(*this)(os, t.monomial());
		}
		os << ")";
	}
	template<typename C, typename O, typename P>
	void operator()(std::ostream& os, const MultivariatePolynomial<C,O,P>& p) {
		os << "MultivariatePolynomial<C>({";
		bool first = true;
		for (const auto& t: p) {
			if (!first) os << ", ";
			first = false;
			(*this)(os, t);
		}
		os << "})";
	}
};

template<typename T1, typename T2>
class GeneratorWriter {
private:
	std::set<T1> mLhs;
	std::set<T2> mRhs;
	std::string mClassname;
	typedef typename std::set<T1>::const_iterator LIT;
	typedef typename std::set<T2>::const_iterator RIT;
	std::vector<std::tuple<LIT,RIT>> calls;
	
	struct less {
		template<typename IT>
		bool operator()(const IT& l, const IT& r) const {
			return (&*l) < (&*r);
		}
	};
	
	template<typename T>
	std::map<typename std::set<T>::const_iterator,std::size_t,less> print(std::ostream& os, const std::string& name, const std::set<T>& values) const {
		ConstructorPrinter cp;
		std::map<typename std::set<T>::const_iterator,std::size_t,less> res;
		std::size_t next = 0;
		for (auto it = values.cbegin(); it != values.cend(); it++) {
			os << "\t\t" << name << ".push_back(";
			cp(os, *it);
			os << ");" << std::endl;
			res.emplace(it, next);
			next++;
		}
		return res;
	}
	
	void printMember(std::ostream& os) const {
		os << "\tstd::vector<" << typeString<T1>() << "> lhs;" << std::endl;
		os << "\tstd::vector<" << typeString<T2>() << "> rhs;" << std::endl;
		os << "\tstd::vector<std::tuple<std::size_t,std::size_t>> calls;" << std::endl;
		os << "\tstd::size_t nextCall;" << std::endl;
	}
	
	void printInitialization(std::ostream& os) const {
		auto lhsmap = print(os, "lhs", mLhs);
		auto rhsmap = print(os, "rhs", mRhs);
		for (const auto& c: calls) {
			std::size_t lid = lhsmap.at(std::get<0>(c));
			std::size_t rid = rhsmap.at(std::get<1>(c));
			os << "\t\tcalls.emplace_back(" << lid << ", " << rid << ");" << std::endl;
		}
		os << "\t\tnextCall = 0;" << std::endl;
	}
	
	void printGeneration(std::ostream& os) const {
		os << "\t\tassert(nextCall < calls.size());" << std::endl;
		os << "\t\tstd::size_t lid = std::get<0>(calls.at(nextCall));" << std::endl;
		os << "\t\tstd::size_t rid = std::get<1>(calls.at(nextCall));" << std::endl;
		os << "\t\tnextCall++;" << std::endl;
		os << "\t\treturn std::make_tuple(lhs.at(lid), rhs.at(rid));" << std::endl;
	}
	
public:
	GeneratorWriter(const std::string& classname): mClassname(classname) {}

	void addCall(const T1& lhs, const T2& rhs) {
		auto lit = mLhs.insert(lhs);
		auto rit = mRhs.insert(rhs);
		calls.emplace_back(lit.first, rit.first);
	}

	template<typename TL, typename TR>
	friend std::ostream& operator<<(std::ostream& os, const GeneratorWriter<TL,TR>& gw) {
		os << "template<typename C>" << std::endl;
		os << "struct " << gw.mClassname << ": public carl::BaseGenerator {" << std::endl;
		os << "\ttypedef std::tuple<" << typeString<TL>() << "," << typeString<TR>() << "> type;" << std::endl;
		os << "\tstatic const std::size_t size = " << gw.calls.size() << ";" << std::endl;
		gw.printMember(os);
		os << "\t" << gw.mClassname << "(const carl::BenchmarkInformation& bi): BaseGenerator(bi) {" << std::endl;
		gw.printInitialization(os);
		os << "\t}" << std::endl;
		os << "\ttype operator()() {" << std::endl;
		gw.printGeneration(os);
		os << "\t}" << std::endl;
		os << "};" << std::endl;
		return os;
	}
};

}

/**
 * @file BenchmarkGenerator.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "Common.h"

namespace carl {

enum class BenchmarkSelection: unsigned {
	Random
};

struct BenchmarkInformation {
	BenchmarkSelection selection;
	std::size_t n = 100;
	std::size_t degree = 5;
	std::vector<std::size_t> degrees;
	bool compareResults = false;
	std::vector<carl::Variable> variables;
	
	BenchmarkInformation(BenchmarkSelection bs, std::size_t variableCount)
		: selection(bs) {
		for (std::size_t i = 0; i < variableCount; i++) {
			variables.push_back(carl::freshRealVariable("x" + std::to_string(i)));
		}
	}
};

struct ObjectGenerator {
protected:
	mutable std::mt19937 rand;
	mutable std::geometric_distribution<> geom;
public:
	BenchmarkInformation bi;
	ObjectGenerator(const BenchmarkInformation& bi):
		rand(4), /* chosen by fair dice roll. guaranteed to be random. */
		bi(bi)
	{}

	ObjectGenerator(const BenchmarkInformation& bi, unsigned long seed):
		rand(seed), /* chosen by fair dice roll. guaranteed to be random. */
		bi(bi)
	{	}
	
	/// integers: [0, ...]
	template<typename T>
	T geomDist() const {
		return T(geom(rand)) + T(1);
	}
    
	/// integers: [0, ..., mod-1]
	std::size_t uniDist(std::size_t mod) const {
		return rand() % mod;
	}
    
	carl::Variable randomVariable() const {
		return bi.variables[uniDist(bi.variables.size())];
	}
    
	std::shared_ptr<const carl::Monomial> randomMonomial(std::size_t degree) const {
		Monomial::Arg res;
		for (unsigned d = 1; d < degree; d++) {
            res = res * randomVariable();
		}
		return res;
	}
    
	template<typename C>
	carl::Term<C> randomTerm(std::size_t degree) const {
		return carl::Term<C>(geomDist<C>(), randomMonomial(degree));
	}
    
	template<typename C>
	CMP<C> newMP() const {
		return newMP<C>(bi.degree);
	}
    
	template<typename C>
	CMP<C> newMP(std::size_t deg) const {
		auto& manager = carl::MultivariatePolynomial<C>::mTermAdditionManager;
		auto id = manager.getId(deg*deg*deg);
		C c = C(geomDist<C>());
		manager.template addTerm<true>(id, Term<C>(c));
		for (std::size_t i = 1; i <= deg; i++) {
			std::binomial_distribution<> bin((int)((deg-i)*(deg-i)), 0.5);
			std::size_t num = (std::size_t)bin(rand) + 1;
			for (std::size_t j = 0; j < num; j++) {
				manager.template addTerm<true>(id, randomTerm<C>(i));
			}
		}
		std::vector<Term<C>> terms;
		manager.readTerms(id, terms);
		return carl::MultivariatePolynomial<C>(terms);
	}
    
	template<typename C>
	CUMP<C> newUMP() const {
		return newMP<C>().toUnivariatePolynomial(randomVariable());
	}
    
	template<typename C>
	CUMP<C> newUMP(carl::Variable::Arg v) const {
		return newMP<C>().toUnivariatePolynomial(v);
	}
};

struct BaseGenerator {
protected:
	BenchmarkInformation bi;
	ObjectGenerator g;
public:
	typedef void type;
	BaseGenerator(const BenchmarkInformation& bi): bi(bi), g(bi) {}
	void operator()() const {}
};

}

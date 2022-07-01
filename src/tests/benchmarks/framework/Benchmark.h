/**
 * @file Benchmark.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <functional>
#include <iterator>
#include <random>
#include <tuple>
#include <vector>
#include <utility>


#include "../config.h"
#include <carl-common/debug/Timer.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>

#include "BenchmarkConversions.h"
#include "BenchmarkGenerator.h"
#include "BenchmarkOutput.h" 

namespace carl {

template<typename Generator>
class BenchmarkGenerator {
public:
	typedef typename Generator::type Sample;
	typedef std::vector<Sample> Samples;
private:
	Samples samples;
	ObjectGenerator generator;
	CIPtr ci;
public:
	BenchmarkGenerator(const BenchmarkInformation& bi, const CIPtr& ci): generator(bi), ci(ci) {
		std::cout << "Generating " << bi.n << " examples ... ";
		std::cout.flush();
		Generator g(bi);
		carl::Timer timer;
		for (std::size_t i = 0; i < bi.n; i++) {
			samples.emplace_back(g());
		}
		std::cout << timer.passed() << " ms" << std::endl;
	}
	typename Samples::const_iterator begin() const {
		return samples.begin();
	}
	typename Samples::const_iterator end() const {
		return samples.end();
	}
	std::size_t size() const {
		return samples.size();
	}
	CIPtr getCI() const {
		return ci;
	}
};

template<typename Converter>
class BenchmarkConverter {
public:
	typedef std::vector<typename Converter::type> Samples;
private:
	Samples samples;
public:
	template<typename Generator>
	BenchmarkConverter(const BenchmarkGenerator<Generator>& bg) {
		Converter conv(bg.getCI());
		//std::cout << "Converting ... ";
		//std::cout.flush();
		carl::Timer timer;
		for (auto it: bg) {	
			samples.emplace_back(conv(it));
		}
		//std::cout << timer.passed() << " ms" << std::endl;
	}
	typename Samples::const_iterator begin() const {
		return samples.begin();
	}
	typename Samples::const_iterator end() const {
		return samples.end();
	}
};

template<typename T> 
class BenchmarkResultComparator {
private:
	CIPtr ci;
	std::vector<std::pair<T, T>> results;
	std::pair<std::string, std::string> names;
protected:
	template<typename Coeff>
	bool operator()(const CMP<Coeff>& lhs, const CMP<Coeff>& rhs) {
		return lhs == rhs;
	}
	bool operator()(const bool lhs, const bool rhs) {
		return lhs == rhs;
	}
	#ifdef USE_COCOA
	bool operator()(const CoMP& lhs, const CoMP& rhs) {
		return lhs == rhs;
	}
	#endif
    #ifdef USE_GINAC
	bool operator()(const GMP& lhs, const GMP& rhs) {
		return lhs == rhs;
	}
    #endif
public:
	BenchmarkResultComparator(const std::string& lname, const std::string& rname, const CIPtr& ci): ci(ci), names(lname, rname) {}
	template<typename T2>
	void put(const T& t, const T2& t2) {
		results.emplace_back(t, Conversion::template convert<T>(t2, ci));
	}
	void check() {
		for (auto it: results) {
			if ((*this)(it.first, it.second)) {
				continue;
			}
			std::cout << "Results differ:" << std::endl;
			std::cout << names.first << "\t" << it.first << std::endl;
			std::cout << names.second << "\t" << it.second << std::endl;
			exit(0);
		}
	}
};

template<typename Generator, typename Executor, typename Result>
class Benchmark {
private:
	CIPtr ci;
	BenchmarkGenerator<Generator> reference;
	BenchmarkInformation bi;
	std::vector<Result> results;
	Executor executor;
	BenchmarkResult runtimes;
	std::string name;

	template<typename R, typename Src>
	std::size_t runSamples(std::vector<R>& res, const Src& src) {
		carl::Timer timer;
		for (const auto& cur: src) {
			res.emplace_back(executor(cur));
		}
		return timer.passed();
	}
public:
	Benchmark(const BenchmarkInformation& bi, const std::string& name): ci(new ConversionInformation(bi.variables)), reference(bi, ci), bi(bi), name(name) {
		results.reserve(reference.size());
		std::cout << "Reference " << name << " ... ";
		std::cout.flush();
		std::size_t time = runSamples(results, reference);
		runtimes[name] = time;
		std::cout << time << " ms" << std::endl;
	}
	template<typename R, typename Converter>
	void compare(const std::string& name) {
		std::vector<R> res;
		res.reserve(reference.size());
		BenchmarkConverter<Converter> benchmarks(reference);
		std::cout << "Comparing " << name << " ... ";
		std::cout.flush();
		std::size_t time = runSamples(res, benchmarks);
		runtimes[name] = time;
		std::cout << time << " ms" << std::endl;
		
		if (bi.compareResults) {
			BenchmarkResultComparator<R> c(name, this->name, ci);
			for (std::size_t i = 0; i < results.size(); i++) {
				c.put(res[i], results[i]);
			}
			c.check();
		}
	}
	BenchmarkResult result() const {
		return runtimes;
	}
	CIPtr& getCI() {
		return ci;
	}
};

}

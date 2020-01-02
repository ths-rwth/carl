/** 
 * @file:   BenchmarkTest.h
 * @author: Sebastian Junges
 *
 * @since November 16, 2014
 */

#pragma once

#include "gtest/gtest.h"

#include "framework/Benchmark.h"
#include "framework/BenchmarkOutput.h"

#include <fstream>

namespace carl 
{
class BenchmarkTest : public ::testing::Test {
protected:
    
	BenchmarkTest(): file({
		"CArL"
#ifdef USE_COCOA
		,"CoCoA"
#endif
#ifdef USE_GINAC
		,"GiNaC"
#endif
#ifdef COMPARE_WITH_Z3
		,"Z3"
#endif
    }) {
	}
	~BenchmarkTest() {
		auto info = ::testing::UnitTest::GetInstance()->current_test_info();
        {
            std::stringstream ss;
            ss << "benchmarks/benchmark_" << info->name() << ".tex";
            std::ofstream out(ss.str(), std::ios_base::out);
			file.writeMain(out, info->name());
        }
		{
			std::stringstream ss;
			ss << "benchmarks/benchmark_" << info->name() << "_plot.tex";
			std::ofstream out(ss.str(), std::ios_base::out);
			file.writePlot(out, info->name());
		}
		{
			std::stringstream ss;
			ss << "benchmarks/benchmark_" << info->name() << "_data.tex";
			std::ofstream out(ss.str(), std::ios_base::out);
			file.writePlotData(out, info->name());
		}
		{
			std::stringstream ss;
			ss << "benchmarks/benchmark_" << info->name() << "_table.tex";
			std::ofstream out(ss.str(), std::ios_base::out);
			file.writeTable(out);
		}
	}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
	BenchmarkFile<std::size_t> file;
};

}

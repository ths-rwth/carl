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

namespace carl 
{
class BenchmarkTest : public ::testing::Test {
protected:
    
	BenchmarkTest(): file({
        "CArL" 
        #ifdef COMPARE_WITH_GINAC
        ,"GiNaC"
        #endif
        #ifdef COMPARE_WITH_Z3
        ,"Z3"
        #endif
    }) {
	}
	~BenchmarkTest() {
		auto info = ::testing::UnitTest::GetInstance()->current_test_info();
		std::stringstream ss;
		ss << "benchmark_" << info->name() << ".tex";
		std::ofstream out(ss.str(), std::ios_base::out);
		out << file;
	}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
	BenchmarkFile<std::size_t> file;
};

}
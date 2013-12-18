#include "gtest/gtest.h"

#include <cln/cln.h>

#include "carl/cad/SampleSet.h"

using namespace carl;

TEST(SampleSet, BasicOperations)
{  
	cad::SampleSet<cln::cl_RA> s;
	
	s.pop();
}

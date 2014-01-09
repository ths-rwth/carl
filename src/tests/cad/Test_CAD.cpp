#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <cln/cln.h>

#include "carl/cad/CAD.h"

using namespace carl;

TEST(CAD, BasicOperations)
{  
	carl::CAD<cln::cl_RA> cad;
	cad.samples();
	cad.complete();
}

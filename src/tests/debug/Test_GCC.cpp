#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <cln/cln.h>
#include <vector>
#include <forward_list>

#include "carl/core/logging.h"
#include "carl/core/RealAlgebraicNumber.h"
#include "carl/cad/Constraint.h"
#include "carl/cad/CAD.h"

using namespace carl;

TEST(GCC, BasicOperations)
{
	carl::CAD<cln::cl_RA> cad;
	cad.complete();
	
	carl::RealAlgebraicPoint<cln::cl_RA> r;
	std::vector<carl::cad::Constraint<cln::cl_RA>> c;
	cad::ConflictGraph cg;
	std::unordered_map<unsigned, ExactInterval<cln::cl_RA>> boundMap;
	std::list<std::pair<std::list<cad::Constraint<cln::cl_RA>>, std::list<cad::Constraint<cln::cl_RA>>>> deductions;
	carl::cad::SampleSet<cln::cl_RA> s;
	std::forward_list<RealAlgebraicNumber<cln::cl_RA>*> lst;
	
	//cad.liftCheck(nullptr, {}, 0, true, {}, c, boundMap, true, true, r, cg);
	cad.samples(nullptr, {}, {}, s, lst, carl::ExactInterval<cln::cl_RA>::emptyExactInterval(), cad.getSetting());
}

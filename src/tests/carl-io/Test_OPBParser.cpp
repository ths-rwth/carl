#include "gtest/gtest.h"

#include "../Common.h"

#include <carl-io/OPBImporter.h>

using namespace carl;
using Poly = carl::MultivariatePolynomial<mpq_class>;

TEST(OPBParser, Basic)
{
	//io::OPBImporter<Poly> importer("../sep6.5.opb");
	//importer.parse();
}

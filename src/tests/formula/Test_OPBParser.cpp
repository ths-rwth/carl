#include "gtest/gtest.h"

#include "../Common.h"

#include <carl/formula/parser/OPBImporter.h>

using namespace carl;
using Poly = carl::MultivariatePolynomial<mpq_class>;

TEST(OPBParser, Basic)
{
	//OPBImporter<Poly> importer("../sep6.5.opb");
	//importer.parse();
}

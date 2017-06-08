#include "carl/io/WriteTosmt2Stream.h"
#include "carl/util/stringparser.h"
#include "carl/core/Relation.h"
#include "carl/formula/Constraint.h"

using namespace carl;

int main (int, char**)
{
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    MultivariatePolynomial<mpq_class> p1 =  sp.parseMultivariatePolynomial<mpq_class>("x^2 + y^2");
    MultivariatePolynomial<mpq_class> p2 =  sp.parseMultivariatePolynomial<mpq_class>("x^2 + -1*y^2");
    std::cout << p1 << std::endl;
    io::WriteTosmt2Stream smt2;
    //smt2 << io::smt2flag::ASSERT;
    smt2 << io::smt2node::AND;
    smt2 << Constraint<MultivariatePolynomial<mpq_class>>(p1, Relation::GEQ);
    smt2 << Constraint<MultivariatePolynomial<mpq_class>>(p2, Relation::EQ);
    smt2 << io::smt2node::CLOSENODE;
    //smt2 << io::smt2flag::ASSERT;
    smt2 << Constraint<MultivariatePolynomial<mpq_class>>(p2, Relation::GEQ);
    smt2 << io::smt2flag::CHECKSAT;
    
    
    std::cout << smt2;
}

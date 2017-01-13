from pycarl import *
import unittest

class TestCarl(unittest.TestCase):
	__v1 = Variable('x',VariableType.VT_REAL)
	__v2 = Variable(VariableType.VT_INT)
	__m = Monomial.createMonomial(Variable('x',VariableType.VT_REAL),3)
	__t = TermRational(Rational(5.5),m)
	__p1 = Polynomial(Monomial.createMonomial(v1))
	__p2 = Polynomial(m)
	__p3 = Polynomial(v1)
	__p4 = Polynomial(Rational(-1))
	__f = RationalFunction(p1,p4)

	__v11 = Variable('v1',VariableType.VT_REAL)	
	__v21 = Variable('v2',VariableType.VT_REAL)	

	__c1 = ConstraintRational(true)
	__c2 = ConstraintRational(v11,Relation.EQ,Rational(10))
	__c3 = ConstraintRational(v21,Relation.GEQ)

	__x = Variable('y',VariableType.VT_BOOL)
	__f1 = FormulaPolynomial(x)
	__f2 = FormulaPolynomial(c2)
	__f3 = FormulaPolynomial(FormulaType.NOT,f2)
	__subformulas = FormulaVector()
	subformulas.add(f1)
	subformulas.add(f2)
	__f4 = FormulaPolynomial(FormulaType.AND,subformulas)

	__cache = FactorizationCache()
	__fp1 = FactorizedPolynomialRational(Rational(2))
	__fp2 = FactorizedPolynomialRational(p1,cache)


	def testShit(self):
		self.assertTrue(False)




}



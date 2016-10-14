

from pycarl import *
import unittest

class TestPolynomial(TestCarl):


	def testToString(self):
		self.assertEqual(p1.toString(),'x')

	def testGetters(self):
		self.assertTrue(p1.constantPart().equals(Rational(0)))
		self.assertEquals(p1.totalDegree(),1)
		self.assertEquals(p1.degree(v1),1)
		self.assertEquals(p1.degree(v2),0)
		self.assertEquals(p1.nrTerms(),1)
		self.assertEquals(p1.size(),1)

	def testEvaluate(self):
		map = VarRationalMap()
		map.set(v1,Rational(2))
		self.assertTrue(p3.evaluate(map).equals(Rational(2)))


	def testComparisons(self):
		self.assertEqual(p1.equals(p1),true)
		self.assertEqual(p1.equals(p2),false)
		self.assertEqual(p1.notEquals(p1),false)
		self.assertEqual(p1.notEquals(p2),true)



	def testOperators(self):
		self.assertEqual(p1.add(v1).toString(),'2*x')
		self.assertEqual(p1.add(m).toString(),'x^3+x')
		self.assertEqual(p1.add(t).toString(),'11/2*x^3+x')
		self.assertEqual(p1.add(p1).toString(),'2*x')
		self.assertEqual(p1.add(Rational(-5.5)).toString(),'x+(-11/2)')

		self.assertEqual(p1.sub(v1).toString(),'0')
		self.assertEqual(p1.sub(m).toString(),'(-1)*x^3+x')
		self.assertEqual(p1.sub(t).toString(),'(-11/2)*x^3+x')
		self.assertEqual(p1.sub(p1).toString(),'0')
		self.assertEqual(p1.sub(Rational(-5.5)).toString(),'x+11/2')

		self.assertEqual(p1.mul(v1).toString(),'x^2')
		self.assertEqual(p1.mul(m).toString(),'x^4')
		self.assertEqual(p1.mul(t).toString(),'11/2*x^4')
		self.assertEqual(p1.mul(p1).toString(),'x^2')
		self.assertEqual(p1.mul(Rational(-5.5)).toString(),'(-11/2)*x')

		self.assertEqual(p1.div(v1).toString(),'1')
		self.assertEqual(p1.div(m).toString(),'(x)/(x^3)')
		self.assertEqual(p1.div(t).toString(),'(2*x)/(11*x^3)')
		self.assertEqual(p1.div(p1).toString(),'1')
		self.assertEqual(p1.div(Rational(-5.5)).toString(),'(-2/11)*x')
		self.assertEqual(p1.div(f).toString(),'((-1)*x)/(x)')

		self.assertEqual(p1.neg().toString(),'(-1)*x')
		self.assertEqual(p1.pow(20).toString(),'x^20')



}



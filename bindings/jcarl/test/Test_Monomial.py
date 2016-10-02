from pycarl import *
import unittest

class TestMonomial(TestCarl):


	def testToString(self):
		self.assertEqual(m.toString(),'x^3')


	def testGetters(self):
		self.assertEqual(m.nrVariables(),1)
		self.assertEqual(m.tdeg(),3)

	def testExponentsGetItem(self): 
		vector = m.exponents()
		self.assertTrue(vector.get(0).getFirst().equals(v1))
		self.assertEqual(vector.get(0).getSecond(),3)
		self.assertTrue(m.getItem(0).getFirst().equals(v1))
		self.assertEqual(m.getItem(0).getSecond(),3)
	
	


	def testOperators(self):
		self.assertEqual(m.add(v1).toString(),'x^3+x')
		self.assertEqual(m.add(m).toString(),'2*x^3')
		self.assertEqual(m.add(t).toString(),'13/2*x^3')
		self.assertEqual(m.add(p1).toString(),'x^3+x')
		self.assertEqual(m.add(Rational(-5.5)).toString(),'x^3+(-11/2)')

		self.assertEqual(m.sub(v1).toString(),'x^3+(-1)*x')
		self.assertEqual(m.sub(m).toString(),'(-1)*x^3+x^3')
		self.assertEqual(m.sub(t).toString(),'(-11/2)*x^3+x^3')
		self.assertEqual(m.sub(p1).toString(),'x^3+(-1)*x')
		self.assertEqual(m.sub(Rational(-5.5)).toString(),'x^3+11/2')

		self.assertEqual(m.mul(v1).toString(),'x^4')
		self.assertEqual(m.mul(m).toString(),'x^6')
		self.assertEqual(m.mul(t).toString(),'11/2*x^6')
		self.assertEqual(m.mul(p1).toString(),'x^4')
		self.assertEqual(m.mul(Rational(-5.5)).toString(),'(-11/2)*x^3')

		self.assertEqual(m.div(v1).toString(),'(x^3)/(x)')
		self.assertEqual(m.div(m).toString(),'(x^3)/(x^3)')
		self.assertEqual(m.div(t).toString(),'(2*x^3)/(11*x^3)')
		self.assertEqual(m.div(p1).toString(),'(x^3)/(x)')
		self.assertEqual(m.div(Rational(-5.5)).toString(),'(-2/11)*x^3')
		self.assertEqual(m.div(f).toString(),'((-1)*x^3)/(x)')

		self.assertEqual(m.neg().toString(),'(-1)*x^3')
		self.assertEqual(m.pow(20).toString(),'x^60')
}


























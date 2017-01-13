from pycarl import *
import unittest

class TestTerm(TestCarl):


	def testToString(self):
		self.assertEqual(t.toString(),'11/2*x^3')

	def testGetters(self):
		self.assertTrue(t.coeff().equals(Rational(5.5)))
		self.assertEqual(t.monomial().toString(),'x^3')



	def testOperators(self):
		self.assertEqual(t.add(v1).toString(),'11/2*x^3+x')
		self.assertEqual(t.add(m).toString(),'13/2*x^3')
		self.assertEqual(t.add(t).toString(),'11*x^3')
		self.assertEqual(t.add(p1).toString(),'11/2*x^3+x')
		self.assertEqual(t.add(Rational(-5.5)).toString(),'11/2*x^3+(-11/2)')

		self.assertEqual(t.sub(v1).toString(),'11/2*x^3+(-1)*x')
		self.assertEqual(t.sub(m).toString(),'9/2*x^3')
		self.assertEqual(t.sub(t).toString(),'0')
		self.assertEqual(t.sub(p1).toString(),'11/2*x^3+(-1)*x')
		self.assertEqual(t.sub(Rational(-5.5)).toString(),'11/2*x^3+11/2')

		self.assertEqual(t.mul(v1).toString(),'11/2*x^4')
		self.assertEqual(t.mul(m).toString(),'11/2*x^6')
		self.assertEqual(t.mul(t).toString(),'121/4*x^6')
		self.assertEqual(t.mul(p1).toString(),'11/2*x^4')
		self.assertEqual(t.mul(Rational(-5.5)).toString(),'(-121/4)*x^3')

		self.assertEqual(t.div(v1).toString(),'(11*x^3)/(2*x)')
		self.assertEqual(t.div(m).toString(),'(11*x^3)/(2*x^3)')
		self.assertEqual(t.div(t).toString(),'1')
		self.assertEqual(t.div(p1).toString(),'(11*x^3)/(2*x)')
		self.assertEqual(t.div(Rational(-5.5)).toString(),'(-1)*x^3')
		self.assertEqual(t.div(f).toString(),'((-11)*x^3)/(2*x)')

		self.assertEqual(t.neg().toString(),'(-11/2)*x^3')
		self.assertEqual(t.pow(20).toString(),'672749994932560009201/1048576*x^60')

}














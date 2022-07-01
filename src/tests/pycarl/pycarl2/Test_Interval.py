from pycarl import *
import unittest

class TestInterval(TestCarl):

	i1 = IntervalRational(Rational(0))
	i2 = IntervalRational(Rational(-100),BoundType.WEAK,Rational(100),BoundType.INFTY)


	def testToString(self):
		self.assertEqual(i1.toString(),'[0, 0]')


	def testGetters(self):
		self.assertTrue(i1.lower().equals(Rational(0)))
		self.assertTrue(i1.upper().equals(Rational(0)))

	
	def testIsMethods(self):
		self.assertEqual(i1.is_infinite(),false)
		self.assertEqual(i1.is_unbounded(),false)
		self.assertEqual(i1.is_half_bounded(),false)
		self.assertEqual(i1.is_empty(),false)
		self.assertEqual(i1.is_point_interval(),true)
		self.assertEqual(i1.is_open_interval(),false)
		self.assertEqual(i1.is_closed_interval(),true)
		self.assertEqual(i1.is_zero(),true)
		self.assertEqual(i1.is_one(),false)
		self.assertEqual(i1.is_positive(),false)
		self.assertEqual(i1.is_negative(),false)
		self.assertEqual(i1.is_semi_positive(),true)
		self.assertEqual(i1.is_semi_negative(),true)


	def testSetBounds(self):
		i = IntervalRational(Rational(-5),Rational(5))
		i.set_lower(Rational(-1))
		i.set_upper(Rational(1))
		self.assertTrue(i.lower().equals(Rational(-1)))
		self.assertTrue(i.upper().equals(Rational(1)))


	def testSomeMoreMethods(self):
		self.assertTrue(i1.integral_part().lower().equals(i1.lower()))
		self.assertTrue(i1.integral_part().upper().equals(i1.upper()))
		self.assertTrue(i1.diameter().equals(Rational(0)))
		self.assertTrue(i1.center().equals(Rational(0)))
		self.assertTrue(i1.sample().equals(Rational(0)))


	
	def testRelationalOperators(self):
		self.assertEqual(i1.contains(Rational(1000)),false)
		self.assertEqual(i2.contains(i1),true)
		self.assertEqual(i1.meets(Rational(1)),false)
		self.assertEqual(i1.isSubset(i2),true)
		self.assertEqual(i1.isProperSubset(i2),true)
		self.assertEqual(i1.intersectsWith(i2),true)

	

	def testSomeMoreOperators(self):
		result = i2.div(i1)
		self.assertTrue(result.lower().equals(Rational(0)))
		self.assertTrue(result.upper().equals(Rational(0)))
		result = i2.inverse()
		self.assertTrue(result.upper().equals(Rational(100)))
		self.assertEqual(result.is_half_bounded(),true)
		result = i2.intersect(i1)
		self.assertTrue(result.lower().equals(Rational(0)))
		self.assertTrue(result.upper().equals(Rational(0)))
		result = i2.abs()
		self.assertTrue(result.lower().equals(Rational(0)))
		self.assertTrue(result.upper().equals(Rational(0)))
		result = i1.pow(2)
		self.assertTrue(result.lower().equals(Rational(0)))
		self.assertTrue(result.upper().equals(Rational(0)))
	

	def test2ResultsMethods(self):
		res1 = IntervalRational(Rational(0))
		res2 = IntervalRational(Rational(0))
		i1.unite(i2,res1,res2)
		self.assertTrue(res1.lower().equals(i2.lower()))
		self.assertTrue(res1.upper().equals(i2.upper()))
		i2.difference(i1,res1,res2);
		self.assertTrue(res1.lower().equals(i2.lower()))
		self.assertTrue(res1.upper().equals(Rational(0)))
		self.assertTrue(res2.is_half_bounded())
		self.assertTrue(res2.lower().equals(Rational(0)))
		i1.complement(res1,res2)
		self.assertTrue(res1.upper().equals(Rational(0)))
		self.assertTrue(res2.lower().equals(Rational(0)))
		self.assertEqual(res1.is_half_bounded(),true)
		self.assertEqual(res2.is_half_bounded(),true)
		i2.symmetricDifference(i1,res1,res2)
		self.assertTrue(res1.lower().equals(i2.lower()))
		self.assertTrue(res1.upper().equals(Rational(0)))
		self.assertTrue(res2.is_half_bounded())
		self.assertTrue(res2.lower().equals(Rational(0)))
	

	def testStaticMethods(self):
		result = IntervalRational.floor(i1);
		self.assertTrue(result.lower().equals(Rational(0)))
		self.assertTrue(result.upper().equals(Rational(0)))
		result = IntervalRational.ceil(i1)
		self.assertTrue(result.lower().equals(Rational(0)))
		self.assertTrue(result.upper().equals(Rational(0)))
		result = IntervalRational.quotient(i2,i1)
		self.assertTrue(result.lower().equals(Rational(0)))
		self.assertTrue(result.upper().equals(Rational(0)))
		self.assertEquals(IntervalRational.is_integer(i2),false)

}
























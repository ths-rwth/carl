from pycarl import *
import unittest

class TestVariable(unittest.TestCase):
	def testEquals(self):
		v1 = Variable(VariableType.INT)
		v2 = Variable(VariableType.REAL)
		self.assertEqual(VariableType.INT, v1.type)
		self.assertEqual(VariableType.REAL, v2.type)
		self.assertTrue(v1 != v2)
		self.assertTrue(v1 == v1)
	
	def testComparison(self):
		pass

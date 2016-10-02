



import glob
import unittest

test_files = glob.glob('./*.py')
print "Testfiles: "+'[%s]' % ', '.join(map(str, test_files))
module_strings = [test_file[0:len(test_file)-3] for test_file in test_files]
suites = [unittest.defaultTestLoader.loadTestsFromName(test_file) for test_file in module_strings]
test_suite = unittest.TestSuite(suites)
test_runner = unittest.TextTestRunner().run(test_suite)

#loader = unittest.TestLoader()

#tests = loader.discover('./test/', pattern = 'Test*.py')

#runner = unittest.TextTestRunner()
#runner.run(tests)

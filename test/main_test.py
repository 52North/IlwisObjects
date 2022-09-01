# content of test_sample.py

import unittest
import basetest as bt
import config
import time

config.errorCount = 0

#time.sleep(10)

loader = unittest.TestLoader()
tests = loader.discover('.','test_*.py')

testRunner = unittest.runner.TextTestRunner()
testRunner.run(tests)

print("\nnumber of tests : " + str(config.testCount))
print("number of fails in tests : " + str(len(config.fails)) + "\n")
print(*config.fails, sep='\n')





     
"""
This module tests the functionality of the Sat Solver program
"""
import subprocess
from TestClass import Tester

if __name__ == "__main__":
    subprocess.call(['make -C ../'], shell=True)
print "This tester requires that you have minisat installed."
TESTER = Tester()
TESTER.test_io()
TESTER.test_simple()
TESTER.stress_test()

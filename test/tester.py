"""
This module tests the functionality of the Sat Solver program
"""
import subprocess
from TestClass import Tester

if __name__ == "__main__":
    subprocess.call(['make -C ../'], shell=True)
TESTER = Tester()
TESTER.test_io()
TESTER.test_simple()

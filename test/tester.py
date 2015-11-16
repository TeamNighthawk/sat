"""
This module tests the functionality of the Sat Solver program
"""
import subprocess
import os
from TestClass import Tester

if __name__ == "__main__":
    # change into the directory where this script is contained
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    subprocess.call(['make -C ../'], shell=True)

    print "\nThis tester requires that you have minisat installed.\n"
    TESTER = Tester()
    TESTER.test_io()
    TESTER.test_simple()
    TESTER.stress_test()

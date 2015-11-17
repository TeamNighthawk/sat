"""
This module tests the functionality of the Sat Solver program
"""
import subprocess
import os
import argparse
from TestClass import Tester

if __name__ == "__main__":

    # Create the command-line arguments
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("--nvars", default=10, type=int, help="The maximum number of literals to use.")
    parser.add_argument("--nclauses", default=60, type=int, help="The total number of clauses.")
    parser.add_argument("--benchmark", action='store_true', help="Run the sat-comp benchmarks.")

    # Parse the arguments
    args = parser.parse_args()

    # Change into the directory where this script is contained
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    subprocess.call(['make -C ../'], shell=True)

    print "\nThis tester requires that you have minisat installed.\n"

    t = Tester(args.nvars, args.nclauses)

    if args.benchmark:
        t.run_benchmarks()
    else:
        t.test_io()
        t.test_simple()
        t.stress_test()

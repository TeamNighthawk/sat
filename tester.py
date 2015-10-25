"""
This module tests the functionality of the Sat Solver program
"""
import subprocess
from FileCreator import SatFileInstance

failed_cnt = 0

def run_test(name):
    """
    Runs the sat solver on the name supplied. Also runs the sat solver on the oracle
    and compares the results of the two.
    """
    print ("Testing with file: %s " % name)
    output = subprocess.check_output("./satsolv %s" % name, shell=True)
    expected = ""
    if output != expected:
        global failed_cnt
        failed_cnt += 1
        print ("FAILED TEST: '%s'\n\tEXPECTED: %s\r\tRETURNED: %s\n") % (name, expected, output)

if __name__ == "__main__":
    subprocess.call(['make'], shell=True)

SOLVER = SatFileInstance(5, 3)

filename = SOLVER.create_file()

run_test(filename)

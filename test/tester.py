"""
This module tests the functionality of the Sat Solver program
"""
import subprocess
from multiprocessing.pool import ThreadPool
from FileCreator import SatFileInstance
from subprocess import Popen, PIPE

class Tester(object):
    """
    The tester class will encapsulate all of the work of testing the sat solver.
    It relies on threading to simultaneously run the oracle (minisat) and the
    satsolver program.
    """
    def __init__(self):
        """
        Initialize the tester instance
        """
        self.failed = []
        self.sat = "SATISFIABLE"
        self.unsat = "UNSATISFIABLE"
        self.pool = ThreadPool(processes=2)
    def worker(self, args):
        """
        This is the thread worker function.
        args - contains the arguments needed for running oracle/satsolv
             - of the format ['Program', 'Filename', 'arguments']
        """
        ps =  Popen( args, stdout=PIPE )
        (out, err) = ps.communicate()
        exit_code = ps.wait()
        return out
    def execute(self, filename):
        """
        Executes the threads and compares results.  Failed comparisons
        are added to the list of failed filenames.
        filename - path to the file being tested.
        returns nothing
        """
        oracle_args = ['minisat', filename, '-verb=0']
        sat_args = ['../bin/satsolv', filename]
        oracle = self.pool.apply_async(self.worker, (oracle_args))
        sat = self.pool.apply_async(self.worker, (sat_args))
        oracle_result = oracle.get()
        sat_result = sat.get()
        if self.unsat in oracle_result:
            oracle_result = self.unsat
        elif self.sat in oracle_result:
            oracle_result = self.sat
        if sat_result != oracle_result:
            self.failed.append(filename)

if __name__ == "__main__":
    subprocess.call(['make -C ../'], shell=True)

SOLVER = SatFileInstance(5, 3)

filename = SOLVER.create_file()

tester = Tester()
tester.execute(filename)

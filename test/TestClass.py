"""
This module contains the class that will build and run the tests on satsolv
Author: Dustin Jay Tuckett u0204088
"""
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
        #setup class variables required for multi-threading
        self.pool = ThreadPool(processes=2) #only two processes are ever running at a time
        self.filename = ''
        self.oracle_args = []
        self.sat_args = []
    def worker(self, is_oracle):
        """
        This is the thread worker function.
        args - contains the arguments needed for running oracle/satsolv
             - of the format ['Program', 'Filename', 'arguments']
        """
        args = self.sat_args
        if is_oracle:
            args = self.oracle_args
        command_process = Popen(args, stdout=PIPE)
        (out, err) = command_process.communicate()
        exit_code = command_process.wait()
        return out
    def execute(self, filename):
        """
        Executes the threads and compares results.  Failed comparisons
        are added to the list of failed filenames.
        filename - path to the file being tested.
        returns nothing
        """
        """
        NOTES:  Both programs have the potential of taking a VERY long time to
        run.  For this reason I have decided to run both programs on their own
        thread.  I am not allowed to pass objects into the worker method so I
        needed a way to get all of the appropriate arguments into the worker
        to do this I used class variables to communicate the argument list
        to the worker.
        """
        self.oracle_args = ['minisat', filename, '-verb=0']
        self.sat_args = ['../bin/satsolv', filename]
        oracle = self.pool.apply_async(self.worker, (True,))
        sat = self.pool.apply_async(self.worker, (False,))
        #wait for both to finish and compare results
        oracle_result = oracle.get()
        sat_result = sat.get()
        #minisat prints warning periodically, I only care about the satisfiability
        #UNSATISFIABLE must be checked first!
        if self.unsat in oracle_result:
            oracle_result = self.unsat
        elif self.sat in oracle_result:
            oracle_result = self.sat
        if sat_result != oracle_result:
            self.failed.append(filename)
        print oracle_result
        print sat_result
    def test_simple(self):
        """
        Runs the simple tests, on small files.  Runs several versions of each type of file
        to increase chances of receiving SATISFIABLE and UNSATISFIABLE
        """
        solver = SatFileInstance(5, 3)
        filename = solver.create_file()
        self.execute(filename)

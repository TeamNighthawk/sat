"""
This module contains the class that will build and run the tests on satsolv
Author: Dustin Jay Tuckett u0204088
"""
from multiprocessing.pool import ThreadPool
from FileCreator import SatFileInstance
from subprocess import Popen, PIPE
import random
import os
import datetime

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
        self.output = open("test_results.txt", 'a')
        self.output.write("Test Run for : %s \n" % datetime.datetime.utcnow())
        self.sat = "SATISFIABLE\n"
        self.unsat = "UNSATISFIABLE\n"
        self.err = "ERROR"
        #setup class variables required for multi-threading
        self.pool = ThreadPool(processes=2) #only two processes are ever running at a time
        self.oracle_args = []
        self.sat_args = []
    def worker(self, is_oracle):
        """
        This is the thread worker function.
        The threads cannot accept objects as arguments, so class variables
        are used to communicate.
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
        Executes the threads and compares results.
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
        if sat_result == oracle_result:
            print "Test Passes for file:  %s" % filename
        else:
            self.write_error(filename, oracle_result, sat_result)
    def test_io(self):
        """
        Tests a variety of different files with good(.ptf) and
        bad inputs (.ftf).
        """
        path = "test_files/"
        for file in os.listdir(path):
            if file.endswith(".ftf"):
                self.sat_args = ['../bin/satsolv', path + file]
                result = self.worker(False)
                if self.err not in result:
                    self.write_error(file, 'ERROR', result)
            elif file.endswith(".ptf"):
                if self.err in result or 'UNKNOWN' in result:
                    self.write_error(file, 'SATISFIABLE | UNSATISFIABLE', result)

    def write_error(self, filename, expected, received):
        """
        Writes the applicable error to the output file.
        """
        error = ("\n Test failed for file: %s  :  Expected output:  %s, Received output: %s \n"
            % (filename, expected, received))
        self.output.write( error )
        print error

    def test_simple(self):
        """
        Runs the simple tests, on small files.  Runs several versions of each type of file
        to increase chances of receiving SATISFIABLE and UNSATISFIABLE
        """
        for i in range (0, 20):
            literals = random.randint(1, 20)
            clauses = random.randint(1, 60)
            solver = SatFileInstance(literals, clauses)
            for j in range (0,10):
                filename = solver.create_file()
                self.execute(filename)
    def stress_test(self):
        """
        Runs a Large file.  Note:  The specifications of the assignment
        indicate that the max for literals or clauses is 65536.  If a file of
        this size were actually generated it would be in excess of 30 GB.  I
        will not generate a file of that size here.  I have settled on an
        size of approximately 300 MB.
        """
        solver = SatFileInstance(10000, 10000)
        filename = solver.create_file()
        self.execute(filename)

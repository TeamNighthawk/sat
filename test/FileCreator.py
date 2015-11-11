"""
This Module contains the class that will create Files for the SatSolver
Author: Dustin Jay Tuckett
"""
import random

class SatFileInstance(object):
    """
    Conjunctive Normal Form (CNF) Sat File Creator.
    1.  Clauses are joined by AND
    2.  Each Clause, in turn, consists of literals joined by OR
    3.  Each literal is either the name of a variable
    A positive literal: (x)
    A negative literal: (-x)
    """
    def __init__(self, literal_count, clause_count):
        """
        Instansiates a SatFile Object.  This object is used to create sat files
          meeting the criteria passed in.

        literal_count -- The maximum number of literals to be used.
              (note:  This does not guarantee that all literals will be used)
        clause_count -- The maximum number of clauses to be used.
        """
        self.literal_count = literal_count
        self.clause_count = clause_count
        self.literal_list = range(1, literal_count + 1)
        self.clause_end = "0\n"
        self.counter = 0
    def create_file(self):
        """
        Creates a satFile and returns the string filename.
        Filename convention sat_<literal_count>_<clause_count>_<counter>.cnf
        Returns the filename after file has been created
        """
        filename = "test_files/sat_%r_%r_%r.cnf" % (self.literal_count, self.clause_count, self.counter)
        current_file = open(filename, 'w')
        current_file.write("c %r \n" %(filename))
        current_file.write("p cnf %r %r\n" %(self.literal_count, self.clause_count))
        for i in range(0, self.clause_count):
            current_file.write(self.create_clause())
        current_file.close()
        #increment the counter for the next file to be created of this type
        self.counter += 1
        return filename
    def create_clause(self):
        """
        Construct a clause:
        A clause is a set of literals and no repeats are allowed.  Each literal
        is randomly negated.
        """

        literals = random.sample(self.literal_list, random.randint(1, self.literal_count))

        clause = ""

        for literal in literals:
            if random.choice([True, False]):
                clause += str(literal) + " "
            else:
                clause += "-" + str(literal) + " "
        clause += self.clause_end
        return clause

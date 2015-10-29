#ifndef SATSOLV_H
#define SATSOLV_H

#include <stdbool.h>

/* Definitions */
#define SATISFIABLE 0
#define UNSATISFIABLE 1
#define UNKNOWN 2
#define ERROR 3
#define DEBUG 1     // enables debugging output
#define MAXLINE 256 // maximum length of a line
#define MAXCLAUSES 256 // maximum number of clauses in a formula
#define SAT_STRING "SATISFIABLE\n"
#define UNSAT_STRING "UNSATISFIABLE\n"
#define UNKNOWN_STRING "UNKNOWN\n"
#define ERROR_STRING "ERROR\n" // define the error string

typedef struct {
        unsigned short id;
        bool sign;
} literal;

typedef struct {
       unsigned short length;
       literal lits[];
} clause;

typedef struct {
       unsigned short nvars;
       unsigned short nclauses;
       clause clauses[];
} formula;

/* Core algorithm */
int solve(formula *);

/* Helper functions */
literal* is_unitclause(clause *, int [], int[]);
bool alllits_assigned(clause *, int[]);
bool clause_satisfied(clause *, int[]);
void assert_literal(literal *, int [], int []);
void pre_process(FILE *fp, formula *);
long int convert_to_int(char *pch);

#endif

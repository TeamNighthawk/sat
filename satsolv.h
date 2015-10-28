#ifndef SATSOLV_H
#define SATSOLV_H

/* Definitions */
#define SATISFIABLE 0
#define UNSATISFIABLE 1
#define UNKNOWN 2
#define ERROR 3
#define DEBUG 1     // enables debugging output
#define MAXLINE 256 // maximum length of a line
#define MAXCLAUSES 65536 // maximum number of clauses in a formula
#define SAT_STRING "SATISFIABLE\n"
#define UNSAT_STRING "UNSATISFIABLE\n"
#define UNKNOWN_STRING "UNKNOWN\n"
#define ERROR_STRING "ERROR\n" // define the error string

/* Core algorithm */
int solve(FILE *fp);

/* Helper functions */
void get_clauses(char *clauses[], FILE *fp);
void print_clauses(char *clauses[], int nclauses);
void print_assigned(int assigned[], int nvar);
void get_fileparams(FILE *fp, int *, int *);
int is_unitclause(char *, int assigned[]);

void pre_process(FILE *fp);
long int convert_to_int(char *pch);

#endif

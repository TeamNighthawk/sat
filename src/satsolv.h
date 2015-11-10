#ifndef SATSOLV_H
#define SATSOLV_H

#include <stdbool.h>

/* Definitions */
#define SATISFIABLE 0
#define UNSATISFIABLE 1
#define UNKNOWN 2
#define ERROR 3
#define DEBUG 1     // enables debugging output
#define MAXLINE 65537 // maximum length of a line
#define MAXCLAUSES 65536 // maximum number of clauses in a formula
#define MAXLITS 65536    // maximum number of literals in a formula
#define SAT_STRING "SATISFIABLE\n"
#define UNSAT_STRING "UNSATISFIABLE\n"
#define UNKNOWN_STRING "UNKNOWN\n"
#define ERROR_STRING "ERROR\n" // define the error string

/* represents a single literal */
typedef struct {
        unsigned short id;
        bool sign;
} literal;

/* represents a clause, which is simply a set of literals */
typedef struct {
       unsigned short length;
       literal* lits[];
} clause;

/* represents a formula, which is just a set of clauses */
typedef struct {
       unsigned short nvars;
       unsigned short nclauses;
       clause* clauses[];
} formula;

typedef struct {
        literal *lp;
        bool guess;
} stack_item;

typedef struct {
    stack_item **items;
    unsigned short top;
} stack;

/* Core algorithm */
int solve(formula *);

/* Helper functions */
literal* is_unitclause(clause *, bool [], bool[]);
bool alllits_assigned(clause *, bool[]);
bool clause_satisfied(clause *, bool[]);
void assert_literal(literal *, bool [], bool []);
formula* pre_process(FILE *fp);
void push_stack(stack *, stack_item *);
void pop_stack(stack *, stack_item *);
long int convert_to_int(char *pch);
void print_structure();

#endif

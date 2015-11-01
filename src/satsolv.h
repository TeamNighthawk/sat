#ifndef SATSOLV_H
#define SATSOLV_H

#include <stdio.h>
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
void pre_process(FILE *fp, formula *);
void push_stack(stack *, stack_item *);
void pop_stack(stack *, stack_item *);
long int convert_to_int(char *pch);

#endif

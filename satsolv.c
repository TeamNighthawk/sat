/* satsolv.c - A simple SAT solver.
 * 
 * <Authors>: Dalton Wallace, Jonathan Whitaker, Jay Tuckett, Tarik Courdy
 */

/* Includes */
#include <stdio.h>

/* Definitions */
#define SATISFIABLE 0
#define UNSATISFIABLE 1
#define UNKNOWN 2
#define ERROR 3
#define DEBUG 1

int solve(FILE *fp);

int main(int argc, char **argv)
{
    /* if an incorrect number of arguments was supplied, simply
       print an error and return. */
    if (argc != 2) {
        if (DEBUG)
            printf("usage: ./satsolv <filename>\n");

        printf("ERROR\n");
        return 0;
    }

    /* attempt to open the file containing the logic formula */
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        if (DEBUG)
            fprintf(stderr, "Can't open input file '%s'!\n", argv[1]);

        printf("ERROR\n");
        return 0;
    }

    /* invoke the solver and print out the result */
    int res = solve(fp);
    switch(res) {
        case SATISFIABLE:
            printf("SATISFIABLE\n");
            break;
        case UNSATISFIABLE:
            printf("UNSATISFIABLE\n");
            break;
        case UNKNOWN:
            printf("UNKNOWN\n");
            break;
        default:
            printf("ERROR\n");
    }

    return 0;
}

/**
  * Attempts to solve the propositional logic formula supplied in the input file.
  *
  * The input argument to this function contains a propositional logic formula given in the simplified 
  * DIMACS format (http://www.satcompetition.org/2004/format-solvers2004.html). This function should
  * parse the input file and apply the Davis-Putnam-Logemann-Loveland (DPLL) algorithm to the logic
  * formulas. If the supplied formulas can be satisfied with some input, then this function will 
  * return SATISFIABLE. If the supplied formulas *cannot* be satisfied, then this function will return
  * UNSATISFIABLE. It the formulas cannot be verified SATISFIABLE or UNSATISFIABLE, then this function
  * will return UNKNOWN.  
  *
  * @arg fp - A pointer to a file object that contains the propositional logic formulas.
  */
int solve(FILE *fp)
{
    /* otherwise the result is unknown */ 
    return UNKNOWN;
}

/* satsolv.c - A simple SAT solver.
 * 
 * <Authors>: Dalton Wallace, Jonathan Whitaker, Jay Tuckett, Tarik Courdy
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "satsolv.h"

// represents a single boolean variable in a clause
struct boolvar {
    unsigned short id;
    char val;
    char guess;
};

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

    fclose(fp);
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

    // get the nvar and nclauses values from the input file
    int nvar, nclauses; 
    char line[MAXLINE];
    while (fgets(line, MAXLINE, fp) != NULL) { 
       if (line[0] == 'p') {
            char * pch;
            pch = strtok (line," ");
 
            int c = 0; 
            while (pch != NULL)
            {
                if (c == 2)
                    nvar = atoi(pch);
                
                if (c == 3) 
                    nclauses = atoi(pch);

                pch = strtok (NULL, " ");
                c++;
            }
            break;
       }
    }
    
    if (DEBUG) {
        printf("nvar (%d)\n", nvar);
        printf("nclauses (%d)\n", nclauses);
    }

    // initialize the stack structure
    struct boolvar varstack[nvar];
    int top = 0;

    // create a structure to keep track of assigned variables
    int assigned[nvar];

    // get the clauses from the input file
    char *clauses[nclauses];  
    int i;
    for (i = 0; i < nclauses; i++)
        clauses[i] = malloc(MAXLINE * sizeof(char));
    get_clauses(clauses, fp);
    if (DEBUG) print_clauses(clauses, nclauses);

    // otherwise the result is unknown
    return UNKNOWN;
}

/** HELPER FUNCTIONS **/

/**
  * Parses the propositional logic clauses in the input file into an array of clauses, where
  * each entry in the array is an array of characters representing that clause.
  *
  * For example, the propositional logic formula:
  * 
  * (a | b) & (b | c)
  * 
  * is composed of two clauses "(a | b)" and "(b | c)". This function would return an array like so:
  *
  * [0] -> "(a | b)"
  * [1] -> "(b | c)"
  */
void get_clauses(char *clauses[], FILE *fp)
{
    int i;
    char line[MAXLINE];
    while (fgets(line, MAXLINE, fp) != NULL) {
        if (line[0] == 'c' || line[0] == 'p')
            continue; 

        strcpy(clauses[i++], strtok(line, "\n")); 
    }
}    

/**
  * Prints the clauses contained in the array of clauses.
  */
void print_clauses(char *clauses[], int nclauses)
{
   printf("Clauses: \n");

   int i;
   for (i = 0; i < nclauses; i++)
       printf("%s\n", clauses[i]); 
    
}

/** END HELPER FUNCTIONS **/

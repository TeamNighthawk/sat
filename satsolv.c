/* satsolv.c - A simple SAT solver.
 * 
 * <Authors>: Dalton Wallace, Jonathan Whitaker, Jay Tuckett, Tarik Courdy
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> 
#include <assert.h>
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

    pre_process(fp);
 
    
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

void pre_process(FILE *fp) {
    long int nvar, nclauses; 
    char line[MAXLINE];
    int outerCount = 0;
    while (fgets(line, MAXLINE, fp) != NULL) { 
        /*Skip the comments*/
        if(line[0] == 'c'){
            continue;
        } else if (line[0] == 'p') { /*Get the nbvar and nbclause args*/
            char * pch;
            pch = strtok (line," ");

            int c = 0; 
            while (pch != NULL){
              if (c == 2)
                  nvar = convert_to_int(pch);

              if (c == 3) 
                  nclauses = convert_to_int(pch);

              pch = strtok (NULL, " ");
              c++;
          }
        } else{ /*This is an argument clause*/
          char *var;
          long int varList[MAXLINE] = {0}; /*Initialize all values to 0*/
          int innerCount;
          long int numVal;
        /*convert line into array*/
          outerCount++;
          var = strtok(line, " ");
          while(var != NULL){
            numVal = convert_to_int(var);

            /*End of line*/
            if(numVal == 0){
                break;
            }

            /*If the number in the clause is greater than what the pcf line told us then file is ill formatted*/
            if(numVal > nvar){
                printf(ERROR_STRING);
                exit(0);
            }
            innerCount = 0;
            while(varList[innerCount] != 0){
                /*Cant have duplicates and can't have a neg and pos of same number in same line*/
                if(numVal == varList[innerCount] || (numVal*-1) == varList[innerCount]){
                    printf(ERROR_STRING);
                    exit(0);
                }
                innerCount++;
            }
            /*we passed all error checking for this value so add it to our list*/
            varList[innerCount] = numVal;
            
            var = strtok(NULL, " ");
        }



    }
}

    /*If we have more arg lines than what pcf told us then bad file*/
    if(outerCount != nclauses){
        printf(ERROR_STRING);
        exit(0);
    }
}

/*Helper to convert string to long int and do error checking to make sure we have an int
  If not print error and exit program */
long int convert_to_int(char * pch){
    /*NULL check on pointer*/
    assert(pch != 0);
    char *end = "\0";
    long int val;
    errno = 0;
    val = strtol(pch, &end, 10);
    /*the \n check is because the line ends in a \n and it will be stored in the end pointer as invalid but the number itself
      is not invalid*/
    if(errno || (strcmp(end, "\0") && strcmp(end, "\n"))) {
      printf(ERROR_STRING);
      exit(0);
    }
    return val;
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
    int *assigned = calloc(nvar, sizeof(int));
    if (DEBUG) print_assigned(assigned, nvar);

    // get the clauses from the input file
    char *clauses[nclauses];  
    int i;
    for (i = 0; i < nclauses; i++)
        clauses[i] = malloc(MAXLINE * sizeof(char));
    get_clauses(clauses, fp);
    if (DEBUG) print_clauses(clauses, nclauses);

    is_unitclause(clauses[0], assigned);

    // cleanup
    //free(varstack);
    free(assigned);
    for (i = 0; i < nclauses; i++)
        free(clauses[i]);

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

/**
  * Prints the assigned variables in the assigned array.
  */
void print_assigned(int assigned[], int nvar)
{
    printf("Assigned variables:\n");

    int i;
    for (i = 0; i < nvar; i++)
        printf("%d ", assigned[i]);
    printf("\n");
}

/**
  * Returns True if supplied clause is a unit clause. False otherwise.
  *
  * A clause is said to be a unit clause if all but 1 variable is assigned
  * in the clause and the clause is unsatisfied. 
  */
int is_unitclause(char *clause, int *assigned)
{
    int cond = 0;

    int var;
    char *pch;
    pch = strtok(clause, " ");
    while (pch != NULL) {
        var = atoi(pch);
        pch = strtok(NULL, " ");
    }

    if (cond) 
        return 1;
    else
        return 0;
}

/** END HELPER FUNCTIONS **/

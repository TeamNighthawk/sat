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
#include <stdbool.h>
#include "satsolv.h"

int main(int argc, char **argv)
{
    /* if an incorrect number of arguments was supplied, simply
       print an error and return. */
    if (argc != 2) {
        if (DEBUG)
            printf("usage: ./satsolv <filename>\n");

        printf(ERROR_STRING);
        return 0;
    }

    /* attempt to open the file containing the logic formula */
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        if (DEBUG)
            fprintf(stderr, "Can't open input file '%s'!\n", argv[1]);

        printf(ERROR_STRING);
        return 0;
    }

    formula form;

    /* validate the input file */
    pre_process(fp, &form);

    /* invoke the solver and print out the result */
    int res = solve(&form);
    switch(res) {
        case SATISFIABLE:
            printf(SAT_STRING);
            break;
        case UNSATISFIABLE:
            printf(UNSAT_STRING);
            break;
        case UNKNOWN:
            printf(UNKNOWN_STRING);
            break;
        default:
            printf(ERROR_STRING);
    }

    // close the input file
    fclose(fp);
    return 0;
}

void pre_process(FILE *fp, formula *form) {

    long int nvar, nclauses;
    char line[MAXLINE];
    int clauseCount = 0;
    while (fgets(line, MAXLINE, fp) != NULL) {
        /* Skip the comments */
        if(line[0] == 'c') {
            continue;
        } else if (line[0] == 'p') { /* Get the nbvar and nbclause args */
            char * pch;
            pch = strtok (line," ");

            int c = 0;
            while (pch != NULL) {
                if (c == 2)
                    nvar = convert_to_int(pch);

                if (c == 3)
                    nclauses = convert_to_int(pch);

                pch = strtok (NULL, " ");
                c++;
            }
        } else { /* assume an argument clause and check its correctness */
            char *var;
            long int varList[MAXLINE] = {0}; /* Initialize all values to 0 */
            int innerCount = 0;
            long int numVal;
	    clauseCount++;

            /* convert line into array */
            var = strtok(line, " ");
            while(var != NULL) {
                numVal = convert_to_int(var);

                /* Account for end of line */
                if(numVal == 0) {
                    break;
                }

                /* If the number in the clause is not between -nbvar and nbvar then file is ill formatted */
                if(numVal < -nvar || numVal > nvar) {
                    printf(ERROR_STRING);
                    exit(0);
                }

                innerCount = 0;
                while(varList[innerCount++] != 0) {
                    /* Cant have duplicates and can't have opposite literals i and -i simultaneously */
                    int dup = numVal == varList[innerCount];
                    int opposite_lit = (numVal*-1) == varList[innerCount];
                    if(dup || opposite_lit) {
                        printf(ERROR_STRING);
                        exit(0);
                    }
                }

                /* Otherwise we passed all error checking for this value so add it to our list */
                varList[innerCount] = numVal;

                // get the next token in the clause
                var = strtok(NULL, " ");
            }

        }


    }

    /* If we there was *not* exactly the right amount of clauses, then the file is ill formatted */
    if(clauseCount != nclauses) {
      printf(ERROR_STRING);
      exit(0);
    }

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
  * @arg form - A pointer to a formula structure that contains the propositional logic clauses.
  */
int solve(formula *form)
{
    if (DEBUG) {
        printf("nvar (%d)\n", form->nvars);
        printf("nclauses (%d)\n", form->nclauses);
    }

    // create the structures to store the state of the
    // algorithm
    bool assigned[form->nvars];
    bool vals[form->nvars];
    stack s[form->nvars];

    // foreach clause in formula
    int i;
    for (i = 0; i < form->nclauses; i++) {

        // if clause is a unit clause
        literal *lp;
        if ((lp = is_unitclause(&form->clauses[i], assigned, vals)) != NULL) {
            assert_literal(lp, vals, assigned);
            stack_item si = {lp, 0};
            push_stack(s, &si);
            continue;
        }
        else {
            // if all literals are assigned
            if (alllits_assigned(&form->clauses[i], assigned)) {
                // if clause satisfied
                if (clause_satisfied(&form->clauses[i], vals))
                    continue;
                else {
                    // "backtrack"
                }
            }
            else {
                // foreach literal
                int j;
                for(j = 0; j < form->clauses[i].length; j++) {
                    // if clause is a unit clause:
                    if ((lp = is_unitclause(&form->clauses[i], assigned, vals)) != NULL) {
                        assert_literal(lp, vals, assigned);
                        stack_item si = {lp, 0};
                        push_stack(s, &si);
                        continue;
                    }
                    else {
                        // guess on literal if literal not assigned
                    }
                }
            }
        }
    }

    // cleanup

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
    int i = 0;
    char line[MAXLINE];
    while (fgets(line, MAXLINE, fp) != NULL) {
        if (line[0] == 'c' || line[0] == 'p')
            continue;

        strcpy(clauses[i++], strtok(line, "\n"));
    }
}

/**
  * Returns a pointer to the remaining unsigned literal if the supplied clause
  * is a unit clause. Null otherwise.
  *
  * sign | value | sign XOR value
  */
literal* is_unitclause(clause *c, bool assigned[], bool vals[])
{
    int i;
    literal * lp;
    int assigned_cnt = 0;
    bool satisfied = 0;
    for (i = 0; i < c->length; i++) {
        if (assigned[c->lits[i].id]) {
            bool sign = c->lits[i].sign;
            bool value = vals[c->lits[i].id];
            assigned_cnt++;
            satisfied |= (sign ^ value);
        }
        else
            lp = &c->lits[i];
    }

    if (assigned_cnt != (c->length - 1))
        return NULL;
    else
        return satisfied ? NULL: lp;
}

void assert_literal(literal *lp, bool vals[], bool assigned[])
{
    if (lp->sign)
        vals[lp->id] = 0;
    else
        vals[lp->id] = 1;

    assigned[lp->id] = 1;

    return;
}

bool alllits_assigned(clause *c, bool assigned[])
{

    // if any of the literals in the clause are not assigned,
    // return false. otherwise return true
    int i;
    for (i = 0; i < c->length; i++) {
        if (!assigned[c->lits[i].id])
            return 0;
    }

    return 1;
}

bool clause_satisfied(clause *c, bool vals[])
{
    bool satisfied = 0;

    int i;
    for (i = 0; i < c->length; i++) {
        bool sign = c->lits[i].sign;
        bool value = vals[c->lits[i].id];
        satisfied |= (sign ^ value);
    }

    return satisfied;
}

void push_stack(stack *sp, stack_item *item)
{
    sp->items[sp->top] = item;
    sp->top++;
}

void pop_stack(stack *sp, stack_item *item)
{
    item = sp->items[sp->top];
    sp->top++;
}

/**
  * Helper to convert string to long int and do error checking to make sure we have an int.
  * If not print error and exit program
  */
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

/** END HELPER FUNCTIONS **/

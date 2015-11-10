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

    /*if we have an empty file just return error */
    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0){
        printf(ERROR_STRING);
        exit(0);
    } else{
        rewind(fp);
    }

    /* validate the input file */
    formula *form = pre_process(fp);

    // prints the resulting structure created by the pre-processing step
    if (DEBUG)
        print_structure(form);

    /* invoke the solver and print out the result */
    int res = solve(form);
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

void print_structure(formula *f)
{
    printf("nvar (%d)\n", f->nvars);
    printf("nclauses (%d)\n\n", f->nclauses);

    int i, j;
    for(i = 0; i < f->nclauses; i++) {
        literal *lp;
        for(j = 0; j < f->clauses[i]->length; j++) {
            lp = f->clauses[i]->lits[j];
            printf("%d ", lp->sign ? -1*(lp->id): lp->id);
        }
        printf("\n");
    }
}

formula* pre_process(FILE *fp) {
    char line[MAXLINE];
    int nvars = 0;
    int nclauses = 0;
    int clauseCount = 0;
    bool containsPLine = false;
    formula *form;
    while (fgets(line, MAXLINE, fp) != NULL)
    {
        /* Skip comments */
        if(line[0] == 'c') {
            continue;
        } else if (line[0] == 'p') { /* Get the nbvar and nbclause args */
            /* If we already processed a p line then there are two which is ill format */
            if(containsPLine) {
                printf(ERROR_STRING);
                exit(0);
            }

            char *pch;
            pch = strtok(line," ");
            int c = 0;
            while (pch != NULL)
            {
                if (c == 1 && strcmp(pch, "cnf") != 0) {
                    printf(ERROR_STRING);
                    exit(0);
                }
                if (c == 2)
                    nvars = convert_to_int(pch);

                if (c == 3)
                    nclauses = convert_to_int(pch);

                pch = strtok(NULL, " ");
                c++;
            }

            if(nvars > MAXLITS || nclauses > MAXCLAUSES || c != 4) {
                printf(ERROR_STRING);
                exit(0);
            }

            form = malloc(sizeof(formula) + nclauses * sizeof(clause*));
            form->nvars = nvars;
            form->nclauses = nclauses;
            containsPLine = true;
        } else { /* assume an argument clause and check its correctness */

            /* if we haven't seen the p line, then the format of the file was
               incorrect */
            if (!containsPLine) {
                printf(ERROR_STRING);
                exit(0);
            }

            char *var;
            long int varList[MAXLINE] = {0}; /* Initialize all values to 0 */
            int innerCount = 0;
            long int numVal;
            int litCount = 0;
            bool hasZero = false;

            /* convert line into array */
            var = strtok(line, " ");
            varList[0] = convert_to_int(var);
            litCount++;
            var = strtok(NULL, " ");
            while(var != NULL) {
                numVal = convert_to_int(var);
                /*If there are inputs after zero it is ill formatted*/
                if(hasZero == true){
                    printf(ERROR_STRING);
                    exit(0);
                }
                /* Account for end of line */
                if(numVal == 0) {
                    hasZero = true;
                } else{
                    litCount++;
                }

                /* If the number in the clause is not between -nbvar and nbvar then file is ill formatted */
                if(numVal < form->nvars * -1 || numVal > form->nvars) {
                    printf(ERROR_STRING);
                    exit(0);
                }

                innerCount = 0;
                while(varList[innerCount] != 0) {
                    /* Cant have duplicates and can't have opposite literals i and -i simultaneously */
                    int dup = numVal == varList[innerCount];
                    int opposite_lit = (numVal*-1) == varList[innerCount];
                    if((dup || opposite_lit) && !hasZero) {
                        printf(ERROR_STRING);
                        exit(0);
                    }

                    innerCount++;
                }

                /* Otherwise we passed all error checking for this value so add it to our list */
                varList[innerCount] = numVal;

                // get the next token in the clause
                var = strtok(NULL, " ");
            }

            if(!hasZero){
                printf(ERROR_STRING);
                exit(0);
            }

            /*create the clause, add the literals, and then add it to the formula struct*/
            clause *c = malloc(sizeof(clause) + (litCount) * sizeof(literal*));
            c->length = litCount;

            innerCount = 0;
            while(varList[innerCount] != 0)
            {
                literal *l = malloc(sizeof(literal));
                if(varList[innerCount] < 0){
                    l->id = varList[innerCount] * -1;
                    l->sign = true;
                } else {
                    l->id = varList[innerCount];
                    l->sign = false;
                }

                c->lits[innerCount] = l;
                innerCount++;
            }

            form->clauses[clauseCount] = c;
            clauseCount++;
        }
    }

    /* If we there was *not* exactly the right amount of clauses, then the file is ill formatted */
    if(clauseCount != form->nclauses) {
      printf(ERROR_STRING);
      exit(0);
    }

    return form;
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
    // create the structures to store the state of the
    // algorithm
    bool assigned[form->nvars];
    bool vals[form->nvars];
    stack s;
    stack_item* sitems[form->nvars];
    s.items = sitems;
    s.top = 0;

    // foreach clause in formula
    int i;
    for (i = 0; i < form->nclauses; i++) {

        // if clause is a unit clause
        literal *lp;
        if ((lp = is_unitclause(form->clauses[i], assigned, vals)) != NULL) {
            assert_literal(lp, vals, assigned);
            stack_item si = {lp, 0};
            push_stack(&s, &si);
            continue;
        }
        else {
            // if all literals are assigned
            if (alllits_assigned(form->clauses[i], assigned)) {
                // if clause satisfied
                if (clause_satisfied(form->clauses[i], vals))
                    continue;
                else {
                    // "backtrack"
                }
            }
            else {
                // foreach literal
                int j;
                for(j = 0; j < form->clauses[i]->length; j++) {
                    // if clause is a unit clause:
                    if ((lp = is_unitclause(form->clauses[i], assigned, vals)) != NULL) {
                        assert_literal(lp, vals, assigned);
                        stack_item si = {lp, 0};
                        push_stack(&s, &si);
                        continue;
                    }
                    else {
                        // guess on literal if literal not assigned
                        literal *lp = form->clauses[i]->lits[j];
                        int lit_id = lp->id;
                        if (!assigned[lit_id]) {
                            assert_literal(lp, vals, assigned);
                            stack_item si = {lp, 1};
                            push_stack(&s, &si);
                        }
                    }
                }
            }
        }
    }

    // if we get this far the assumption is that we could satisfy the formula
    return SATISFIABLE;
}

/** HELPER FUNCTIONS **/

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

    /* For each literal in the clause, check if it is assigned. If the literal
       is already assigned, then gets its appropriate value and add it to the
       satisfied variable. Otherwise the literal remains a candidate for
       assignment. */
    for (i = 0; i < c->length; i++) {
        if (assigned[c->lits[i]->id]) {
            bool sign = c->lits[i]->sign;
            bool value = vals[c->lits[i]->id];
            assigned_cnt++;
            satisfied |= (sign ^ value);
        }
        else
            lp = c->lits[i];
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
        if (!assigned[c->lits[i]->id])
            return 0;
    }

    return 1;
}

bool clause_satisfied(clause *c, bool vals[])
{
    bool satisfied = 0;

    int i;
    for (i = 0; i < c->length; i++) {
        bool sign = c->lits[i]->sign;
        bool value = vals[c->lits[i]->id];
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

    if(strcmp(pch, "-0") == 0 || strcmp(pch, "-0\n") == 0){
        printf(ERROR_STRING);
        exit(0);
    }
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

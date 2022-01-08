#include <stdio.h>
#include <stdlib.h>

#include "nfa.h"

// constuctor for new nfa
NFA* newNFA() {
    NFA* nfa = (NFA*) malloc(sizeof(NFA));
    nfa->start = 0;
    nfa->accept = 0;
    nfa->transitionCount = 0;
    nfa->transitions = NULL;
    return nfa;
}

// constructor for new transition
Transition* newTransition(char symbol) {
    Transition* t = (Transition*) malloc(sizeof(Transition));
    t->start = 0;
    t->end = 0;
    t->symbol = symbol;
    t->next = NULL;
    return t;
}

// qsort comparison function
int cmpTransitions(const void * a, const void * b) {
    /*
    Note to future self:
    A (Transition*) passed to comparison function is passed as (Transition**)
    and disquised as a (void*) so we need to dereference that with *(Transition**)
    before we can use it safely.
    */
    Transition* tA = *(Transition**) a;
    Transition* tB = *(Transition**) b;

    /*
    If you have multiple transitions coming from a single start state
    return them in order of their end states
    */
    if (tA->start == tB->start) {
        return (tA->end - tB->end);
    // else just return the transition with the lowest start state
    } else if (tA->start < tB->start) {
        return -1;
    } else {
        return 1;
    }

}

// prints NFA with sorted transitions
void printNfa(Stack* s) {
    if (s->top == NULL) {
        perror("Error: tried printing empty stack.");
        exit(1);
    }
    StackItem* tmp = s->top;

    if (tmp != NULL) {
        NFA* tmpNFA = tmp->val;
        Transition* t = tmpNFA->transitions;
        Transition* orderedTransitions[tmpNFA->transitionCount];
        unsigned int sortedCount = 0;

        // store transtions in an array for easier sorting
        while (t) {
            if (sortedCount > tmpNFA->transitionCount) {
                printf("Error. Count error when sorting...\n");
                exit(1);
            }
            orderedTransitions[sortedCount++] = t;
            t = t->next;
        }

        //sort array and print
        qsort(orderedTransitions, sortedCount, sizeof(Transition*), cmpTransitions);
        for (int i = 0; i < sortedCount; i++) {
            t = orderedTransitions[i];
            printf("(q%d, %c)->q%d\n", t->start, t->symbol, t->end);
        }
    }        
}

void freeNfa(NFA* nfa) {
    Transition *tmpT = nfa->transitions;
    while (nfa->transitions) {
        nfa->transitions = nfa->transitions->next;
        free(tmpT);
        tmpT = nfa->transitions;
    }
}

void deleteNfaStack(Stack* s) {
            if (s) {
                StackItem* tmp = s->top;
                while (s->top) {
                    s->top = s->top->next;
                    freeNfa((NFA*) tmp->val);
                    free(tmp);
                    tmp = s->top;
                }
                
            }
            
}
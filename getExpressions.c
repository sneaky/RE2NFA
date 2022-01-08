#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "getExpressions.h"
#include "nfa.h"
#include "stack.h"


// separated and function to enable anding when reading >2 sequential operands
void nfaAnd(Stack* nfaStack) {
    // 2 then 1 because popping from stack
    NFA *tmpNFA2 = (NFA *)pop(nfaStack);
    NFA *tmpNFA1 = (NFA *)pop(nfaStack);
    NFA *freshNFA = newNFA();

    // new epsilon transition for linking NFA1->accept to NFA2->start
    Transition *t = newTransition('E');
    t->start = tmpNFA1->accept;
    t->end = tmpNFA2->start;

    // link new transition to NFA1's transitions
    freshNFA->transitions = t;
    t->next = tmpNFA1->transitions;

    // walk to end of NFA1 transition list
    Transition *tmpT = tmpNFA1->transitions;
    while (tmpT->next)
    {
        tmpT = tmpT->next;
    }
    // link NFA1 transitions to NFA2 transitions
    tmpT->next = tmpNFA2->transitions;

    //update the nfa's transition count
    freshNFA->transitionCount = tmpNFA1->transitionCount + tmpNFA2->transitionCount + 1;

    // new start

    freshNFA->start = tmpNFA1->start;
    // new accept
    freshNFA->accept = tmpNFA2->accept;

    // push new NFA and free up old NFAs
    push(freshNFA, nfaStack);
    free(tmpNFA1);
    free(tmpNFA2);
}

char* getNextExpression(FILE* fd, Stack* nfaStack) {
    if (fd == NULL) {
        perror("Error: file descriptor does not exist.");
        exit(1);
    }

    char ch;
    char buf[MAX_EXPRESSION_LEN];
    size_t index = 0;

    unsigned int currentState = 0;

    int operandCount = 0;
    // new
    int operatorCount = 0;

    while( (ch = fgetc(fd) ) ) {
        if (ch == EOF) {
            return NULL;
        }
        if (isspace(ch)) {
            // new
            if (!operatorCount) { // no operators given, just and everything
                if (nfaStack->top) {
                    while(nfaStack->top->next) {
                        nfaAnd(nfaStack);
                    }
                }
            }
            /*
            if we come across a space,
            we want to clear the stack and start over.
            */
            buf[index] = '\0';   
            printf("RE: %s\n", buf);
            
            //NFA* tmpNFA = pop(nfaStack);
            if (!nfaStack->top) {return NULL; }
            NFA* tmpNFA = (NFA*) (nfaStack->top->val);
            printf("Start: q%d\n", tmpNFA->start);
            printf("Accept: q%d\n", tmpNFA->accept);
            printNfa(nfaStack);
            printf("\n");
            deleteNfaStack(nfaStack);
            free(tmpNFA);
            currentState = 0;
            index = 0;
        }
        if (isalnum(ch) || ispunct(ch)) buf[index++] = ch;
        
        // error catching for invalid operators
        if (ispunct(ch)) {
            operandCount = 1;
            if (ch != '*' && ch != '|' && ch != '&') {
                char invalid = ch;
                while ((ch = fgetc(fd))) {
                    if (isalnum(ch) || ispunct(ch))
                        buf[index++] = ch;
                    if (isspace(ch) || ch == EOF) {
                        buf[index] = '\0';
                        printf("Error. Invalid input detected...\n");
                        printf("RE : %s\n", buf);
                        printf("Invalid input: \'%c\'\n", invalid);
                        printf("Skipping to next expression...\n\n");
                        currentState = 0;
                        index = 0;
                        deleteNfaStack(nfaStack);
                        return strdup(buf);
                    }
                }
            }
        }

        /*
        Invalid input handling
        */
        if (isalpha(ch)) {
            if (ch != 'a' && ch != 'b' && ch != 'c' && ch != 'd' && ch != 'e') {
                char invalid = ch;
                while( (ch = fgetc(fd)) ) {
                    //testing
                    if (nfaStack->top) {
                        while(nfaStack->top->next) {
                            nfaAnd(nfaStack);
                        }
                    }
                    if (isalnum(ch) || ispunct(ch)) buf[index++] = ch;
                    if ( isspace(ch) || ch == EOF) {
                        buf[index] = '\0';
                        printf("Error. Invalid input detected...\n");
                        printf("RE : %s\n", buf);
                        printf("Invalid input: \'%c\'\n", invalid);
                        printf("Skipping to next expression...\n\n");
                        currentState = 0;
                        index = 0; 
                        deleteNfaStack(nfaStack);
                        return strdup(buf);
                    }
                }
            }
            
            
            operandCount++;

            // create new singleton NFA
            NFA* nfa = newNFA();
            nfa->start = ++currentState;
            nfa->accept = ++currentState;

            // create new transition for operand
            Transition* transition = newTransition(ch);
            transition->start = nfa->start;
            transition->end = nfa->accept;

            //increment the nfa's transition count
            nfa->transitionCount++;
            
            // add the newly created transition to the new NFA
            nfa->transitions = transition;
            push(nfa, nfaStack);
        } else {
            /*
            We've found a valid operator.
            Make sure we handle the case where there are N > 2 operands read before reading this operator
            */
           if (operandCount > 2) {
               if (ch == '*') {
                   // handle * with N > 2 operands
                   // call nfaAnd on all the NFAs on the stack before moving on...
                   while (nfaStack->top->next) {
                       nfaAnd(nfaStack);
                   }
               } else if (ch == '&' || ch == '|') {
                   // handle | and & with N > 2 operands
                   // call nfaAnd on all N - 1 operands on the stack
                   // let each operators logic below handle the last operand
                   for (int i = 0; i < operandCount - 1; i+=2) {
                       nfaAnd(nfaStack);
                   }
               }
           }
        }

        /*
        Logic for evaluating operators
        */
        if (ch == '*') {
            NFA* tmpNFA = (NFA*) pop(nfaStack);
            NFA *freshNFA = newNFA();

            // transition from new start/accept to old start
            Transition *t1 = newTransition('E');
            t1->start = ++currentState;
            t1->end = tmpNFA->start;

            // transition from old accept to new start/accept
            Transition *t2 = newTransition('E');
            t2->start = tmpNFA->accept;
            t2->end = t1->start;

            // new start
            freshNFA->start = t1->start;
            // new accept
            freshNFA->accept = t1->start;
            

            // link new transitions to old transition list
            freshNFA->transitions = t1;
            t1->next = t2;
            t2->next = tmpNFA->transitions;
            
            //update the nfa's transition count
            freshNFA->transitionCount = tmpNFA->transitionCount + 2;
            
            // push the new nfa and free up the old nfa
            push(freshNFA, nfaStack);
            free(tmpNFA); 

            // reset operand count after you see an operator
            operandCount = 0;
        }
        if (ch == '|') {
            // 2 then 1 because popping from stack
            NFA* tmpNFA2 = (NFA*) pop(nfaStack);
            NFA* tmpNFA1 = (NFA*) pop(nfaStack);
            NFA* freshNFA = newNFA();

            // transition from new start to NFA1
            Transition *t1 = newTransition('E');
            t1->start = ++currentState;
            t1->end = tmpNFA1->start;

            // transition from new start to NFA2
            Transition *t2 = newTransition('E');
            t2->start = t1->start;
            t2->end = tmpNFA2->start;

            // transition from NFA1->accept to new accept
            Transition *t3 = newTransition('E');
            t3->start = tmpNFA1->accept;
            t3->end = ++currentState;

            // transition from NFA2->accept to new accept
            Transition *t4 = newTransition('E');
            t4->start = tmpNFA2->accept;
            t4->end = t3->end;

            // new start
            freshNFA->start = t1->start;
            // new accept
            freshNFA->accept = t3->end;

            //set up transitions for freshNFA
            freshNFA->transitions = t1;
            // link new start state's transitions
            // to NFA1's transitions
            t1->next = t2;
            t2->next = tmpNFA1->transitions;

            //update the nfa's transition count
            freshNFA->transitionCount = tmpNFA1->transitionCount + tmpNFA2->transitionCount + 4;
            
            // walk to end of NFA1 transition list 
            Transition* tmpT = tmpNFA1->transitions;
            while(tmpT->next) {
                tmpT = tmpT->next;
            }
            // link end of NFA1's transition list to
            // beginning of NFA2's transition list
            tmpT->next = tmpNFA2->transitions;

            // walk to end of NFA2 transition list 
            tmpT = tmpNFA2->transitions;
            while(tmpT->next) {
                tmpT = tmpT->next;
            }
            // link new accept state's transitions
            // to the end of NFA2 transition list
            tmpT->next = t3;
            t3->next = t4;
            
            // push the new NFA and clean up old NFAs
            push(freshNFA, nfaStack);
            free(tmpNFA1);
            free(tmpNFA2);

            // reset operand count after you see an operator
            operandCount = 0;
        }
        if (ch == '&') {
            //
            nfaAnd(nfaStack);
            // reset operand count after you see an operator
            operandCount = 0;
        }
    }
    
    buf[index] = '\0';

    return strdup(buf);
}
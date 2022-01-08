#ifndef NFA_H
#define NFA_H

#include "stack.h" // for handling nfa printing

/*
structure for singly linked list of transitions
*/
typedef struct _transition {
    unsigned int start;
    unsigned int end;
    char symbol;
    struct _transition* next;
} Transition;

/*
structure for nfa object
*/
typedef struct _nfa {
    unsigned int start;
    unsigned int accept;
    Transition* transitions;
    unsigned int transitionCount;
} NFA;

NFA* newNFA();
Transition* newTransition(char symbol);
void printNfa(Stack* s);
void deleteNfaStack(Stack* s);

#endif
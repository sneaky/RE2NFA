#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stack.h"
//#include "nfa.h"

Stack* newStack() {
    Stack* s = (Stack*) malloc(sizeof(Stack));
    s->capacity = 0;
    s->top = NULL;
    return s;
}

void deleteStack(Stack* s) {
    if (s->top == NULL) {
        free(s);
    } else {
        StackItem* tmp = s->top;
        while (tmp != NULL) {
            s->top = s->top->next;
            free(tmp);
            tmp = s->top;
        }
    }
}

StackItem* newItem(void* val) {
    StackItem* item = (StackItem*) malloc(sizeof(StackItem));
    item->val = val;
    item->next = NULL;
    return item;
}

void push(void* val, Stack* s) {
    StackItem* item = newItem(val);
    if (s->top == NULL) {
        s->top = item;
    } else {
        item->next = s->top;
        s->top = item;
        s->capacity++;
    }
}

void* pop(Stack* s) {
    if (s->top == NULL) {
        perror("Error: undeflow.");
        return NULL;
    }
    void* val = s->top->val;
    StackItem* tmp = s->top;
    s->top = s->top->next;
    s->capacity--;
    free(tmp);
    return val;
}

void printStack(Stack* s) {
    if (s->top == NULL) {
        perror("Error: tried printing empty stack.");
        exit(1);
    }
    unsigned int i = 0;
    StackItem* tmp = s->top;
    printf("Printing stack contents:\n\n");
    printf("<Inserted Order>\t:\t<Inserted Value>\n\n");
    while (tmp != NULL) {
        printf("%d\t\t\t:\t\'%s\'\n", s->capacity - i, (char*) tmp->val);
        tmp = tmp->next;
        i++;
    }
}
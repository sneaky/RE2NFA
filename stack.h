#ifndef STACK_H
#define STACK_H

typedef struct _stackItem {
    void* val;
    struct _stackItem* next;
} StackItem;

typedef struct _stack {
    StackItem* top;
    unsigned int capacity;
} Stack;

Stack* newStack();
void deleteStack(Stack* s);
void push(void* val, Stack* s);
void* pop(Stack* s);
void deleteItem(StackItem* item);
void printStack(Stack* s);

#endif
#ifndef GETEXPRESSIONS_H
#define GETEXPRESSIONS_H

#include <stdio.h>
#include "stack.h"

#define MAX_EXPRESSION_LEN 256

/*
Walks through the input file one character at a time.

If all inputs are valid and the current character is a space,
we're at the end of a regular expression,
so output the regular expression and the associated NFA,
and then move on to look for the next regular expression or EOF.

If the current character is invalid,
then the current regular expression is invalid,
so let the user know and then move on to the next regular expression.

Once we reach EOF we're done.
*/
char* getNextExpression(FILE* fd, Stack* nfaStack);

#endif
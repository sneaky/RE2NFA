#include <stdio.h>
#include <stdlib.h>

#include "getExpressions.h"
#include "stack.h"
#include "nfa.h"

int main(int argc, char** argv) {
    // for handling multiple inputs
    for (int i = 1; i < argc; i++) {
        FILE* fd = fopen(argv[i], "r");
        if (fd == NULL) {
            perror("Can't open file.");
            exit(1);
        }
        char* str = NULL;

        Stack* nfaStack = newStack();
        
        // make sure to grab all expressions
        while ( (str = getNextExpression(fd, nfaStack)) ) {
            free(str);
        } 
        
        // clean up
        deleteStack(nfaStack);
        fclose(fd);

        // if we have multiple inputs, let the user know when we finish each input file
        if (argc > 2) {
            if ((i + 1) != argc) {
                printf("Finished reading file %d. Moving on to next input file...\n\n", i);
            } else {
                printf("Finished reading all input files. Exiting program now...\n");
            }
        } 
    }
    
    return 0;
}
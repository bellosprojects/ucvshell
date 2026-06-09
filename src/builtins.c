#include <stdlib.h>
#include <string.h>
#include "builtins.h"

int is_builtin(const char* command) {
    const char* builtins[] = {"cd", "pwd", "exit", "history", "help", NULL};
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(command, builtins[i]) == 0) {
            return 1; 
        }
    }
    return 0;
}
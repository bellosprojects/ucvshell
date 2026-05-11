#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    char linea[1024];
    char *tokens[64];

    while (1){

        printf("ucvsh> ");
        
        if(fgets(linea, sizeof(linea), stdin) == NULL) break;

        linea[strcspn(linea, "\n")] = 0;

        int i = 0;
        tokens[i] = strtok(linea, " ");

        while (tokens[i] != NULL && i < 64){
            printf("Token [%d]: %s\n", i, tokens[i]);
            i++;
            tokens[i] = strtok(NULL, " ");
        }
        
        if(i > 0 && strcmp(tokens[0], "exit") == 0) break;
    }

    return 0;
}
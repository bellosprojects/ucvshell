#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell_loop.h"
#include "path_utils.h"

void shell_loop(void) {

    char linea[1024];
    
    while (1) {

        printf("ucvsh> ");
        fflush(stdout);
        
        if(fgets(linea, sizeof(linea), stdin) != NULL){

            linea[strcspn(linea, "\n")] = 0;

            if (strcmp(linea, "exit") == 0) break;
            if (strlen(linea) == 0) continue;

            char *espacio = strchr(linea, ' ');
            char *comando;
            char *args = NULL;

            if(espacio){
                *espacio = '\0';
                args = espacio + 1;
            } else {
                args = "";
            }
            comando = linea;

            char * ruta_absoluta = find_command_path(comando);

            if(ruta_absoluta){
                printf("  Comando: %s\n", comando);
                printf("  Ruta: %s\n", ruta_absoluta);
                printf("  Args: %s\n", args);
                free(ruta_absoluta);
            } else {
                printf("Command '%s' not found\n", comando);
            }

            if(espacio) *espacio = ' ';
        }
        
    }
    
}
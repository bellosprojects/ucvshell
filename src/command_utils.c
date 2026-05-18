#include "command_utils.h"
#include "path_utils.h"
#include "dequeue.h"
#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

ast_node_t *crear_arbol_de_ejecucion(Dequeue *tokens){

    return NULL;
}

Dequeue *tokentizar(char *linea){

    Dequeue *tokens = crear_dequeue();

    if(!tokens) return NULL;

    return tokens;
}

void extraer_partes_de_comando(char *comando, char *ruta_programa, char **args){

    char *espacio = strchr(comando, ' ');

    if(espacio){
        *espacio = '\0';
        *args = espacio + 1;
    } else {
        *args = "";
    }
    
    char * ruta_absoluta = find_command_path(comando);

    ruta_programa = ruta_absoluta;

    if(espacio) *espacio = ' ';
}
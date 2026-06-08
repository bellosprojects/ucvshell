#include "command_utils.h"
#include "dequeue.h"
#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void mostrar_ast_recursivo(ast_node_t *nodo, int nivel) {
    if (nodo == NULL) return;
    
    // Crear la sangría según el nivel actual
    for (int i = 0; i < nivel; i++) {
        printf("    │"); // Identificador visual de jerarquía
    }
    printf("-- ");

    // 1. Si es un nodo de comando (Hoja del árbol) 
    if (nodo->type == NODE_COMMAND) {
        command_t *cmd = nodo->data.cmd;

        if (cmd && cmd->argv && cmd->argv[0]) { 
            // Imprimir el comando principal y sus argumentos
            printf("[CMD] %s", cmd->argv[0]);  
            
            int j = 1;
            while (cmd->argv[j] != NULL) { 
                printf(" '%s'", cmd->argv[j]); 
                j++;
            }

            // Mostrar si tiene redirecciones asociadas 
            if (cmd->input_file) { 
                printf(" < (%s)", cmd->input_file); 
            }
            if (cmd->output_file) { 
                if (cmd->append_output) printf(" >> (%s)", cmd->output_file); 
                else printf(" > (%s)", cmd->output_file); 
            }
            if (cmd->background) { 
                printf(" & [BG]"); 
            }
            printf("\n");
        } else {
            printf("[CMD] Vacío o inválido\n");
        }
        return; // Fin de la rama
    }

    // 2. Si es un nodo operador (Conector binario) 
    switch (nodo->type) { 
        case NODE_PIPE:      printf("[OPERADOR] PIPE (|)\n"); break; 
        case NODE_AND:       printf("[OPERADOR] AND (&&)\n"); break; 
        case NODE_OR:        printf("[OPERADOR] OR (||)\n"); break; 
        case NODE_SEMICOLON: printf("[OPERADOR] SEMICOLON (;)\n"); break; 
        default:             printf("[OPERADOR] DESCONOCIDO\n"); break; 
    }

    // Llamadas recursivas: Primero el hijo izquierdo, luego el derecho 
    mostrar_ast_recursivo(nodo->data.op.left, nivel + 1); 
    mostrar_ast_recursivo(nodo->data.op.right, nivel + 1); 
}

// Función principal pública que invocarás desde tu main.c
void print_ast(ast_node_t *node) {
    printf("\n========= ESTRUCTURA DEL AST =========\n");
    if (node == NULL) {
        printf("( - Árbol vacío - )\n");
    } else {
        mostrar_ast_recursivo(node, 0);
    }
    printf("======================================\n\n");
}

node_type_t obtener_tipo(Dequeue *tokens){

    char *tipo = ((token_t*)getAt(tokens, 0))->value;

    if(strcmp(tipo, "&&") == 0){
        return NODE_AND;
    } else if(strcmp(tipo, "||") == 0) {
        return NODE_OR;
    } else if(strcmp(tipo, "|") == 0) {
        return NODE_PIPE;
    } else if(strcmp(tipo, ";") == 0) {
        return NODE_SEMICOLON;
    }
    
    return UNKNOW_NODE;
}

char *obtener_valor(Dequeue *tokens){
    return ((token_t*)getAt(tokens, 0))->value;
}

ast_node_t *parsear_comando(Dequeue *tokens){

    if(!tokens) return NULL;

    char **args = (char **)malloc(sizeof(char*) * 64);
    int argc = 0;
    char *file_input = NULL;
    char *file_output = NULL;
    int bg = 0;
    int append = 0;

    while (getSize(tokens) > 0)

    {

        int type = ((token_t*)getAt(tokens, 0))->type;
        if(type == 1){

            if (strcmp(obtener_valor(tokens), ">") == 0)
            {
                liberar_token((token_t*)pop_front(tokens));

                // Se espera un archivo de salida ERROR

                file_output = ((token_t*)pop_front(tokens))->value;

                continue;
            } else if (strcmp(obtener_valor(tokens), "<") == 0)
            {
                liberar_token((token_t*)pop_front(tokens));

                // Se espera un archivo de entrada ERROR

                file_input = ((token_t*)pop_front(tokens))->value;
                continue;
            } else if (strcmp(obtener_valor(tokens), ">>") == 0)
            {
                liberar_token((token_t*)pop_front(tokens));

                // Se espera un archivo de salida ERROR

                file_output = ((token_t*)pop_front(tokens))->value;

                append = 1;

                continue;
            }

            break;
        }

        args[argc] = ((token_t*)pop_front(tokens))->value;

        argc++;

    }

    args[argc] = NULL;

    // Verificar background
    if(getSize(tokens) > 0)
    if(strcmp(obtener_valor(tokens), "&") == 0){
        bg = 1;
        liberar_token((token_t*)pop_front(tokens));
    }

    command_t *comando = crear_comando(args, file_input, file_output, append, bg);

    return crear_nodo_comando(comando);
}

ast_node_t *parsear_pipe(Dequeue *tokens){

    if(!tokens) return NULL;

    ast_node_t *left = parsear_comando(tokens);

    while(getSize(tokens) > 0){

        if(((token_t*)getAt(tokens, 0))->type == 0 || strcmp(obtener_valor(tokens), "|") != 0) break;

        liberar_token((token_t*)pop_front(tokens));

        ast_node_t *right = parsear_comando(tokens);

        left = crear_nodo_operador(NODE_PIPE, left, right);
    }

    return left;

}

ast_node_t *crear_arbol_de_ejecucion(Dequeue *tokens){

    if(!tokens) return NULL;

    ast_node_t *left = parsear_pipe(tokens);
    
    while (getSize(tokens) > 0 && ((token_t*)getAt(tokens, 0))->type == 1 && obtener_valor(tokens)) {
        
        node_type_t op = obtener_tipo(tokens);

        liberar_token((token_t*)pop_front(tokens));

        ast_node_t *right = parsear_pipe(tokens);

        left = crear_nodo_operador(op, left, right);

    }
    
    print_ast(left);
    
    return left;
}

void print_token(void* token){
    token_t t = *((token_t*)token);
    printf("[Token tipo %d]: %s\n", t.type, t.value);
}

void guardar(Dequeue* tokens, char *current_token, int type, int *n){

    current_token[*n] = '\0';
            
    char *value = (char*)malloc(sizeof(char) * ((*n) + 1));
    
    strcpy(value, current_token); 
    
    token_t* token = crear_token(type, value);
    
    push_back(tokens, (void*)token);
    *n = 0;
}

Dequeue *tokentizar(char *linea){
    
    Dequeue *tokens = crear_dequeue();
    
    if(!tokens) return NULL;

    int is_large = 0;
    char current_token[MAX_COMMAND_LENGTH] = "";
    char current_type = 0;
    int token_position = 0; 

    for(size_t i=0; i<strlen(linea); i++){

        int is_end = 0;
        
        if (linea[i] == ' ') {
            if(is_large){
                current_token[token_position] =' ';
                token_position++;
            } else {
                is_end = 1;
            }
        } else if(linea[i] == '\"') {
            is_large = is_large? 0 : 1;
        } else if(linea[i] == '&' || linea[i] == '|' || linea[i] == ';' || linea[i] == '>' || linea[i] == '<') {
            if(current_type == 0 && token_position > 0) guardar(tokens, current_token, 0, &token_position);
            
            current_type = 1;
            current_token[token_position] = linea[i];
            token_position++;
        } else {
            if(current_type == 1 && token_position > 0) guardar(tokens, current_token, 1, &token_position);

            current_type = 0;
            current_token[token_position] = linea[i];
            token_position++;
        }

        
        if((is_end || i == strlen(linea) - 1) && token_position > 0) guardar(tokens, current_token, current_type, &token_position);
    }

    print(tokens, print_token);

    return tokens;
}
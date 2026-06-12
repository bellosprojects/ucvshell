#include "command_utils.h"
#include "dequeue.h"
#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void free_token(void *token){
    liberar_token((token_t*)token);
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

ast_node_t *parsear_secuencia(Dequeue *tokens);

ast_node_t *parsear_comando(Dequeue *tokens){

    if(!tokens || getSize(tokens) == 0) return NULL;

    char **args = (char **)malloc(sizeof(char*) * 64);
    int argc = 0;
    char *file_input = NULL;
    char *file_output = NULL;
    int bg = 0;
    int append = 0;

    while (getSize(tokens) > 0)
    {
        token_t *token_actual = (token_t*)getAt(tokens, 0);
        int type = token_actual->type;

        if(type == 1){

            char *val_op = token_actual->value;

            if (strcmp(obtener_valor(tokens), ">") == 0 || strcmp(obtener_valor(tokens), "<") == 0 || strcmp(obtener_valor(tokens), ">>") == 0)
            {
                token_t *op_token = (token_t*)pop_front(tokens);
                int es_append = (strcmp(val_op, ">>") == 0);
                int es_input = (strcmp(val_op, "<") == 0);
                liberar_token(op_token);

                if(getSize(tokens) == 0){
                    // Error [Se esperaba descriptor]
                    perror("Se esperaba un descriptor");
                    break;
                }

                token_t *file_tok = (token_t*)pop_front(tokens);

                if(es_input){
                    if(file_input) free(file_input);
                    file_input = strdup(file_tok->value);
                } else {
                    if(file_output) free(file_output);
                    file_output = strdup(file_tok->value);
                    append =es_append;
                }

                liberar_token(file_tok);

                continue;
            }

            break;
        }

        token_t *arg_tok = (token_t*)pop_front(tokens);
        args[argc] = strdup(arg_tok->value);
        liberar_token(arg_tok);
        argc++;
    }

    args[argc] = NULL;

    // Verificar background
    if(getSize(tokens) > 0){
        token_t *posible_bg = (token_t*)getAt(tokens, 0);
        if(posible_bg->type == 1 && strcmp(posible_bg->value, "&") == 0){
            bg = 1;
            liberar_token((token_t*)pop_front(tokens));
        }
    }

    command_t *comando = crear_comando(args, file_input, file_output, append, bg);

    return crear_nodo_comando(comando);
}

ast_node_t *pasear_parentesis(Dequeue *tokens){

    if(!tokens || getSize(tokens) == 0) return NULL;

    token_t *token = (token_t*)getAt(tokens, 0);

    if (token->type == 1 && strcmp(token->value, "(") == 0)
    {
        liberar_token((token_t*)pop_front(tokens));

        ast_node_t* hijo = parsear_secuencia(tokens);

        if(getSize(tokens) == 0){
            // Error [Falta parentesis de cierre]
            perror("Se esperaba parentesis de cierre");
        }

        token_t *token_cierre = (token_t*)getAt(tokens, 0);

        if(token_cierre->type != 1 || strcmp(token_cierre->value, ")") != 0){
            // Error [Falta parentesis de cierre]
            perror("Se esperaba parentesis de cierre");
        }

        liberar_token((token_t*)pop_front(tokens));

        return hijo;
    }
    
    return parsear_comando(tokens);
}

ast_node_t *parsear_pipe(Dequeue *tokens){

    if(!tokens || getSize(tokens) == 0) return NULL;

    ast_node_t *left = pasear_parentesis(tokens);

    while(getSize(tokens) > 0){

        if(((token_t*)getAt(tokens, 0))->type == 0 || strcmp(obtener_valor(tokens), "|") != 0) break;

        liberar_token((token_t*)pop_front(tokens));

        ast_node_t *right = pasear_parentesis(tokens);

        left = crear_nodo_operador(NODE_PIPE, left, right);
    }

    return left;

}

ast_node_t *parsear_logicos(Dequeue *tokens){

    if(!tokens || getSize(tokens) == 0) return NULL;

    ast_node_t *left = parsear_pipe(tokens);

    while (getSize(tokens) > 0 && ((token_t*)getAt(tokens, 0))->type == 1) {

        if(strcmp(obtener_valor(tokens), "&&") != 0 && strcmp(obtener_valor(tokens), "||") != 0) break;
        
        node_type_t op = obtener_tipo(tokens);

        liberar_token((token_t*)pop_front(tokens));

        ast_node_t *right = parsear_pipe(tokens);

        left = crear_nodo_operador(op, left, right);

    }

    return left;
}

ast_node_t *parsear_secuencia(Dequeue *tokens){

    if(!tokens || getSize(tokens) == 0) return NULL;

    ast_node_t *left = parsear_logicos(tokens);

    while (getSize(tokens) > 0 && ((token_t*)getAt(tokens, 0))->type == 1) {

        if(strcmp(obtener_valor(tokens), ";") != 0) break;
        
        node_type_t op = obtener_tipo(tokens);

        liberar_token((token_t*)pop_front(tokens));

        ast_node_t *right = parsear_logicos(tokens);

        left = crear_nodo_operador(op, left, right);

    }

    return left;
}

ast_node_t *crear_arbol_de_ejecucion(Dequeue *tokens){

    if(!tokens || getSize(tokens) == 0) return NULL;

    ast_node_t *root = parsear_secuencia(tokens);

    freedq(tokens, free_token);
        
    return root;
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
        } else if(linea[i] == '&' || linea[i] == '|' || linea[i] == ';' || linea[i] == '>' || linea[i] == '<' || linea[i] == '(' || linea[i] == ')') {
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

    return tokens;
}
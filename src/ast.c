#include "ast.h"
#include <stdlib.h>

command_t *crear_comando(char **argv, char *input, char* output, int append, int bg){

    command_t* comando = (command_t*)malloc(sizeof(command_t));

    if(!comando) return NULL;

    comando->argv = argv;
    comando->input_file = input;
    comando->output_file = output;
    comando->append_output = append;
    comando->background = bg;

    return comando;
}

ast_node_t* crear_nodo_comando(command_t *comando){

    ast_node_t* nodo = (ast_node_t*)malloc(sizeof(ast_node_t));

    if(!nodo) return NULL;

    nodo->data.cmd = comando;
    nodo->type = NODE_COMMAND;

    return nodo;
}

ast_node_t* crear_nodo_operador(node_type_t type, ast_node_t *left, ast_node_t* right){

    ast_node_t *nodo = (ast_node_t*)malloc(sizeof(ast_node_t));

    if(!nodo) return NULL;

    nodo->type = type;
    nodo->data.op.left = left;
    nodo->data.op.right = right;

    return nodo;
}

void liberar_ast(ast_node_t *nodo){

    if(!nodo) return;

    if(nodo->type == NODE_COMMAND) {

        command_t *cmd = nodo->data.cmd;

        if(cmd){

            if(cmd->argv) {
                for(int i = 0; cmd->argv[i]; i++) free(cmd->argv[i]);
                free(cmd->argv);
            }

            free(cmd->input_file);
            free(cmd->output_file);
            free(cmd);
        }

    } else {
        liberar_ast(nodo->data.op.left);
        liberar_ast(nodo->data.op.right);
    }

    free(nodo);
}
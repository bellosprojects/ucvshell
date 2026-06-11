#include "ast.h"
#include "jobs.h"
#ifndef EJECUTAR_AST_H
#define EJECUTAR_AST_H

/// @brief Ejecuta el árbol de sintaxis abstracta que contiene los comandos escritos en la terminal
/// @param nodo nodo del arbol ast
/// @param jobs lista de jobs foreground
void ejecutar_ast(ast_node_t *nodo, Dequeue* jobs);

#endif
#include "ast.h"
#include "jobs.h"
#ifndef EJECUTAR_AST_H
#define EJECUTAR_AST_H


/// @brief Ejecuta un arbol de sintaxis abstrata que representa una linea de comandos estructurada
/// @param nodo Apuntador al nodo raiz
/// @param jobs tabla de procesos
void ejecutar_ast(ast_node_t *nodo);

#endif
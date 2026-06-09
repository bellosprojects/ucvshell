#ifndef COMMAND_UTILS
#define COMMAND_UTILS
#include "ast.h"
#include "dequeue.h"

#define MAX_COMMAND_LENGTH 128

/// @brief Crea un arbol de ejecucion basado en la lista de tokens
/// @param tokens La lista de tokens a procesar
/// @return Apuntador al arbol de ejecucion
ast_node_t *crear_arbol_de_ejecucion(Dequeue *tokens);

/// @brief Crea una lista de tokens basada en la linea ingresada por el usuario
/// @param Linea La linea introducida por el usuario (a procesar)
/// @return Lista de tokens para crear el ast
Dequeue *tokentizar(char *linea);

#endif
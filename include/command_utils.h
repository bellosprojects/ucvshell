#ifndef COMMAND_UTILS
#define COMMAND_UTILS
#include "ast.h"
#include "dequeue.h"

/// @brief Separa el comado en programa y sus argumentos
/// @param comando (in) comando original del usuario
/// @param ruta_programa (out) donde se almacena la direccion real del programa
/// @param args (out) donde se almacena la lista de argumentos
void extraer_partes_de_comando(char *comando, char *ruta_programa, char **args);

/// @brief Crea un arbol de ejecucion basado en la lista de tokens
/// @param tokens La lista de tokens a procesar
/// @return Apuntador al arbol de ejecucion
ast_node_t *crear_arbol_de_ejecucion(Dequeue *tokens);

/// @brief Crea una lista de tokens basada en la linea ingresada por el usuario
/// @param Linea La linea introducida por el usuario (a procesar)
/// @return Lista de tokens para crear el ast
Dequeue *tokentizar(char *linea);

#endif
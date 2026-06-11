#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include "dequeue.h"

#ifndef BUILTINS_H
#define BUILTINS_H

/**
 * @brief Verifica si un comando es un comando builtin
 * @param command El comando a verificar
 * @return 1 si es un comando builtin, 0 en caso contrario
 */
int is_builtin(const char* command);

/**
 * @brief Ejecuta el comando builtin 'cd'
 * @param args Argumentos del comando, donde args[0] es "cd" y args[1] es el destino
 * @return 0 si se ejecutó correctamente, 1 en caso de error
 */
int ejecutar_cd(char **args);

/**
 * @brief Ejecuta el comando builtin 'pwd'
 * @return 0 si se ejecutó correctamente, 1 en caso de error
 */
int pwd();

/**
 * @brief Ejecuta el comando builtin 'fg' para traer un proceso en segundo plano al primer plano
 * @param args Argumentos del comando, donde args[0] es "fg" y args[1] puede ser un job ID o PID
 * @param jobs La lista de jobs en segundo plano
 * @return void
 */
void ejecutar_fg(char **args, Dequeue* jobs);

#endif
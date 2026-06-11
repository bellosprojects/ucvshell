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
int ejecutar_cd(char **args);
int pwd();
void ejecutar_fg(char **args, Dequeue* jobs);

#endif
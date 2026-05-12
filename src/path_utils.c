#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "path_utils.h"

char* find_command_path(const char* command) {
    // 1. Si el comando ya es una ruta absoluta o relativa (contiene '/')
    if (strchr(command, '/') != NULL) {
        // Verificar si el archivo existe y tiene permisos de ejecución
        if (access(command, X_OK) == 0) {
            // Devolver una copia (el caller la liberará)
            char* result = malloc(strlen(command) + 1);
            if (result) {
                strcpy(result, command);
            }
            return result;
        }
        return NULL;  // No existe o no es ejecutable
    }
    
    // 2. Obtener la variable de entorno $PATH
    char* path_env = getenv("PATH");
    if (path_env == NULL) {
        return NULL;  // No hay PATH definido
    }
    
    // 3. Copiar PATH porque strtok lo modifica
    char* path_copy = malloc(strlen(path_env) + 1);
    if (path_copy == NULL) {
        return NULL;
    }
    strcpy(path_copy, path_env);
    
    // 4. Tokenizar por ':'
    char* dir = strtok(path_copy, ":");
    char* full_path = NULL;
    
    while (dir != NULL) {
        // Construir la ruta completa: directorio + '/' + comando
        size_t dir_len = strlen(dir);
        size_t cmd_len = strlen(command);
        
        // +2 por el '/' y el '\0'
        char* candidate = malloc(dir_len + cmd_len + 2);
        if (candidate == NULL) {
            free(path_copy);
            return NULL;
        }
        
        sprintf(candidate, "%s/%s", dir, command);
        
        // Verificar si existe y es ejecutable
        if (access(candidate, X_OK) == 0) {
            full_path = candidate;
            break;  // Encontrado!
        }
        
        free(candidate);  // No funcionó, probar siguiente directorio
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return full_path;  // NULL si no encontró nada
}
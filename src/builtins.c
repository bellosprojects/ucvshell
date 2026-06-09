#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "builtins.h"

int is_builtin(const char* command) {
    const char* builtins[] = {"cd", "pwd", "exit", "history", "help", NULL};
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(command, builtins[i]) == 0) {
            return 1; 
        }
    }
    return 0;
}

int ejecutar_cd(char **args) {
    char cwd_actual[PATH_MAX];
    char nueva_ruta[PATH_MAX];
    char *destino = args[1];

    if(args[2] != NULL) {
        fprintf(stderr, "ucvsh: cd: too many arguments\n");
        return 1;
    }

    // Obtener el directorio actual antes de cambiar (para OLDPWD)
    if (getcwd(cwd_actual, sizeof(cwd_actual)) == NULL) {
        perror("ucvsh: error en getcwd");
        return 1;
    }

    //"cd " o "cd ~" va al directorio HOME
    if (destino == NULL || strcmp(destino, "~") == 0) {
        destino = getenv("HOME");
        if (destino == NULL) {
            fprintf(stderr, "ucvsh: cd: HOME no está definido\n");
            return 1;
        }
    } 
    //"cd ~/ruta" HOME +Ruta
    else if (strncmp(destino, "~/", 2) == 0) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "ucvsh: cd: HOME no está definido\n");
            return 1;
        }
        snprintf(nueva_ruta, sizeof(nueva_ruta), "%s%s", home, destino + 1);
        destino = nueva_ruta;
    } 
    //"cd -" vuelve al directorio anterior
    else if (strcmp(destino, "-") == 0) {
        destino = getenv("OLDPWD");
        if (destino == NULL) {
            fprintf(stderr, "ucvsh: cd: OLDPWD no está definido\n");
            return 1;
        }
        // Si hacemos "cd -" imprime la ruta
        printf("%s\n", destino);
    }

    // cambio de directorio (Maneja ".." y ".")
    if (chdir(destino) != 0) {
        perror("ucvsh: cd");
        return 1;
    }

    // Actualizar variables de entorno PWD y OLDPWD
    setenv("OLDPWD", cwd_actual, 1);
    //Obtengo actual y actualizo PWD
    if (getcwd(cwd_actual, sizeof(cwd_actual)) != NULL) {
        setenv("PWD", cwd_actual, 1);
    }

    return 0;
}

int pwd() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        perror("ucvsh: pwd");
        return 1;
    }
}
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include "signals_.h"
#include "dequeue.h"
#include "jobs.h"
#include "builtins.h"
#define NUM_BUILTINS (sizeof(tabla_ayuda) / sizeof(HelpCommand))

typedef struct {
    const char *nombre;
    const char *descripcion;
    const char *sintaxis;
} HelpCommand;

// comandos builtins implementados
static HelpCommand tabla_ayuda[] = {
    {"cd",      "Cambia el directorio actual de trabajo de la shell.", "cd [directorio]"},
    {"pwd",     "Imprime la ruta absoluta del directorio de trabajo actual.", "pwd"},
    {"history", "Muestra la lista de los comandos ejecutados recientemente.", "history"},
    {"jobs",    "Lista todos los trabajos (jobs) activos en segundo plano (background).", "jobs"},
    {"fg",      "Trae un trabajo en segundo plano al primer plano (foreground).", "fg <job_id>"},
    {"help",    "Muestra información sobre los comandos internos de la shell.", "help [comando]"}
};

int ejecutar_help(char **argv) {
    // "help sin más argumentos"
    if (argv[1] == NULL) {
        printf("\n--- ucvshell, version 1.0.0 ---\n");
        printf("Estos comandos de shell son definidos internamente.\n");
        printf("Escribe 'help <comando>' para ver los detalles de un comando especifico.\n\n");
        printf("%-12s %s\n", "Comando", "Descripcion");
        printf("-----------------------------------------------------------------\n");
        
        for (size_t i = 0; i < NUM_BUILTINS; i++) {
            printf("%-12s %s\n", tabla_ayuda[i].nombre, tabla_ayuda[i].descripcion);
        }
        printf("\n");
        return 0;
    }

    // "help [comando]"
    for (size_t i = 0; i < NUM_BUILTINS; i++) {
        if (strcmp(argv[1], tabla_ayuda[i].nombre) == 0) {
            printf("\nComando:       %s\n", tabla_ayuda[i].nombre);
            printf("Descripcion:   %s\n", tabla_ayuda[i].descripcion);
            printf("Uso:           %s\n", tabla_ayuda[i].sintaxis);
            return 0;
        }
    }

    // comando no existe en los builtins
    fprintf(stderr, "help: No es un builtin '%s'. Intenta con otro comando.\n", argv[1]);
    return 1;
}

int is_builtin(const char* command) {
    const char* builtins[] = {"cd", "pwd", "exit", "history", "help", "jobs", "fg", NULL};
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

void ejecutar_fg(char **args, Dequeue* jobs) {
    if (jobs == NULL || getSize(jobs) == 0) {
        fprintf(stderr, "fg: no hay trabajos en segundo plano\n");
        return;
    }

    Job* job_actual = NULL;

    if (args[1] == NULL) {
        job_actual = obtener_ultimo_job(jobs);
    } else if (args[1][0] == '%') {
        int job_id = atoi(&args[1][1]);
        job_actual = buscar_job_por_id(jobs, job_id);
    } else {
        int pid_directo = atoi(args[1]);
        job_actual = buscar_job_por_pid(jobs, pid_directo);
    }

    if (job_actual == NULL) {
        fprintf(stderr, "fg: trabajo no encontrado: %s\n",
                args[1] ? args[1] : "actual");
        return;
    }

    pid_t pid_hijo = job_actual->pid;
    printf("%s\n", job_actual->command);
    fflush(stdout);

    // Bloquear SIGCHLD para que el manejador no coseche este proceso
    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, &old_mask);

    // Reanudar el proceso (grupo de procesos)
    if (kill(-pid_hijo, SIGCONT) < 0) {
        perror("fg: error al enviar SIGCONT");
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
        return;
    }

    // Ceder la terminal al hijo — usar su process group
    pid_t pgid = getpgid(pid_hijo);
    if (pgid == -1) pgid = pid_hijo;
    if (tcsetpgrp(STDIN_FILENO, pgid) == -1) {
        perror("fg: tcsetpgrp");
        // continuamos de todas formas
    }

    // Desbloquear SIGCHLD para poder recibir la señal, pero manejar race con waitpid
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    int status;
    pid_t w;
    do {
        w = waitpid(pid_hijo, &status, WUNTRACED);
    } while (w == -1 && errno == EINTR);

    // Recuperar la terminal para la shell — SIEMPRE, pase lo que pase
    tcsetpgrp(STDIN_FILENO, getpgrp());

    if (w == -1) {
        if (errno == ECHILD) {
            // El manejador SIGCHLD ya repleó al hijo; consultar estado del job
            if (job_actual->status == FINALIZADO) {
                eliminar_job(jobs, pid_hijo);
            } else if (job_actual->status == SUSPENDIDO) {
                printf("\n[%d]+  Stopped\t%s\n", job_actual->id, job_actual->command);
            }
        } else {
            perror("fg: waitpid");
        }
        return;
    }

    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        eliminar_job(jobs, pid_hijo);
    } else if (WIFSTOPPED(status)) {
        job_actual->status = SUSPENDIDO;
        printf("\n[%d]+  Stopped\t%s\n", job_actual->id, job_actual->command);
    }
}

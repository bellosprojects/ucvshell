#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <signal.h>
#include "signals_.h"
#include "dequeue.h"
#include "jobs.h"
#include "builtins.h"

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

    // Reanudar el proceso
    if (kill(-pid_hijo, SIGCONT) < 0) {
        perror("fg: error al enviar SIGCONT");
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
        return;
    }

    // Ceder la terminal al hijo
    tcsetpgrp(STDIN_FILENO, pid_hijo);

    // Desbloquear SIGCHLD ANTES de esperar (para no perder señales)
    sigprocmask(SIG_SETMASK, &old_mask, NULL);

    int status;
    waitpid(pid_hijo, &status, WUNTRACED);

    // Recuperar la terminal para la shell — SIEMPRE, pase lo que pase
    tcsetpgrp(STDIN_FILENO, getpgrp());

    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        eliminar_job(jobs, pid_hijo);
    } else if (WIFSTOPPED(status)) {
        job_actual->status = SUSPENDIDO;
        printf("\n[%d]+  Detenido\t%s\n", job_actual->id, job_actual->command);
    }
}
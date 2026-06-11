#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "signals_.h"
#include "jobs.h"

void manejador_señales(int sig) {
    int old_errno = errno;
    int status;
    pid_t pid;

    Dequeue *jobs = obtener_jobs();
    if (jobs == NULL) {
        errno = old_errno;
        return;
    }

    /* Reap and update status for any child state changes. Use WNOHANG|WUNTRACED|WCONTINUED
       so that we detect exits, stops and continues. */
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        for (int i = 0; i < getSize(jobs); i++) {
            Job *job = (Job *)getAt(jobs, i);
            if (job->pid == pid) {
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    job->status = FINALIZADO;
                    job->mostrado = 0;
                } else if (WIFSTOPPED(status)) {
                    job->status = SUSPENDIDO;
                } else if (WIFCONTINUED(status)) {
                    job->status = EJECUTANDO;
                }
                break;
            }
        }
    }

    errno = old_errno;
}

void configurar_señales(){
    struct sigaction sa;
    
    // Configurar SIGCHLD
    sa.sa_handler = manejador_señales;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
    
    // ESTRUCTURA PARA IGNORAR SEÑALES EN EL PROMPT DE FORMA SEGURA
    struct sigaction sa_ign;
    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags = SA_RESTART; // <-- CRÍTICO: Evita que el prompt se rompa al presionar las teclas
    sigemptyset(&sa_ign.sa_mask);

    // Configurar SIGTSTP (Ctrl+Z) para IGNORAR de verdad de forma segura
    sigaction(SIGTSTP, &sa_ign, NULL);
    
    // Configurar SIGINT (Ctrl+C) para IGNORAR de forma segura
    sigaction(SIGINT, &sa_ign, NULL);
    
    // Configurar SIGQUIT (Ctrl+\) para IGNORAR de forma segura
    sigaction(SIGQUIT, &sa_ign, NULL);
    
    // Configurar señales de terminal
    sigaction(SIGTTOU, &sa_ign, NULL);
    sigaction(SIGTTIN, &sa_ign, NULL);
}
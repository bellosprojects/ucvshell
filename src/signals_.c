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
    
    // Configurar SIGTSTP (Ctrl+Z) para IGNORAR en la shell
    struct sigaction sa_tstp;
    sa_tstp.sa_handler = SIG_IGN;  // Ignorar la señal
    sa_tstp.sa_flags = 0;
    sigemptyset(&sa_tstp.sa_mask);
    sigaction(SIGTSTP, &sa_tstp, NULL);
    
    // Configurar SIGINT (Ctrl+C) para IGNORAR en la shell
    struct sigaction sa_int;
    sa_int.sa_handler = SIG_IGN;
    sa_int.sa_flags = 0;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);
    
    // Configurar SIGQUIT (Ctrl+\) para IGNORAR
    struct sigaction sa_quit;
    sa_quit.sa_handler = SIG_IGN;
    sa_quit.sa_flags = 0;
    sigemptyset(&sa_quit.sa_mask);
    sigaction(SIGQUIT, &sa_quit, NULL);
    
    // Configurar señales de terminal
    struct sigaction sa_tty;
    sa_tty.sa_handler = SIG_IGN;
    sa_tty.sa_flags = 0;
    sigemptyset(&sa_tty.sa_mask);
    sigaction(SIGTTOU, &sa_tty, NULL);
    sigaction(SIGTTIN, &sa_tty, NULL);
}
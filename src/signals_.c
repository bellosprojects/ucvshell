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
    sa.sa_handler = manejador_señales;
    /* Want to receive notifications for stop/continue as well, so DO NOT set SA_NOCLDSTOP */
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);

    signal(SIGINT, SIG_IGN);  //ctrl c
    signal(SIGTSTP, SIG_IGN);  //ctrl z
    signal(SIGQUIT, SIG_IGN);  //ctrl \ //
    /* Evitar que la shell sea detenida al tocar la terminal desde background */
    signal(SIGTTOU, SIG_IGN); // ignore background write-to-tty
    signal(SIGTTIN, SIG_IGN); // ignore background read-from-tty
}
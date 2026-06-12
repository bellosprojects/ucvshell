#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "signals_.h"
#include "jobs.h"

void manejador_senales(int sig) {
    int old_errno = errno;
    int status;
    pid_t pid;

    Dequeue *jobs = obtener_jobs();
    if (jobs == NULL) {
        errno = old_errno;
        return;
    }

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

void configurar_senales(){
    struct sigaction sa;
    
    sa.sa_handler = manejador_senales;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
    
    struct sigaction sa_ign;
    sa_ign.sa_handler = SIG_IGN;
    sa_ign.sa_flags = SA_RESTART;
    sigemptyset(&sa_ign.sa_mask);

    sigaction(SIGTSTP, &sa_ign, NULL);
    
    sigaction(SIGINT, &sa_ign, NULL);
    
    sigaction(SIGQUIT, &sa_ign, NULL);
    
    // Configurar señales de terminal
    sigaction(SIGTTOU, &sa_ign, NULL);
    sigaction(SIGTTIN, &sa_ign, NULL);
}
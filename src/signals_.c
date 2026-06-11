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

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Buscar el job en la lista y actualizar su estado
        Dequeue *jobs = obtener_jobs();
        if (jobs == NULL) continue;

        for (int i = 0; i < getSize(jobs); i++) {
            Job *job = (Job *)getAt(jobs, i);
            if (job->pid == pid) {
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    job->status = FINALIZADO;
                    job->mostrado = 0;
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
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    //sigaction(SIGCHLD, &sa, NULL);

    signal(SIGINT, SIG_IGN);  //ctrl c
    signal(SIGTSTP, SIG_IGN);  //ctrl z
    signal(SIGQUIT, SIG_IGN);  //ctrl \ //
}
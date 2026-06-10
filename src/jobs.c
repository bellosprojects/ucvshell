#include "dequeue.h"
#include "jobs.h"
#include <unistd.h>

int agregar_job(Dequeue* jobs, const char* command, int pid, Status status){
    Job* new_job = (Job*)malloc(sizeof(Job));
    if (!new_job) return -1; // Error de memoria
    
    new_job->id = getSize(jobs) +1;
    new_job->command = strdup(command); // Duplicar el comando para evitar problemas de
    new_job->pid = pid;
    new_job->status = status;

    push_back(jobs, new_job);
    return 0;
}

int eliminar_job(Dequeue* jobs, int pid){
    for (int i = 0; i < getSize(jobs); i++) {
        Job* job = (Job*)getAt(jobs, i);
        if (job->pid == pid) {
            Job* aux = removeAt(jobs, i);
            liberar_job(aux);
            return 0; // Éxito
        }
    }
    return 1;
}

int actualizar_status(Dequeue* jobs){
    int status;
    for (int i = 0; i < getSize(jobs); i++) {
        // WNOHANG verifica el estado inmediatamente sin bloquear la shell
        Job* new_job = (Job*)getAt(jobs, i);
        pid_t result = waitpid(new_job->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        
        if (result > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                // El proceso terminó
                new_job->status = FINALIZADO;
            } else if (WIFSTOPPED(status)) {
                // El proceso fue suspendido (ej: Ctrl+Z)
                new_job->status = SUSPENDIDO;
            } else if (WIFCONTINUED(status)) {
                // El proceso reanudó su marcha
                new_job->status = EJECUTANDO;
            }
        } else if (result == -1) {
            // Si da -1, el proceso ya no existe o hubo un error
            new_job->status = FINALIZADO;
        }
    }

    // Limpiar de la tabla los que ya finalizaron de forma definitiva
    for (int i = 0; i < getSize(jobs); i++) {
        Job* new_job = (Job*)getAt(jobs, i);
        if (new_job->status == FINALIZADO) {
            eliminar_job(jobs, new_job->pid);
        }
    }
    return 0;
}

void printJobs(void* dato){
    Job* job = (Job*)dato;
    const char* state_str;
    switch (job->status) {
        case EJECUTANDO: state_str = "Running"; break;
        case SUSPENDIDO: state_str = "Stopped"; break;
        case FINALIZADO: state_str = "Done"; break;
        default: state_str = "Unknown"; break;
    }
    printf("[%d]  %d %-20s %s\n", job->id, job->pid, state_str, job->command);
}

int listar_jobs(Dequeue* jobs){
    actualizar_status(jobs);
    if (getSize(jobs) == 0) {
        return 0;
    }
    print(jobs, printJobs);
    return 0;
}

void liberar_job(Job* job){
    if(job){
        free(job->command);
    }
    free(job);
}


#include "dequeue.h"
#include "jobs.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

Dequeue *jobs_global = NULL;

Dequeue* obtener_jobs(){
    if(jobs_global == NULL){
        jobs_global = crear_dequeue();
    }
    return jobs_global;
}

int agregar_job(Dequeue* jobs, const char* command, int pid, Status status){
    Job* new_job = (Job*)malloc(sizeof(Job));
    if (!new_job) return -1;
    
    new_job->id = getSize(jobs) +1;
    new_job->command = (char*)strdup(command);
    new_job->pid = pid;
    new_job->status = status;
    new_job->mostrado = 0;

    push_back(jobs, new_job);
    return 0;
}

int eliminar_job(Dequeue* jobs, int pid){
    if (jobs == NULL) return 1;

    for (int i = 0; i < getSize(jobs); i++) {
        Job* job = (Job*)getAt(jobs, i);
        if (job->pid == pid) {
            Job* aux = removeAt(jobs, i);
            liberar_job(aux);
            return 0;
        }
    }
    return 1;
}

int actualizar_status(Dequeue* jobs) {
    int status;
    for (int i = 0; i < getSize(jobs); i++) {
        Job* job = (Job*)getAt(jobs, i);
        pid_t result = waitpid(job->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        
        if (result > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                job->status = FINALIZADO;
                job->mostrado = 0;
            } else if (WIFSTOPPED(status)) {
                job->status = SUSPENDIDO;
            } else if (WIFCONTINUED(status)) {
                job->status = EJECUTANDO;
            }
        } else if (result == -1) {
            job->status = FINALIZADO;
            job->mostrado = 0;
        }
    }
    return 0;
}

void printJobs(void* dato) {
    Job* job = (Job*)dato;
    const char* state_str;
    switch (job->status) {
        case EJECUTANDO: state_str = "Running"; break;
        case SUSPENDIDO: state_str = "Stopped"; break;
        case FINALIZADO: state_str = "Done"; break;
        default: state_str = "Unknown"; break;
    }
    printf("[%d]  %d %-20s %s\n", job->id, job->pid, state_str, job->command);
    
    if (job->status == FINALIZADO) {
        job->mostrado = 1;
    }
}

int listar_jobs(Dequeue* jobs) {
    actualizar_status(jobs);
    
    if (getSize(jobs) == 0) {
        return 0;
    }
    
    print(jobs, printJobs); 
    
    for (int i = getSize(jobs) - 1; i >= 0; i--) {
        Job* job = (Job*)getAt(jobs, i);
        
        if (job->status == FINALIZADO && job->mostrado) {

            Job* aux = removeAt(jobs, i); 
            liberar_job(aux); 
        }
    }
    
    return 0;
}

void liberar_job(void* node){
    Job *job = (Job*)node;
    if(job){
        free(job->command);
    }
    free(job);
}

Job* obtener_ultimo_job(Dequeue* jobs) {
    if (jobs == NULL || getSize(jobs) == 0) return NULL;
    return (Job*)getAt(jobs, getSize(jobs) - 1);
}

Job* buscar_job_por_id(Dequeue* jobs, int id) {
    for (int i = 0; i < getSize(jobs); i++) {
        Job* job = (Job*)getAt(jobs, i);
        if (job->id == id) return job;
    }
    return NULL;
}

Job* buscar_job_por_pid(Dequeue* jobs, int pid) {
    for (int i = 0; i < getSize(jobs); i++) {
        Job* job = (Job*)getAt(jobs, i);
        if (job->pid == pid) return job;
    }
    return NULL;
}
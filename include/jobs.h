#include "dequeue.h"
#include <unistd.h>

#ifndef JOBS_H
#define JOBS_H

typedef enum Status {
    EJECUTANDO,
    SUSPENDIDO,
    FINALIZADO
} Status;

typedef struct Job {
    int id;             // ID del trabajo
    char* command;      // Comando asociado al trabajo
    // char[COMMAND_SIZE] command;
    int pid;           // PID del proceso asociado al trabajo
    int mostrado;
    Status status;        // Estado del trabajo (ejecutando, detenido, etc.)
} Job;

int agregar_job(Dequeue* jobs, const char* command, int pid, Status status);
int eliminar_job(Dequeue* jobs, int pid);
int actualizar_status(Dequeue* jobs);
int listar_jobs(Dequeue* jobs);
void liberar_job(void* job);

Dequeue* obtener_jobs();

#endif
#include "dequeue.h"
#include <unistd.h>

#ifndef JOBS_H
#define JOBS_H

/// @brief Identificador del estado de un trabajo en segundo plano
typedef enum Status {
    EJECUTANDO,
    SUSPENDIDO,
    FINALIZADO
} Status;

/// @brief Trabajo en segundo plano
typedef struct Job {
    int id;             // ID del trabajo
    char* command;      // Comando asociado al trabajo
    int pid;           // PID del proceso asociado al trabajo
    int mostrado;
    Status status;        // Estado del trabajo (ejecutando, detenido, etc.)
} Job;

/**
 * @brief Agrega un nuevo trabajo a la lista de trabajos en segundo plano
 * @param jobs La lista de trabajos
 * @param command El comando asociado al trabajo
 * @param pid El PID del proceso asociado al trabajo
 * @param status El estado inicial del trabajo
 * @return 0 si se agregó correctamente, -1 en caso de error
 */
int agregar_job(Dequeue* jobs, const char* command, int pid, Status status);

/**
 * @brief Elimina un trabajo de la lista de trabajos en segundo plano dado su PID
 * @param jobs La lista de trabajos
 * @param pid El PID del proceso asociado al trabajo a eliminar
 * @return 0 si se eliminó correctamente, 1 si no se encontró el trabajo o en caso de error
 */
int eliminar_job(Dequeue* jobs, int pid);

/**
 * @brief Actualiza el estado de los trabajos en segundo plano verificando su estado actual
 * @param jobs La lista de trabajos
 * @return 0 si se actualizó correctamente, 1 en caso de error
 */
int actualizar_status(Dequeue* jobs);

/**
 * @brief Lista los trabajos en segundo plano, mostrando su estado actual y eliminando los finalizados
 * @param jobs La lista de trabajos
 * @return 0 si se listó correctamente, 1 en caso de error
 */
int listar_jobs(Dequeue* jobs);

/**
 * @brief Libera la memoria asociada a un trabajo
 * @param job El trabajo a liberar
 */
void liberar_job(void* job);

/// @brief Crea o devuelve si ya existe un apuntador a la tabla global de procesos usando SINGLETON
/// @return Apuntador a la tabla de procesos
Dequeue* obtener_jobs();

#endif
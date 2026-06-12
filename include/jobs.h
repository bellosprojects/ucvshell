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

/**
 * @brief Obtiene el último trabajo agregado a la lista de trabajos en segundo plano
 * @param jobs La lista de trabajos
 * @return Un puntero al trabajo encontrado, o NULL si no se encontró
 */
Job* obtener_ultimo_job(Dequeue* jobs);

/**
 * @brief Busca un trabajo en la lista de trabajos en segundo plano dado su ID
 * @param jobs La lista de trabajos
 * @param id El ID del trabajo a buscar
 * @return Un puntero al trabajo encontrado, o NULL si no se encontró
 */
Job* buscar_job_por_id(Dequeue* jobs, int id);

/**
 * @brief Busca un trabajo en la lista de trabajos en segundo plano dado su PID
 * @param jobs La lista de trabajos
 * @param pid El PID del proceso asociado al trabajo a buscar
 * @return Un puntero al trabajo encontrado, o NULL si no se encontró
 */
Job* buscar_job_por_pid(Dequeue* jobs, int pid);

/**
 * @brief Obtiene la lista global de trabajos en segundo plano (singleton), creando una nueva si no existe
 * @return Un puntero a la lista de trabajos
 */
Dequeue* obtener_jobs();

#endif
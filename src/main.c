#include <stdio.h>
#include <stdlib.h>
#include "shell_loop.h"
#include "historial.h"
#include "jobs.h"
#include "signals_.h"

int main(int argc, char *argv[]){

    // - Manejo de señales
    
    Historial* historial = obtener_historial();
    Dequeue* jobs = obtener_jobs();
    
    configurar_señales();
    
    shell_loop(historial, jobs);
    liberar_historial(historial, liberar_nodo_linea);
    freedq(jobs, liberar_job);

    return 0;
}
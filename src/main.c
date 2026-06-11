#include <stdio.h>
#include <stdlib.h>
#include "shell_loop.h"
#include "historial.h"
#include "jobs.h"
#include "signals_.h"

void liberar_nodo_linea(void* dato) {
    if (dato == NULL) return;
    linea* l = (linea*)dato;
    if (l->comando != NULL) {
        free(l->comando);
    }
    free(l);
}

int main(int argc, char *argv[]){
    
    Historial* historial = crear_historial();
    Dequeue* jobs = obtener_jobs();
    
    configurar_señales();
    
    shell_loop(historial, jobs);
    
    liberar_historial(historial, liberar_nodo_linea);
    freedq(jobs, liberar_job);

    return 0;
}
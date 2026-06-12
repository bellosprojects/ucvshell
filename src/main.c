#include <stdio.h>
#include <stdlib.h>
#include "shell_loop.h"
#include "historial.h"
#include "jobs.h"
#include "signals_.h"

int main(int argc, char *argv[]){

    /**
     *  Archivo de entrada a UCV Shell
     *  Inicializacion del Historial y la tabla de Jobs
     *  Configuracion de las señales
     */
    
    Historial* historial = obtener_historial();
    Dequeue* jobs = obtener_jobs();
    
    configurar_senales();

    /**
     *  Invocacion al loop de shell que coloca la terminal en modo no cononico
     *  Al finalizar libera las tablas previamente declaradas
     */
    
    shell_loop(historial);
    liberar_historial(historial, liberar_nodo_linea);
    freedq(jobs, liberar_job);

    return 0;
}
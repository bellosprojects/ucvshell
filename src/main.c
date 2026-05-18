#include <stdio.h>
#include <stdlib.h>
#include "shell_loop.h"
#include "historial.h"
#include <signal.h>

int main(int argc, char *argv[]){

    // - Manejo de señales
    signal(SIGSTOP, SIG_IGN);

    // Por hacer
    // - Tabla de Jobs
    Historial* historial = crear_historial();
    
    shell_loop(historial);

    liberar_historial(historial, free);

    // Por hacer
    // - Liberar memoria (Jobs)

    return 0;
}
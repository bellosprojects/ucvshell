#include <stdio.h>
#include <stdlib.h>
#include "shell_loop.h"
#include "historial.h"
#include "../include/signals.h"

void liberar_nodo_linea(void* dato) {
    if (dato == NULL) return;
    linea* l = (linea*)dato;
    if (l->comando != NULL) {
        free(l->comando);
    }
    free(l);
}

int main(int argc, char *argv[]){

    // - Manejo de señales
    configurar_señales();

    // Por hacer
    // - Tabla de Jobs
    Historial* historial = crear_historial();
    
    shell_loop(historial);
    liberar_historial(historial, liberar_nodo_linea);

    // Por hacer
    // - Liberar memoria (Jobs)

    return 0;
}
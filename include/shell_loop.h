#include "historial.h"

#ifndef SHELL_LOOP_H
#define SHELL_LOOP_H

typedef enum {
    TECLA_LETRA_NORMAL,
    TECLA_ARRIBA,
    TECLA_ABAJO,
    TECLA_DERECHA,
    TECLA_IZQUIERDA,
    TECLA_ENTER,
    TECLA_BORRAR,
    TECLA_DESCONOCIDA
} TipoTecla;

int lecturaInmediata();
TipoTecla procesarEntrada(int *caracter);
/// @brief Loop de ucvsh 
void shell_loop(Historial *historial);

#endif
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

/// @brief Funcion encargada de cambiar el tipo de lectura para que se lea de caracter a caracter inmediatamente
/// @return El caracter ingresado en la terminal en formato entero
int lecturaInmediata();



/// @brief Funcion encargada de procesar la entrada del usuario y determinar el tipo de tecla presionada
/// @param caracter Puntero al caracter ingresado en la terminal
/// @param jobs Lista donde se almacenan los procesos foreground
/// @return El tipo de tecla presionada
TipoTecla procesarEntrada(int *caracter, Dequeue* jobs);


/// @brief Loop de ucvsh que se encarga de manejar los distintos tipos de tecla ingresados y ejecutar sus acciones respectivas
/// @param historial Lista donde se almacenan los comandos ingresados
/// @param jobs Lista donde se almacenan los procesos foreground
void shell_loop(Historial *historial, Dequeue* jobs);

#endif
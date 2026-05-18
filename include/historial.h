#include <stdio.h>
#include <stdlib.h>
#include "dequeue.h"

extern const char* HISTORIAL_FILE;

#ifndef HISTORIAL_H
#define HISTORIAL_H

typedef struct Historial Historial;

Historial* crear_historial();

void guardar_historial(Historial* historial);
void cargar_historial(Dequeue* dq);

void actualizar_valor(Historial* historial, char* nuevo_valor);
void agregar_historial(Historial* historial, char* dato);

void* obtener_historial_anterior(Historial* historial); //flecha hacia arriba
void* obtener_historial_posterior(Historial* historial); //flecha hacia abajo

void imprimir_historial(Historial* historial, void (*printFunc)(void*));

void liberar_historial(Historial* historial, void (*freeFunc)(void*));

#endif

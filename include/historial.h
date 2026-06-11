#include <stdio.h>
#include <stdlib.h>
#include "dequeue.h"
#define COMMAND_SIZE 256
extern const char* HISTORIAL_FILE;

#ifndef HISTORIAL_H
#define HISTORIAL_H

typedef struct Historial Historial;

typedef struct linea{
    int num;
    char* comando;
} linea;

/// @brief Crear un nuevo historial
/// @return Apuntando al nuevo historial creado
Historial* crear_historial();

/// @brief Guarda el historial en un archivo
/// @param historial 
void guardar_historial(Historial* historial);

/// @brief Carga desde un archivo al historial en memoria
/// @param dq Apuntador a la lista de comandos del historial
void cargar_historial(Dequeue* dq);

/// @brief Actualiza el valor del nodo actual del historial por otro
/// @param historial Apuntandor al historial
/// @param nuevo_valor Nuevo valor
void actualizar_valor(Historial* historial, char* nuevo_valor);

/// @brief Agrega al frente del hsitorial un nuevo comando
/// @param historial Apuntandor al historial
/// @param dato 
void agregar_historial(Historial* historial, char* dato);

//flecha hacia arriba

/// @brief Obtiene el valor del comando anterior al actual del historial
/// @param historial Apuntandor al historial
/// @return Apuntador al payload del nodo anterior al actual del historial, o NULL si no existe
void* obtener_historial_anterior(Historial* historial); 

//flecha hacia abajo

/// @brief Obtiene el valor del comando posterior al actual del historial
/// @param historial Apuntandor al historial
/// @return Apuntador al payload del nodo posterior al actual del historial, o NULL si no existe
void* obtener_historial_posterior(Historial* historial);

/// @brief Imprime el historial
/// @param historial Apuntandor al historial
/// @param printFunc Funcion personalizada para imprimir el payload de cada nodo
void imprimir_historial(Historial* historial);


/// @brief Liberar la memoria del historial
/// @param historial Apuntandor al historial
/// @param freeFunc función personalizada para liberar el payload de cada nodo
void liberar_historial(Historial* historial, void (*freeFunc)(void*));

#endif

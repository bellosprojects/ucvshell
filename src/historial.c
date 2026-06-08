#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dequeue.h"
#include "historial.h"
#define COMMAND_SIZE 256
const char* HISTORIAL_FILE = "../historial.txt";

typedef struct Historial {
    Dequeue* dq;
    Nodo* cursor_actual;
} Historial;

typedef struct linea{
    int num;
    char* comando;
} linea;

void cargar_historial(Dequeue* dq);

Historial* crear_historial() {
    Historial* historial = (Historial*)malloc(sizeof(Historial));
    if (!historial) return NULL;
    
    historial->dq = crear_dequeue();
    historial->cursor_actual = NULL;
    cargar_historial(historial->dq); // Cargar datos del archivo a la lista
    return historial;
}

void actualizar_valor(Historial* historial, char* nuevo_valor){
    if (!historial || !nuevo_valor) return;
    if (historial->cursor_actual == NULL) return; // No hay comando seleccionado

    // Liberar el valor anterior para evitar fugas de memoria
    free(historial->cursor_actual->dato);
    
    linea * nueva_linea = (linea*)malloc(sizeof(linea));
    nueva_linea->num = ((linea*)historial->cursor_actual->dato)->num; // Mantener el mismo número
    nueva_linea->comando = strdup(nuevo_valor); // Duplicar el nuevo comando
    historial->cursor_actual->dato = nueva_linea;
}

void agregar_historial(Historial* historial, char* dato) {
    if (!historial || !dato) return;
    
    // Comparar con el último para evitar duplicados seguidos
    char* ultimo = getAt(historial->dq, 0); 
    if (ultimo && strcmp(ultimo, dato) == 0) return;
    
    // IMPORTANTE: Duplicar la memoria porque 'linea' en shell_loop es local
    linea * nueva_linea = (linea*)malloc(sizeof(linea));
    nueva_linea->num = getSize(historial->dq) + 1; // Asignar número secuencial
    nueva_linea->comando = strdup(dato); // Duplicar el comando para evitar problemas de memoria
    //char* copia = strdup(dato);
    push_front(historial->dq, nueva_linea);
    
    historial->cursor_actual = NULL; // Resetear navegación

    FILE* f = fopen(HISTORIAL_FILE, "a");
    if (f) {
        fprintf(f, "%s\n", nueva_linea->comando);
        fclose(f);
    }
}

void cargar_historial(Dequeue* dq) {
    FILE* file = fopen(HISTORIAL_FILE, "r");
    if (file == NULL) return; // Si no existe, no hay nada que cargar

    char line[COMMAND_SIZE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Limpiar salto de línea
        
        if (strlen(line) > 0) {
            // Insertamos al inicio para mantener orden cronológico al leer
            linea * nueva_linea = (linea*)malloc(sizeof(linea));
            nueva_linea->num = getSize(dq) + 1; // Asignar número secuencial
            nueva_linea->comando = strdup(line); // Duplicar el comando para evitar
            push_front(dq, nueva_linea);
        }
    }
    fclose(file);
}

void guardar_historial(Historial* historial) {
    if (!historial) return;

    // Usamos "w" para sobrescribir con el estado actual de la lista
    FILE* file = fopen(HISTORIAL_FILE, "w");
    if (file == NULL) {
        perror("Error al guardar historial");
        return;
    }

    // Recorrer la lista (usando tu función getSize y getAt)
    for(int i = 0; i < getSize(historial->dq); i++) {
        char* str = (char*)getAt(historial->dq, i);
        if (str) fprintf(file, "%s\n", str);
    }

    fclose(file);
}

void liberar_historial(Historial* historial, void (*freeFunc)(void*)) {
    if (historial == NULL) return;
    
    freedq(historial->dq, freeFunc);
    free(historial);
}

//UpArrow
void* obtener_historial_anterior(Historial* historial) {
    if (historial == NULL ) return NULL;
    if(historial->dq->head == NULL) return NULL;

    // Si es la primera vez que presionamos "Arriba"
    if (historial->cursor_actual == NULL) {
        historial->cursor_actual = historial->dq->head;
    } 
    // Si no es el final de la lista, avanzamos al siguiente (más antiguo)
    else if (historial->cursor_actual->next != NULL) {
        historial->cursor_actual = historial->cursor_actual->next;
    }

    return ((linea*)historial->cursor_actual->dato)->comando; // Retornar el comando del nodo actual
}

//Down Arrow
void* obtener_historial_posterior(Historial* historial) {
    if (historial == NULL) return NULL;
    if (historial->cursor_actual == NULL) return "";
    // Si bajamos, vamos hacia el 'prev' (más reciente)
    if (historial->cursor_actual->prev != NULL) {
        historial->cursor_actual = historial->cursor_actual->prev;
        return ((linea*)historial->cursor_actual->dato)->comando;
    } else {
        // Si llegamos al inicio (lo más reciente) y bajamos una vez más, 
        // el cursor vuelve a NULL (línea vacía para escribir algo nuevo)
        historial->cursor_actual = NULL;
        return ""; 
    }
}

void imprimir_historial(Historial* historial, void (*printFunc)(void*)){
    print(historial->dq, printFunc);
}
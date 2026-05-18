#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dequeue.h"
#include "historial.h"

const char* HISTORIAL_FILE = ".ucvsh_history";

typedef struct Historial {
    Dequeue* dq;
    Nodo* cursor_actual;
} Historial;

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
    
    // Asignar el nuevo valor (duplicado para seguridad)
    historial->cursor_actual->dato = strdup(nuevo_valor);
}

void agregar_historial(Historial* historial, char* dato) {
    if (!historial || !dato) return;
    
    // Comparar con el último para evitar duplicados seguidos
    char* ultimo = getAt(historial->dq, 0); 
    if (ultimo && strcmp(ultimo, dato) == 0) return;
    
    // IMPORTANTE: Duplicar la memoria porque 'linea' en shell_loop es local
    char* copia = strdup(dato);
    push_front(historial->dq, copia);
    
    historial->cursor_actual = NULL; // Resetear navegación

    FILE* f = fopen(HISTORIAL_FILE, "a");
    if (f) {
        fprintf(f, "%s\n", copia);
        fclose(f);
    }
}

void cargar_historial(Dequeue* dq) {
    FILE* file = fopen(HISTORIAL_FILE, "r");
    if (file == NULL) return; // Si no existe, no hay nada que cargar

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Limpiar salto de línea
        
        if (strlen(line) > 0) {
            // Insertamos al inicio para mantener orden cronológico al leer
            push_front(dq, strdup(line));
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
    if(historial->dq->head == NULL) return "";

    // Si es la primera vez que presionamos "Arriba"
    if (historial->cursor_actual == NULL && historial->dq->head != NULL) {
        historial->cursor_actual = historial->dq->head;
    } 
    // Si no es el final de la lista, avanzamos al siguiente (más antiguo)
    else if (historial->cursor_actual->next != NULL) {
        historial->cursor_actual = historial->cursor_actual->next;
    }

    return historial->cursor_actual->dato;
}

//Down Arrow
void* obtener_historial_posterior(Historial* historial) {
    if (historial == NULL) return NULL;
    if (historial->cursor_actual == NULL) return "";
    // Si bajamos, vamos hacia el 'prev' (más reciente)
    if (historial->cursor_actual->prev != NULL) {
        historial->cursor_actual = historial->cursor_actual->prev;
        return historial->cursor_actual->dato;
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
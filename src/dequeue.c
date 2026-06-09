#include <stdio.h>
#include <stdlib.h>
#include "dequeue.h"

Dequeue* crear_dequeue(){
    Dequeue* dq = (Dequeue*)malloc(sizeof(Dequeue));
    if(dq == NULL) return NULL;
    dq->head = NULL; 
    dq->tail = NULL;
    dq->size = 0;
    return dq;
}

Nodo* crearNodo(void* dato){
    Nodo* newNode = (Nodo*) malloc(sizeof(Nodo));
    if(newNode == NULL) return NULL;
    newNode->dato = dato;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

//------------------ Insertar ------------------------------------------

void push_front(Dequeue* dq, void* dato){
    Nodo* newNode = crearNodo(dato);
    newNode->next =  dq->head;
    newNode->prev = NULL;
    if(dq->head != NULL)
        dq->head->prev = newNode;
        
    dq->head = newNode;

    if(dq->tail == NULL)
        dq->tail = newNode;
    dq->size++;
}

void push_back(Dequeue* dq, void* dato){
    Nodo* newNode = crearNodo(dato);
    newNode->prev = dq->tail;
    newNode->next = NULL;

    if(dq->size == 0){
        dq->head = newNode;
        dq->tail = newNode;
    } else {
        dq->tail->next = newNode;
        dq->tail = newNode;
    }
    dq->size++;
}

//------------------ Buscar ------------------------------------------
void* getAt(Dequeue* dq, int index){
    if(index < 0 || index >= dq->size){
        return NULL; // Index out of bounds
    }
    Nodo* current = dq->head;
    for(int i = 0; i < index; i++){
        current = current->next;
    }
    return current->dato;
}

void* find(Dequeue* dq, void* dato, int (*cmpFunc)(void*, void*)){
    Nodo* current = dq->head;
    while(current != NULL){
        if(cmpFunc(current->dato, dato) == 0){
            return current->dato; 
        }
        current = current->next;
    }
    return NULL; 
}

//------------------ Eliminar ------------------------------------------

void* pop_front(Dequeue* dq) {
    if (dq == NULL || dq->head == NULL) return NULL;

    Nodo* temp = dq->head;
    void* data = temp->dato;

    dq->head = temp->next;
    
    if (dq->head != NULL) {
        dq->head->prev = NULL;
    }

    free(temp);
    dq->size--;
    return data;
}

void* removeAt(Dequeue* dq, int index){
    if (dq == NULL || index < 0 || index >= dq->size) return NULL;

    Nodo* current = dq->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    void* data = current->dato;

    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        dq->head = current->next; // Si es el primer nodo
    }

    if (current->next != NULL) {
        current->next->prev = current->prev;
    } else {
        dq->tail = current->prev; // Si es el último nodo
    }

    free(current);
    dq->size--;
    return data;
}

int getSize(Dequeue* dq){
    return dq->size;
}

//-----------------------------------------------------------



void print(Dequeue* dq, void (*printFunc)(void*)){
    Nodo* current = dq->head;
    while(current != NULL){
        printFunc(current->dato);
        current = current->next;
    }
}

void freedq(Dequeue* dq, void (*freeFunc)(void*)) {
    if (dq == NULL) return;
    
    Nodo* current = dq->head;
    while (current != NULL) {
        Nodo* next = current->next;
        if (freeFunc != NULL) {
            freeFunc(current->dato); // Liberar el contenido genérico
        }
        free(current); // Liberar el nodo
        current = next;
    }
    free(dq);
}
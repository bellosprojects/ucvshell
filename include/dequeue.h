#include <stdio.h>
#include <stdlib.h>

#ifndef DEQUEUE_H
#define DEQUEUE_H
/// @brief Struct Nodo para la lista doblemente enlazada
typedef struct Nodo{
    void * dato;
    struct Nodo* next;
    struct Nodo* prev;
} Nodo;


/// @brief Lista doblemente enlazada
typedef struct Dequeue{
    Nodo* head;
    Nodo* tail;
    int size;
} Dequeue;

/// @brief Constructor de la lista
/// @return nodo cabeza de la lista
Dequeue* crear_dequeue();

/// @brief Funcion encargada de añadir un nuevo nodo al comienzo de la lista
/// @param dq Lista a trabajar
/// @param dato Atributo dato para crear el nuevo nodo a añadir 
void push_front(Dequeue* dq, void* dato);

/// @brief Funcion encargada de añadir un nuevo nodo al final de la lista
/// @param dq Lista a trabajar
/// @param dato Atributo dato para crear el nuevo nodo a añadir
void push_back(Dequeue* dq, void* dato);

/// @brief Funcion encargada de obtener el tamaño de la lista
/// @param dq Lista a trabajar
/// @return Tamaño de la lista
int getSize(Dequeue* dq);

/// @brief Funcion encargada de obtener un nodo en una posicion especifica
/// @param dq Lista a trabajar
/// @param index Posicion del nodo a obtener
/// @return Puntero al nodo en la posicion especifica
void* getAt(Dequeue* dq, int index);

/// @brief Funcion encargada de eliminar un nodo en una posicion especifica
/// @param dq Lista a trabajar
/// @param index Posicion del nodo a eliminar
/// @return Puntero al nodo eliminado
void* removeAt(Dequeue* dq, int index);

/// @brief Funcion encargada de buscar un nodo con un valor especifico
/// @param dq Lista a trabajar
/// @param dato Valor a buscar
/// @param cmpFunc Funcion de comparacion
/// @return Puntero al nodo encontrado o NULL si no se encuentra
void* find(Dequeue* dq, void* dato, int (*cmpFunc)(void*, void*));

/// @brief Funcion encargada de eliminar el primer nodo de la lista
/// @param dq Lista a trabajar
/// @return Puntero al nodo eliminado
void* pop_front(Dequeue* dq);

/// @brief Funcion encargada de imprimir los elementos de la lista
/// @param dq Lista a trabajar
/// @param printFunc Funcion para imprimir cada elemento
void print(Dequeue* dq, void (*printFunc)(void*));

/// @brief Funcion encargada de liberar la memoria de la lista
/// @param dq Lista a trabajar
/// @param freeFunc Funcion para liberar la memoria de cada elemento
void freedq(Dequeue* dq, void (*freeFunc)(void*));

#endif
#include <stdio.h>
#include <stdlib.h>

#ifndef DEQUEUE_H
#define DEQUEUE_H

typedef struct Nodo{
    void * dato;
    struct Nodo* next;
    struct Nodo* prev;
} Nodo;

typedef struct Dequeue{
    Nodo* head;
    Nodo* tail;
    int size;
} Dequeue;

Dequeue* crear_dequeue();

void push_front(Dequeue* dq, void* dato);
void push_back(Dequeue* dq, void* dato);

int getSize(Dequeue* dq);
void* getAt(Dequeue* dq, int index);
void* removeAt(Dequeue* dq, int index);
void* find(Dequeue* dq, void* dato, int (*cmpFunc)(void*, void*));
void* pop_front(Dequeue* dq);
void print(Dequeue* dq, void (*printFunc)(void*));
void freedq(Dequeue* dq, void (*freeFunc)(void*));


#endif
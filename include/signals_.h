#ifndef SIGNALS_H 
#define SIGNALS_H 

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

void manejador_señales(int sig);
void configurar_señales();

#endif
#ifndef SIGNALS_H 
#define SIGNALS_H 

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


/// @brief Funcion encargada de manejar las señales para que no afecten a la terminal sino a los procesos ejecutados en esta
void manejador_senales(int sig);

/// @brief Funcion encargada de configurar las señales para que se manejen correctamente
void configurar_senales();

#endif
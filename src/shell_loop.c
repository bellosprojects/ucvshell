#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include "shell_loop.h"
#include "path_utils.h"
#include "trim.h"
#include "command_utils.h"

#define PROMPT "ucvsh> "

/*
Recibo la lista y el tamaño, si se presiona la flecha de arriba o la de abajo tengo que restar y sumar respectivamente verificando los limites
para detecto la tecla, borro la linea, imprimo el prompt y luego coloco el ultimo comando de la lista
*/

int lecturaInmediata(){
    struct termios viejo, nuevo;
    int ch;
    
    if (tcgetattr(STDIN_FILENO, &viejo) < 0) {
        return 0; 
    }
    
    nuevo = viejo;
    
    nuevo.c_lflag &= ~(ICANON | ECHO); 
    
    nuevo.c_cc[VMIN] = 1; 
    nuevo.c_cc[VTIME] = 0; 
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &nuevo) < 0) {
        return 0;
    }
    
    ch = getchar(); 
    
    tcsetattr(STDIN_FILENO, TCSANOW, &viejo); 
    
    return ch;
}

TipoTecla procesarEntrada(int *caracter){
    int primer_caracter= lecturaInmediata();
    if(primer_caracter==27){
        int segundo_caracter= lecturaInmediata();
        int tercer_caracter= lecturaInmediata();
        if(segundo_caracter==91){
            switch(tercer_caracter){
                case 65: return TECLA_ARRIBA;
                case 66: return TECLA_ABAJO;
                case 67: return TECLA_DERECHA;
                case 68: return TECLA_IZQUIERDA;
                default: return TECLA_DESCONOCIDA;
            }
        }
        return TECLA_DESCONOCIDA;
    }

    if(primer_caracter==10||primer_caracter==13){
        return TECLA_ENTER;
    }

    if(primer_caracter==127||primer_caracter==8){
        return TECLA_BORRAR;
    }

    *caracter= primer_caracter;
    return TECLA_LETRA_NORMAL;
}

int procesar_linea(char *linea){

    Dequeue *tokens = tokentizar(linea);
    ast_node_t *ast = crear_arbol_de_ejecucion(tokens);

    /// ejecutar_arbol(ast);

    freedq(tokens, free);

    return 0;
}

void shell_loop(Historial *historial){
    
    int caracter_leido=0;
    char buffer_comando[256]= "";
    int tamano_cadena=0;
    int posicion_cursor=0;

    char comando_temporal[256] = "";
    int editando_temporal = 1;

    printf("%s", PROMPT);
    fflush(stdout);


    while(1){

        TipoTecla tecla = procesarEntrada(&caracter_leido);

        switch(tecla){
            case TECLA_ARRIBA: 

                if(editando_temporal){

                    editando_temporal = 0;
                    buffer_comando[posicion_cursor] = '\0';
                    strcpy(comando_temporal, buffer_comando);
                }

                actualizar_valor(historial, buffer_comando);
                memset(buffer_comando, 0, sizeof(buffer_comando));
                strcpy(buffer_comando, obtener_historial_anterior(historial));
                
                tamano_cadena= strlen(buffer_comando);
                posicion_cursor= tamano_cadena;

                printf("\r\033[K");
                printf("%s%s", PROMPT, buffer_comando);
                fflush(stdout);
                break;

            case TECLA_ABAJO:

                actualizar_valor(historial, buffer_comando);
                memset(buffer_comando, 0, sizeof(buffer_comando));

                char *linea_historial = obtener_historial_posterior(historial);
                
                if(strcmp(linea_historial, "") != 0){
                
                    printf("\r\033[K");
                    printf("%s%s", PROMPT, linea_historial);
                    strcpy(buffer_comando, linea_historial);
                    tamano_cadena = strlen(buffer_comando);
                    
                } else {
                    //Es el finalll

                    printf("\r\033[K");
                    printf("%s%s", PROMPT, comando_temporal);

                    strcpy(buffer_comando, comando_temporal);
                    tamano_cadena = strlen(buffer_comando);
                    
                    editando_temporal = 1;

                }
                
                posicion_cursor = tamano_cadena;
                fflush(stdout);
                break;

            case TECLA_DERECHA:
                if(posicion_cursor<tamano_cadena){
                    posicion_cursor++;
                    printf("\033[C");
                    fflush(stdout);
                }
                break;

            case TECLA_IZQUIERDA:
                if(posicion_cursor>0){
                    posicion_cursor--;
                    printf("\033[D");
                    fflush(stdout);
                }
                break;

            case TECLA_BORRAR:
                if(tamano_cadena>0 && posicion_cursor>0){
                    posicion_cursor--;

                    for(int i= posicion_cursor; i< tamano_cadena-1; i++){
                        buffer_comando[i] = buffer_comando[i+1];
                    }
                    tamano_cadena--;
                    buffer_comando[tamano_cadena] = '\0';

                    printf("\033[D");
                    printf("\033[K");

                    printf("%s", &buffer_comando[posicion_cursor]);
                    
                    int caracteres_a_regresar= tamano_cadena-posicion_cursor;
                    for(int i=0; i<caracteres_a_regresar; i++){
                        printf("\033[D");
                    }
                    fflush(stdout);
                }
                break;

            case TECLA_LETRA_NORMAL: 

                for (int i = tamano_cadena; i > posicion_cursor; i--) {
                    buffer_comando[i] = buffer_comando[i - 1];
                }
                buffer_comando[posicion_cursor] = caracter_leido;
                tamano_cadena++;
                posicion_cursor++;

                printf("\033[K");
        
                printf("%s", &buffer_comando[posicion_cursor - 1]);

                int mover_atras = tamano_cadena - posicion_cursor;
                for (int i = 0; i < mover_atras; i++) printf("\033[D");
                fflush(stdout);
                break;

            case TECLA_ENTER:
                buffer_comando[tamano_cadena]= '\0';
                trim(buffer_comando);

                printf("\n"); 
                if(!strcmp(buffer_comando, "exit")){
                    guardar_historial(historial);
                    return;  
                } 
                
                if(strcmp(buffer_comando, "history") == 0){
                    imprimir_historial(historial);
                    //guardar_historial(historial);
                    return;
                }

                if (tamano_cadena > 0){
                    agregar_historial(historial, buffer_comando);

                    procesar_linea(buffer_comando);

                }
                tamano_cadena=0;
                posicion_cursor=0;
                memset(buffer_comando, 0, sizeof(buffer_comando)); 
                printf("%s", PROMPT);
                fflush(stdout);
                break;

            case TECLA_DESCONOCIDA:

                break;
        }
    }
}
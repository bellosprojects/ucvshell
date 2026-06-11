#include "ast.h"
#include <fcntl.h>    
#include <unistd.h>   
#include <sys/types.h>
#include <sys/wait.h> 
#include <stdio.h>    
#include <stdlib.h>  
#include <string.h>
#include <signal.h>
#include "signals_.h"
#include "builtins.h"
#include "jobs.h"
#include "historial.h"

void ejecutar_ast(ast_node_t *nodo);

static int last_exit_status = 0;
void set_last_exit_status(int status) {
    last_exit_status = status;
}

int get_last_exit_status() {
    return last_exit_status;
}

void ejecutar_and(ast_node_t *nodo){
    ejecutar_ast(nodo->data.op.left);
    if(get_last_exit_status() == 0){
        ejecutar_ast(nodo->data.op.right);
    }
}

void ejecutar_or(ast_node_t *nodo){
    ejecutar_ast(nodo->data.op.left);
    if(get_last_exit_status() != 0){
        ejecutar_ast(nodo->data.op.right);
    }
}

void ejecutar_secuencia(ast_node_t *nodo){
    ejecutar_ast(nodo->data.op.left);
    ejecutar_ast(nodo->data.op.right);
}

void ejecutar_pipe(ast_node_t *nodo){
    int fd[2];
    if (pipe(fd) == -1) {
        perror("Error al crear pipe");
        return;
    }

    pid_t pid1= fork();
    if(pid1==0){
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        ejecutar_ast(nodo->data.op.left);
        exit(get_last_exit_status());
    }

    pid_t pid2= fork();
    if(pid2==0){
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        ejecutar_ast(nodo->data.op.right);
        exit(get_last_exit_status());
    }

    close(fd[0]);
    close(fd[1]);
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    set_last_exit_status(WEXITSTATUS(status2));
}


void ejecutar_comando(command_t *cmd){
    if(is_builtin(cmd->argv[0])){
        if(strcmp(cmd->argv[0], "cd") == 0){
            set_last_exit_status(ejecutar_cd(cmd->argv));
        } else if(strcmp(cmd->argv[0], "pwd") == 0){
            set_last_exit_status(pwd());
        } else if(strcmp(cmd->argv[0], "history") == 0){
            imprimir_historial((Historial *)obtener_historial());
            set_last_exit_status(1);
        } else if(strcmp(cmd->argv[0], "help") == 0){
            //implementar help
            fprintf(stderr, "this is the help command \n");
            set_last_exit_status(1);
        } else if(strcmp(cmd->argv[0], "jobs") == 0){
            //implementar jobs
            listar_jobs((Dequeue *)obtener_jobs());
            set_last_exit_status(1);
        
        } else if (strcmp(cmd->argv[0], "fg") == 0) {
            ejecutar_fg(cmd->argv, obtener_jobs());
            set_last_exit_status(0);
        } else {
            fprintf(stderr, "Comando builtin no implementado\n");
            set_last_exit_status(1);
        }
        return;
    }

    pid_t pid = fork();

    if(pid==-1){
        perror("Error al crear proceso hijo");
        return;
    }

    if(pid==0){

        setpgid(0, 0);
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);

        if(cmd->input_file){
            int fd= open(cmd->input_file, O_RDONLY);
            if(fd==-1){
                perror("Error al abrir archivo de entrada");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if(cmd->output_file){
            int fd;
            if(cmd->append_output){
                fd= open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                fd= open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if(fd==-1){
                perror("Error al abrir archivo de salida");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }


        execvp(cmd->argv[0], cmd->argv);
        perror("Error al ejecutar comando");
        exit(1);
    }
    else{
        if(!cmd->background){ //foreground

            pid_t shell_gpit = getpgrp();
            pid_t child_gpid = pid;

            sigset_t mask, old_mask;

            sigemptyset(&mask);
            sigaddset(&mask, SIGTSTP);
            sigprocmask(SIG_BLOCK, &mask, &old_mask);

            if(child_gpid != shell_gpit)
            tcsetpgrp(STDERR_FILENO, child_gpid);

            int status;
            waitpid(pid, &status, WUNTRACED);

            tcsetpgrp(STDIN_FILENO, shell_gpit);

            if (WIFEXITED(status)) {
                set_last_exit_status(WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                set_last_exit_status(128 + WTERMSIG(status));
            }
        } else {
            if(obtener_jobs() != NULL){
                agregar_job(obtener_jobs(), cmd->argv[0], pid, EJECUTANDO);
                printf("[%d] %d\n", getSize(obtener_jobs()), pid);
            } else {
                fprintf(stderr, "Error: Estructura de jobs no inicializada.\n");
            }
        }
        return;
    }
}
void ejecutar_ast(ast_node_t *nodo){

    if(!nodo) return;

    switch (nodo->type) {
        case NODE_COMMAND:
            ejecutar_comando(nodo->data.cmd);
            break;
        case NODE_PIPE:
            ejecutar_pipe(nodo);
            break;
        case NODE_AND:
            ejecutar_and(nodo);
            break;
        case NODE_OR:
            ejecutar_or(nodo);
            break;
        case NODE_SEMICOLON:
            ejecutar_secuencia(nodo);
            break;
        default:
            fprintf(stderr, "Tipo de nodo desconocido\n");
    }
}

//SEÑALES
//JOBLIST


/*if (cmd->background) {
    pid_t pid = fork();
    if (pid == 0) {
        // ... hijo: redirige stdin a /dev/null y ejecuta ...
    } else {
        // ... padre: NO hace waitpid. 
        // Agrega el job a la lista:
        agregar_job(pid, cmd->name);
        printf("[%d] %d\n", num_job, pid); // Típico formato de shell
    }
}*/
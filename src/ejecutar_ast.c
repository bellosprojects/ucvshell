#include "../include/ast.h"
#include <fcntl.h>    
#include <unistd.h>   
#include <sys/types.h>
#include <sys/wait.h> 
#include <stdio.h>    
#include <stdlib.h>  
#include <signal.h>
#include "../include/signals.h"


void ejecutar_comando(command_t *cmd){
    //if(es_builtin(cmd->argv[0])){
    //    return ejecutar_builtin(cmd);
    //}

    pid_t pid = fork();

    if(pid==-1){
        perror("Error al crear proceso hijo");
        return -1;
    }

    if(pid==0){
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

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        execvp(cmd->argv[0], cmd->argv);
        perror("Error al ejecutar comando");
        exit(1);
    }
    else{
        if(!cmd->background){
            int status;
            waitpid(pid, &status, 0);
            set_last_exit_status(WEXITSTATUS(status));
        }
        return 0;
    }
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
    pipe(fd);

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
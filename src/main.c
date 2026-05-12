#include <stdio.h>
#include <signal.h>

int main(int argc, char *argv[]){

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGSTOP, SIG_IGN);

    
    

    return 0;
}
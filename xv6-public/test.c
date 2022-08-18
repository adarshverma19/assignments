#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int processPIPEcmd(){
  int fd[2];
    pipe(fd);

    if(fork() == 0){
        close(1);
        dup(fd[1]);
       // printf(1,"The file id is : %d \n", id);
        close(fd[0]);
        close(fd[1]);

        char *arg[3];
        arg[0] = "cat";
        arg[1] = "README";
        arg[2] = 0; 

        exec(arg[0],arg);
        exit(0);
    }

    if(fork()== 0 ){
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        char *arg[2];
        arg[0] = "wc";
        arg[1] = 0; 
        exec(arg[0],arg);
        exit(0);
    }

    close(fd[0]);
    close(fd[1]);

    wait(0);
    wait(0);

    exit(0);
    return 0;
}

int main(void){

  processPIPEcmd();
  exit(0);

return 0;
    
}
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define CMDSIZE 100
#define READSIZE 300

struct line
{
    char inputchars[500];
    char *tokens[100];
    int size;
    int type;
    int flag[6];
};

// int parseCommand(struct line* command);
int inputredirection(struct line *command, int input);
int outputredirection(struct line *command, int output);
int ioredirection(struct line *command, int input);
int oiredirection(struct line *command, int output);
int succOrExecution(struct line *command, int position);
int succAndExecution(struct line *command, int position);
int parallelExecution(struct line *command, int position);
int pipeExecution(struct line *command, int position);

int debug(char *string)
{
    printf(1, "Inside Point : %s\n", string);
    return 0;
}

int prompt()
{
    printf(1, "MyShell>> ");
    return 0;
}

int printcmd(char *command[])
{

    for (int i = 0; i < 10; i++)
    {
        printf(1, "Command at %d : %s\n", i, command[i]);
    }

    return 0;
}

int printinput(char *input)
{
    for (int i = 0; i < CMDSIZE; i++)
    {
        printf(1, "Char At %d : %c\n", i, input[i]);
    }
    return 0;
}

int clearAll(char *input, char *command[])
{

    for (int i = 0; i < READSIZE; i++)
    {
        input[i] = 0;
    }

    for (int i = 0; i < CMDSIZE; i++)
    {
        command[i] = 0;
    }
    return 0;
}

int takeinput(char *input)
{
    gets(input, READSIZE);
    return 0;
}

struct line *readLine(char *letters)
{
    struct line *cmd = malloc(sizeof(struct line));
    strcpy(cmd->inputchars, letters);
    int totalwords = 0;
    char *let = letters;
    char tok[50];
    int index = 0;
    char temp[20][50];
    while (1)
    {
        if (*let == ' ' || *let == '\n' || *let == '\0')
        {
            tok[index] = '\0';
            strcpy(temp[totalwords], tok);
            cmd->tokens[totalwords] = temp[totalwords];
            totalwords++;
            index = 0;
            if (*let == '\n' || *let == '\0')
                break;
        }
        else
        {
            tok[index++] = *let;
        }
        let++;
    }
    cmd->size = totalwords;
    return cmd;
}

int executecmd(struct line *command)
{

    char type = 'A';

    //parrallel execution

    for (int i = 0; i < command->size; i++)
    {
        if (strcmp(command->tokens[i], ";") == 0)
        {
            command->tokens[i] = 0;
            type = 'L';
            parallelExecution(command, i);
            break;
        }
    }

    // successive AND execution
    if (type == 'A')
    {
        for (int i = 0; i < command->size; i++)
        {
            if (strcmp(command->tokens[i], "&&") == 0)
            {
                command->tokens[i] = 0;
                type = 'N';
                succAndExecution(command, i);
                break;
            }
        }
    }

    // successive OR execution

    if (type == 'A')
    {
        for (int i = 0; i < command->size; i++)
        {
            if (strcmp(command->tokens[i], "||") == 0)
            {
                command->tokens[i] = 0;
                type = 'O';
                succOrExecution(command, i);
                break;
            }
        }
    }

    //pipe execution

    if (type == 'A')
    {

        for (int j = 0; j < command->size; j++)
        {
            if (strcmp(command->tokens[j], "|") == 0)
            {

                command->tokens[j] = 0;
                type = 'P';
                // debug("for loop");
                // printcmd(command->tokens);
                pipeExecution(command, j);
                break;
            }
        }
    }

    // int input = 0;
    // int output = 0;

    if (type == 'A')
    {
        for (int i = 0; i <= command->size; i++)
        {
            if (strcmp(command->tokens[i], "<") == 0)
            {
                // input = i;
                command->tokens[i] = 0;
                type = 'L';
                ioredirection(command, i);
            }
            if (strcmp(command->tokens[i], ">") == 0)
            {
                // output = i;
                command->tokens[i] = 0;
                type = 'R';
                oiredirection(command, i);
            }
        }
    }

    // if (type == 'L')
    // {
        
    // }

    // if (type == 'R')
    // {
    //     oiredirection(command, output);
    // }

    if (type == 'A')
    {
        if (fork() == 0)
        {
            exec(command->tokens[0], command->tokens);
            exit(0);
        }

        wait(0);
    }

    return 0;
}

int main(void)
{
    char input[READSIZE];
    while (1)
    {
        prompt();
        takeinput(input);
        struct line *command = readLine(input);
        if(strcmp(command->tokens[0],"exit")==0){
            exit(0);
        }
        executecmd(command);
        clearAll(input, command->tokens);
    }

    exit(0);

    return 0;
}


int inputredirection(struct line *command, int position)
{
    struct line lhs;


    int llen = 0;
    for (int i = 0; i <= position; i++)
    {
        lhs.tokens[i] = command->tokens[i];
        llen++;
    }
    lhs.size = llen - 1;


    if (fork() == 0)
    {
        close(0);
        printf(1, "File name : %s \n", command->tokens[position + 1]);
        int infile = open(command->tokens[position + 1], O_RDONLY);
        dup(infile);
        close(infile);
        executecmd(&lhs);
        exit(0);
        //    exec(lhs.tokens[0],lhs.tokens);
    }

    wait(0);
    return 0;
}

int outputredirection(struct line *command, int output)
{
    debug("ouput redirecction");
    int ouputdes = open(command->tokens[output + 1], O_CREATE | O_RDWR);
    close(1);
    dup(ouputdes);
    close(ouputdes);

    return 0;
}

int ioredirection(struct line *command, int input)
{

    int flag = -1;
    for (int i = input + 1; i <= command->size; i++)
    {
        if (strcmp(command->tokens[i], ">") == 0)
        {
            flag = i;
            break;
        }
    }

  
command->size = input;

    if (fork() == 0)
    {
        int inputdes = open(command->tokens[input + 1], O_RDONLY);
        //debug(command->tokens[input + 1]);

        if(flag!=-1){
            int outputdes = open(command->tokens[flag+1], O_CREATE | O_RDWR);
        //debug(command->tokens[input + 1]);
        close(1);
        dup(outputdes);
        close(outputdes);
        }

        close(0);
        dup(inputdes);
        close(inputdes);
        executecmd(command);
        exit(0);
    }

    wait(0);
 

    return 0;
}


int oiredirection(struct line *command, int output)
{
    
    command->size = output;
    if (fork() == 0)
    {
       int outputdes = open(command->tokens[output+1], O_CREATE | O_RDWR);
        //debug(command->tokens[input + 1]);
        close(1);
        dup(outputdes);
        close(outputdes);
        executecmd(command);
        exit(0);
    }

    wait(0);
   
    return 0;
}



int bothioexec(struct line *command, int input , int output)
{
    

    command->size = output;
    if (fork() == 0)
    {


       int outputdes = open(command->tokens[output+1], O_CREATE | O_RDWR);
        //debug(command->tokens[input + 1]);
        close(1);
        dup(outputdes);
        close(outputdes);
        executecmd(command);
        exit(0);
    }

    wait(0);
   
    return 0;
}



int succOrExecution(struct line *command, int position)
{
    struct line lhs;
    struct line rhs;
    int llen = 0;
    for (int i = 0; i <= position; i++)
    {
        lhs.tokens[i] = command->tokens[i];
        llen++;
    }
    lhs.size = llen - 1;

    // debug("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
    // printf(1, "LHS size : %d\n", lhs.size);
    // printcmd(lhs.tokens);

    int j = 0;
    int rlen = 0;
    for (int i = position + 1; i <= command->size; i++)
    {
        rhs.tokens[j++] = command->tokens[i];
        rlen++;
    }
    rhs.size = rlen - 1;

    // debug("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
    // printf(1, "RHS size : %d\n", rhs.size);
    // printcmd(rhs.tokens);

    int status;
    if (fork() == 0)
    {
        executecmd(&lhs);
        exit(0);
    }

    wait(&status);

    if (status != 0)
    {

        if (fork() == 0)
        {
            executecmd(&rhs);
            exit(0);
        }
        wait(0);
    }

    return 0;
}

int succAndExecution(struct line *command, int position)
{
    struct line lhs;
    struct line rhs;
    int llen = 0;
    for (int i = 0; i <= position; i++)
    {
        lhs.tokens[i] = command->tokens[i];
        llen++;
    }
    lhs.size = llen - 1;

    // debug("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
    // printf(1, "LHS size : %d\n", lhs.size);
    // printcmd(lhs.tokens);

    int j = 0;
    int rlen = 0;
    for (int i = position + 1; i <= command->size; i++)
    {
        rhs.tokens[j++] = command->tokens[i];
        rlen++;
    }
    rhs.size = rlen - 1;
    // debug("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
    // printf(1, "RHS size : %d\n", rhs.size);
    // printcmd(rhs.tokens);

    int status;

    if (fork() == 0)
    {
        executecmd(&lhs);
        exit(0);
    }

    wait(&status);

    if (status == 0)
    {
        if (fork() == 0)
        {
            executecmd(&rhs);
            exit(0);
        }
        wait(0);
    }

    return 0;
}

int parallelExecution(struct line *command, int position)
{
    struct line lhs;
    struct line rhs;
    int llen = 0;
    for (int i = 0; i <= position; i++)
    {
        lhs.tokens[i] = command->tokens[i];
        llen++;
    }
    lhs.size = llen - 1;

    // debug("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
    // printf(1, "LHS size : %d\n", lhs.size);
    // printcmd(lhs.tokens);

    int j = 0;
    int rlen = 0;
    for (int i = position + 1; i <= command->size; i++)
    {
        rhs.tokens[j++] = command->tokens[i];
        rlen++;
    }
    rhs.size = rlen - 1;
    // debug("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
    // printf(1, "RHS size : %d\n", rhs.size);
    // printcmd(rhs.tokens);

    if (fork() == 0)
    {
        executecmd(&lhs);
        exit(0);
    }

    wait(0);

    if (fork() == 0)
    {
        executecmd(&rhs);
        exit(0);
    }

    wait(0);
    return 0;
}

int pipeExecution(struct line *command, int position)
{
    struct line lhs;
    struct line rhs;
    int llen = 0;
    for (int i = 0; i <= position; i++)
    {
        lhs.tokens[i] = command->tokens[i];
        llen++;
    }
    lhs.size = llen - 1;

    // debug("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL");
    // printf(1, "LHS size : %d\n", lhs.size);
    // printcmd(lhs.tokens);

    int j = 0;
    int rlen = 0;
    for (int i = position + 1; i <= command->size; i++)
    {
        rhs.tokens[j++] = command->tokens[i];
        rlen++;
    }
    rhs.size = rlen - 1;
    // debug("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
    // printf(1, "RHS size : %d\n", rhs.size);
    // printcmd(rhs.tokens);

    int fd[2];
    pipe(fd);

    if (fork() == 0)
    {
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        executecmd(&lhs);
        exit(0);
        //    exec(lhs.tokens[0],lhs.tokens);
    }

    if (fork() == 0)
    {
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        executecmd(&rhs);
        exit(0);
        //    exec(rhs.tokens[0],rhs.tokens);
    }

    close(fd[0]);
    close(fd[1]);

    wait(0);
    wait(0);
    return 0;
}

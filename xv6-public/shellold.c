#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

//#include "processInfo.h"

#define CMDSIZE 100
#define READSIZE 300




// struct command {
//   char cmdString[50]; // for testing purpose
//   char *argv[10]; //at max 10 arguments are supported
//   char inputfile[20];
//   char outputfile[20];
//   int argc;
// };

struct line {
  char inputchars[500];
   char *tokens[100];
   int size;
   int type;
   int flag[6];
};


// Separates each argument from command input line
// if I/O redirection exist, it takes care of that too
// and returns a new "command" structure.
// struct command* somfunc(char *userInput) {
//   struct command *newCommand = malloc(sizeof(struct command));
//   strcpy(newCommand->cmdString, userInput);
//   strcpy(newCommand->inputfile, "");
//   strcpy(newCommand->outputfile, "");
//   int argCount = 0; // total count of arguments
//   char *ch = userInput;
//   char currWord[50];
//   int currWordIndex = 0;
//   char tempArgs[10][50];
//   while(1) {
//     if(*ch == '<' || *ch == '>') {
//       int flag;
//       if(*ch == '<') flag = 0; // inputfile
//       else if(*ch == '>') flag = 1; // outputfile
//       ch += 2;
//       currWordIndex=0;
//       while(*ch != ' ' && *ch != '\0' && *ch !='\n') {
//         currWord[currWordIndex++] = *ch;
//         ch++;
//       }
//       currWord[currWordIndex] = '\0';
//       if(!flag) strcpy(newCommand->inputfile, currWord);
//       else strcpy(newCommand->outputfile, currWord);
//       currWordIndex=0;
//       if(*ch == '\n' || *ch == '\0') break;
//     } else if(*ch == ' ' || *ch == '\n' || *ch == '\0') {
//       currWord[currWordIndex] = '\0';
//       // printf(1, "currWord: %s\n", currWord);
//       strcpy(tempArgs[argCount], currWord);
//       newCommand->argv[argCount] = tempArgs[argCount];
//       argCount++;
//       currWordIndex=0;
//       if(*ch == '\n' || *ch == '\0') break;
//     } else {
//       currWord[currWordIndex++] = *ch;
//     }
//     ch++;
//   }
//   newCommand->argc = argCount;
//   return newCommand;
// }






int printFlag(int *flag){
  for(int i= 0 ; i<6;i++){
    printf(1,"This is flag: %d : Position : %d \n",i, flag[i]);
  }

  return 0;
}


int printcmd(char *command[])
{

  for (int i = 0; i < CMDSIZE; i++)
  {
    // if (command[i] != 0)
    // {
      printf(1, "Command at %d : %s\n", i, command[i]);
    // }
  }
  return 0;
}

int printinput(char *input)
{

  for (int i = 0; i < CMDSIZE; i++)
  {
    // if (input[i] != 0)
    // {
      printf(1, "Char At %d : %c\n", i, input[i]);
    // }
  }
  return 0;
}

int clearAll(char *input, char *command[])
{
  
  for(int i = 0; i<READSIZE;i++){
    input[i] = 0;
  }

  for (int i = 0; i < CMDSIZE; i++)
  {
    command[i] = 0;
  }
  return 0;
}


int takeinput(char *input){
  gets(input, READSIZE);
    return 0 ; 
}


int constructargs(char *command[], char *args[], int start , int end){
  int k =0;
  for(int i =start; i<end; i++){
    args[k++] = command[i];
  }
  args[end] = 0;

  return 0; 
}





struct line* readLine(char *letters) {
  struct line *cmd = malloc(sizeof(struct line));
  strcpy(cmd->inputchars,letters);
  int totalwords = 0; // total count of arguments
  char *let = letters;
  char currWord[50];
  int currWordIndex = 0;
  char tempArgs[20][50];
  while(1) {
    if(*let == ' ' || *let == '\n' || *let == '\0') {
      currWord[currWordIndex] = '\0';
      strcpy(tempArgs[totalwords], currWord);
      cmd->tokens[totalwords] = tempArgs[totalwords];
      totalwords++;
      currWordIndex=0;
      if(*let == '\n' || *let == '\0') break;
    } else {
      currWord[currWordIndex++] = *let;
    }
    let++;
  }
  cmd->size = totalwords;
  return cmd;
}



















int toCommmandArray(char *input, struct line command)
{
  char *temp[CMDSIZE];
  char wr[READSIZE];
  char *inp;
  int partcount = 0;

  gets(wr, READSIZE);

  

  int n = 0;
  for (int i = 0; i < READSIZE; i++)
  {
    if (wr[i] != 10)
    {
      input[n++] = wr[i];
    }
    else
    {
      input[n++] = 0;
    }
  } 

    // printf(1,"This is input inside wr:11111111111111111111111111111 \n");

    // printinput(wr);

    // printf(1,"This is input inside input: 222222222222222222222222222222\n");

    // printinput(input);





  inp = input;
  temp[partcount++] = inp;

  //printinput(inp);
  char *ptr = inp;
  
  // char word[50];
  // int q=0;

  while (*ptr)
  { //check if the string is over
    if (*ptr == ' ')
    {
      *ptr = 0;
      // word[q++]= '\0';
      //strcpy(temp,word);

    temp[partcount++] = ptr + 1;
     
     
     
    }
    ptr++;
  }

  printf(1,"This is input inside input: 5555555555555555555555555555555555555\n");
  // printcmd(temp);


  // printinput(input);
  //     printf(1,"This is input inside input: 5555555555555555555555555555555555555\n");

  // printcmd(temp);

  int length = 0;
  for (int i = 0; i < partcount; i++)
  {

    if(strcmp(temp[i],"")!=0){
     strcpy(command.tokens[i],temp[i]);
     //command.tokens[i]
    }

    length++;
    // printf(1, " Command %d :  %s \n",i, temp[i]);

    // if (strcmp(command[i], " "))
    // {
    //   //printf(1,"Encounter space\n");
    //   command[k++] = temp[i];
    // }    
    if(strcmp(command.tokens[i],"|")==0){
        command.flag[0] = i;
    }
    else  if(strcmp(command.tokens[i],"<")==0){
        command.flag[1] = i;
    }
    else  if(strcmp(command.tokens[i],">")==0){
        command.flag[2] = i;
    }
    else  if(strcmp(command.tokens[i],"&&")==0){
        command.flag[3] = i;
    }
    else  if(strcmp(command.tokens[i],"||")==0){
        command.flag[4] = i;
    }
    else  if(strcmp(command.tokens[i],";")==0){
        command.flag[5] = i;
    }
  }

  if(strcmp(command.tokens[0],"exit")==0){
    exit(0);
  }

  // command.size = length;
  // command[k++] = "extra";
  // // command[k++] = 0;

  // printf(1,"I was in tocommandarray\n");
  // printcmd(command);
  // printf(1,"I was in Outsidetocommandarray\n");

  printcmd(command.tokens);
  return 0;
}

int splitcmd(char *command[], char *leftcomm[], char *rightcomm[], char *delim)
{
  int l = 0;
  int r = 0;
  int seen = 1;

  for (int i = 0; i < CMDSIZE; i++)
  {
    // printf(1, "Process PIPE : %s \n ", command[i]);
    char temp[50];
     
    if ((command[i]) == 0)
    {
      break;
    }

    if (strcmp(command[i], delim) == 0)
    {
      // printf(1, "Inside Test for PIPE: %s\n ", command[i]);
      command[i] = 0;
      seen = 0;
    }
    else
    {
      if (seen == 1)
      {
        strcmp(temp, command[i]);
        leftcomm[l++] = temp;
      }
      if (seen == 0)
      {  strcmp(temp, command[i]);
        rightcomm[r++] = temp;
      }
    }
  }

  leftcomm[l]  = 0;
  rightcomm[r] = 0;

  return 0;
}

int runCMD(int *fd, char *cmd[], int side)
{
  if (side == 1)
  {
    //leftside
    close(1);
    dup(fd[1]);
    close(fd[0]);
    close(fd[1]);
    exec(cmd[0], cmd);
    // exit(0);
  }
  else
  {
    close(0);
    dup(fd[0]);
    close(fd[0]);
    close(fd[1]);
    exec(cmd[0], cmd);
    // exit(0);
  }

  // close(fd[0]);
  // close(fd[1]);

  return 0;
}

int prompt()
{
  printf(1, "MyShell>> ");
  return 0;
}


int outputExecute(char *command[], int *flag){
       // printf(1,"Inside OUTPUT \n");

      char * args[10];
     
      int k =0;  

      // printFlag(flag);

      for( int i = flag[0] + 1; i<flag[2]; i++){
        args[k++] = command[i]; 
      }
      args[k] = 0;


    // int filedindex = 0; int k =0;
    //   for(int i = 0; i<CMDSIZE/2;i++){

    //       if(strcmp(command[i], ">") ==0){
    //            filedindex = k;
    //       }
    //       else{
    //         args[k++] = command[i];
    //       }
    //   }
     // args[k] = 0;

   // printcmd(command);

     // printf(1,"Index of the file is : %d", filedindex);

  //   printf(1,"The output file is : %s \n", command[flag[2]+1]);
      
      int ouputdes = open(command[flag[2]+1], O_CREATE | O_RDWR);
      close(1);
      dup(ouputdes);
      close(ouputdes);

      // char *args[flag[2]+1];
      // for(int i =0 ; i<flag[2]; i++){
      //   args[i] =  command[i];
      // }
      // args[flag[2]] = 0;
      exec(args[0], args);

      return 0; 

}


int inputExecute(char *command[], int *flag){

       // printf(1,"Inside INPUT \n");
        int inputdes = open(command[flag[1]+1] , O_RDONLY);
       close(0);
      dup(inputdes);
      close(inputdes);

       char *args[10];
      for(int i =0 ; i<flag[1]; i++){
        args[i] =  command[i];
      }

      args[flag[1]] = 0;

      exec(args[0], args);

      return 0;
}

int atomicexecution(char* command[], int *flag){


    if((flag[1]!=0) && (flag[2]!=0)){
     // printf(1,"Inside both INPUT OUTPUT \n");
      close(0);
      int inputdes = open(command[flag[1]+1] , O_RDONLY);
      dup(inputdes);
      close(inputdes);
      close(1);
      int ouputdes = open(command[flag[2]+1], O_CREATE | O_RDWR);
      dup(ouputdes);
      close(ouputdes);

      char *args[flag[1]+1];
      for(int i =0 ; i<flag[1]; i++){
        args[i] =  command[i];
      }
       args[flag[1]] = 0;
      exec(args[0], args);
    }
    else if (flag[1]!=0){
       // printf(1,"Inside INPUT \n");
      int inputdes = open(command[flag[1]+1] , O_RDONLY);
      close(0);
      dup(inputdes);
      close(inputdes);

       char *args[10];
      for(int i =0 ; i<flag[1]; i++){
        args[i] =  command[i];
      }

      args[flag[1]] = 0;

      exec(args[0], args);
    }
    else if( flag[2]!=0){
    //  printf(1,"Inside OUTPUT \n");

      char * args[10];
     
      int k =0;  

       //printFlag(flag);

      for( int i = flag[0] + 1; i<flag[2]; i++){
        args[k++] = command[i]; 
      }
      args[k] = 0;


    // int filedindex = 0; int k =0;
    //   for(int i = 0; i<CMDSIZE/2;i++){

    //       if(strcmp(command[i], ">") ==0){
    //            filedindex = k;
    //       }
    //       else{
    //         args[k++] = command[i];
    //       }
    //   }
     // args[k] = 0;

   // printcmd(command);

     // printf(1,"Index of the file is : %d", filedindex);

    // printf(1,"The output file is : %s \n", command[flag[2]+1]);
      
      int ouputdes = open(command[flag[2]+1], O_CREATE | O_RDWR);
      close(1);
      dup(ouputdes);
      close(ouputdes);

      // char *args[flag[2]+1];
      // for(int i =0 ; i<flag[2]; i++){
      //   args[i] =  command[i];
      // }
      // args[flag[2]] = 0;
      exec(args[0], args);

    }
    else{
      
         exec(command[0],command);
      
      
    }

  return 0;
}

int pipeExecution(struct line* command, int position){

  //char **t = command->tokens;
  struct line lhs;
  struct line rhs;

  for(int i =0 ; i<=position; i++){
    lhs.tokens[i] = command->tokens[i];
  }

   for(int i =position+1 ; i<=command->size; i++){
    rhs.tokens[i] = command->tokens[i];
  }

  printf(1,"iipppppppppppppppppppppppppppppp");

printcmd(lhs.tokens);
printcmd(rhs.tokens);
  
  
return 0;
  // char *leftcomm[CMDSIZE / 2];
  // char *rightcomm[CMDSIZE / 2];

 
   //printcmd(command);

  // //  ls 
  // //  0
  // //  wc 

  // // cat < README | wc 
  // // cat < README 0 wc

  // // printf(1,"11111111111111111111111111111111\n");

  //  splitcmd(command, leftcomm, rightcomm, command[flag[0]]);
  // // printcmd(leftcomm);
  // // printcmd(rightcomm);

  // for(int i =0 ; i<8; i++){
  //   printf(1,"Command at %d : %s\n", i , leftcomm[i]);
  // }

  //  printf(1,"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n");


  // for(int i =0 ; i<8; i++){
  //   printf(1,"Command at %d : %s\n", i , rightcomm[i]);
  // }

  //  printf(1,"RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR\n");


  //printcmd(command);

 // splitcmd(command, leftcomm, rightcomm, command[flag[0]]);

// int fd[2];
//   pipe(fd);

//   if (fork() == 0)
//   {

//     //printf(1,"Entered Left Section \n");

//     close(1);
//     dup(fd[1]);
//     close(fd[0]);
//     close(fd[1]);

//     int leftflag[6] = {0,0,0,0,0,0};
   
//     for(int i = 0; i<7; i++)
//     {
//        if(flag[i]<=flag[0]){
//          leftflag[i] = flag[i];
//        }
//     }    

//     atomicexecution(command,leftflag);
    
//     //exec(cmd[0], cmd);


//     //runCMD(fd, leftcomm, 1);
//     exit(0);
//   }

//   if (fork() == 0)
//   {
//      //printf(1,"Entered Right Section \n");

     

//     close(0);
//     dup(fd[0]);
//     close(fd[0]);
//     close(fd[1]);
//   //  exec(cmd[0], cmd);

//    // runCMD(fd, rightcomm, 0);


//     int rightflag[6] = {0,0,0,0,0,0};
   
//     for(int i = 0; i<6; i++)
//     {
//        if(flag[i]>=flag[0]){
//          rightflag[i] = flag[i];
//        }
//     } 

//     //printFlag(rightflag);


//     // for(int i = 0 ; i<CMDSIZE/2; i++){
//     //   printf(1,"Right : %d, %s \n", i, rightcomm[i]);
//     // }
//    // printcmd(command);

//     atomicexecution(command,rightflag);

//     exit(0);
//   }

//   close(fd[0]);
//   close(fd[1]);
//   wait(0);
//   wait(0);
//   return 0;
}

int withoutPipeExecution(char* command[], int *flag){

  if(fork()==0){
    
    if((flag[1]!=0) && (flag[2]!=0)){
     // printf(1,"Inside both INPUT OUTPUT \n");
      close(0);
      int inputdes = open(command[flag[1]+1] , O_RDONLY);
      dup(inputdes);
      close(inputdes);
      close(1);
      int ouputdes = open(command[flag[2]+1], O_CREATE | O_RDWR);
      dup(ouputdes);
      close(ouputdes);

      char *args[flag[1]+1];
      for(int i =0 ; i<flag[1]; i++){
        args[i] =  command[i];
      }
       args[flag[1]] = 0;
      exec(args[0], args);
    }
    else if (flag[1]!=0){
       // printf(1,"Inside INPUT \n");
        int inputdes = open(command[flag[1]+1] , O_RDONLY);
       close(0);
      dup(inputdes);
      close(inputdes);

       char *args[flag[1]+1];
      for(int i =0 ; i<flag[1]; i++){
        args[i] =  command[i];
      }
      args[flag[1]] = 0;

      exec(args[0], args);
    }
    else if( flag[2]!=0){
     // printf(1,"Inside OUTPUT \n");
      
      int ouputdes = open(command[flag[2]+1], O_CREATE | O_RDWR);
      close(1);
      dup(ouputdes);
      close(ouputdes);

      char *args[flag[2]+1];
      for(int i =0 ; i<flag[2]; i++){
        args[i] =  command[i];
      }
      args[flag[2]] = 0;
      exec(args[0], args);

    }
    else{
      exec(command[0],command);
    }

  }

  // close(0);
  // close(1);


  wait(0);

  return 0;
}

int parallelExecution(char *command[],int *flag){

  char *leftcomm[CMDSIZE / 2];
  char *rightcomm[CMDSIZE / 2];


 // printcmd(command);

  // printf(1,"11111111111111111111111111111111\n");

  splitcmd(command, leftcomm, rightcomm, command[flag[5]]);
  // printcmd(leftcomm);
  // printcmd(rightcomm);

  // for(int i =0 ; i<5; i++){
  //   printf(1,"Command at %d : %s\n", i , leftcomm[i]);
  // }

 //  printf(1,"LLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n");


  // for(int i =0 ; i<5; i++){
  //   printf(1,"Command at %d : %s\n", i , rightcomm[i]);
  // }

  // printf(1,"RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR\n");


  // printcmd(command);
  return 0;
}



int successiveANDexecution(char *command[], int *flag){
    int status;
      if(fork()==0){
        char *args[flag[3]];
        for(int i = 0; i<flag[3];i++){
          args[i] = command[i];
        }
        args[flag[3]] = 0;
         exec(args[0], args);

      }

      wait(&status);

      if(status !=0){
        return 0;
      }

      if(fork()==0){
        int size = flag[6]-flag[3];
         char *args[size];
          int k = 0; 
          for(int i = flag[3]+1; i<flag[6];i++){
              args[k++] = command[i];
            }
        args[k] = 0;
        
        exec(args[0], args);
      }

      wait(0);
      return 0;
}


int successiveORexecution(char *command[], int *flag){
    
      if(fork()==0){
        char *args[flag[4]];
        for(int i = 0; i<flag[4];i++){
          args[i] = command[i];
        }
        args[flag[4]] = 0;
         exec(args[0], args);

      }
      wait(0);
   
      if(fork()==0){
        int size = flag[6]-flag[4];
         char *args[size];
          int k = 0; 
          for(int i = flag[4]+1; i<flag[6];i++){
              args[k++] = command[i];
            }
        args[k] = 0;
        
        exec(args[0], args);
      }

      wait(0);
      return 0;
}




int executeCommand(char *input, char *command[])
{
  //  flag = {pipe, input, output,   &&,  ||,   ; , length}
  int flag[] = {    0,    0,      0,    0,   0,   0  , 0 };

  // toCommmandArray(input, command, flag);

  //   successive execution 

  printcmd(command);

 

  if(flag[3]!=0){
    successiveANDexecution(command, flag);

  }
  else if(flag[4]!=0){
    successiveORexecution(command, flag);
  }
  else if(flag[5]!=0){
      parallelExecution(command, flag);
  }
  else if(flag[0] != 0){
    pipeExecution(command,flag);
  }
  else{
    withoutPipeExecution(command,flag);
  }

  return 0;
}


int readCommandType(char *command[]){
return 0;
}

int parseCommand(struct line *command){

 

int i;
  for(i = 0; i<command->size;i++){
    if(strcmp(command->tokens[i],"|")==0){
      command->tokens[i] = 0; 
      pipeExecution(command,i);

      }
  }








  // int posittion; 
  // int type;

  // parse(command, &posittion, &type);


    // if(type == 1){
    //   parallelExecution();
    // }
    // else if(type ==2){
    //   pipe(pipeExecution);
    // }
    // else if(type==3){
    //   successiveANDexecution();
    // }
    // else if(type = 4){
    //   successiveORexecution();
    // }
    // else if(type = 5){
    //   inputExecute();
    // }else if(type = 6){
    //   outputExecute();
    // }else{
    //   exec
    // }
      // parallel 

      // pipe

      // &&

      // ||

      // <

      // >

      // normal command




    return 0;
}




int main(void)
{

  //int status;
  char input[READSIZE];
  // char *string[CMDSIZE];
  // struct line *command = malloc(s);
   
  // command.tokens = string;

  while (1)
  {

   
    prompt();
    takeinput(input);

    // struct line cmd; 
  
    struct line *command = readLine(input);
    
    // printcmd(command->tokens);

    parseCommand(command);

    // printinput(command->inputchars);


    clearAll(input,command->tokens);

  }

  exit(0);

  return 0;
}

//	parser(comm);

// memset(input, 0, 100);

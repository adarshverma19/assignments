#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  int status;

 if( argint(0, &status) < 0 ){
   return -1;
 }

  exit(status);
  return 0;  // not reached
}

int
sys_wait(void)
{
 int *p;
 if(argptr(0, (void*)&p, sizeof(int))<0){

   return -1;

 }
  return wait(p);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Deliverable 1
//Prints Hello World on the console
int 
sys_helloWorld(void)
{
	cprintf("Hello World\n");
	return 0; 	
}

// Deliverable 2
//returns the number of open files(nof) by the process
int 
sys_numOpenFiles(void)
{  
   int processid;

 if( argint(0, &processid) < 0 ){
   return -1;
 }
  numOpenFiles(processid);
	return 0;
}

// Deliverable 3
//returns the total heap memory allocated to the process till now
int 
sys_memAlloc(void){
  int processid;

 if( argint(0, &processid) < 0 ){
   return -1;
 }

   memAlloc(processid);

	return 0; //myproc()->heapSize;
}

//Deliverable 4 
//Prints the Process creation, last contextswitchOut, last contextSwitchIn Time
int 
sys_getprocesstimedetails(void)
{

  int processid;

 if( argint(0, &processid) < 0 ){
   return -1;
 }

   getprocesstimedetails(processid);

	return 0;
}

int
sys_psinfo(void)
{
  return psinfo();
}

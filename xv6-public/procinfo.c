#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
  //   int i;

  if (argc < 2)
  {
    printf(1, "Error : No Process ID Provided \n");
    exit(0);
  }

  int processid = atoi(argv[1]);

 
  numOpenFiles(processid);
  memAlloc(processid);
  getprocesstimedetails(processid);

  exit(0);
  return 0;
}

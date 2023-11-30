#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
  if(argc <= 1){
    fprintf(2, "sleep: No args passed!\n");
    exit(1);
  }

  int tics = atoi(argv[1]);

  printf("sleeping for %d tics\n", tics);
  if(sleep(tics)){
    fprintf(2, "Error while running sys_sleep");
    exit(1);
  }
  exit(0);
}

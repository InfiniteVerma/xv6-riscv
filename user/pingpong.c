#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
  printf("pingpong called\n");

  int p[2];
  pipe(p);

  int pid = fork();

  if(pid < 0) {
    fprintf(2, "fork failed\n");
    exit(1);
  }
  
  if(pid == 0) {
    // child
    uint64 pid = getpid();

    char buffer[4];
    read(p[0], buffer, sizeof(buffer));
    printf("Child pid: %d received: %s\n", pid, buffer);

    strcpy(buffer, "pong");
    write(p[1], buffer, sizeof(buffer));
    close(p[0]);
    close(p[1]);

    exit(0);
  } else {
    // parent
    uint64 pid = getpid();

    char buffer[4];
    strcpy(buffer, "ping");
    write(p[1], buffer, sizeof(buffer));
    read(p[0], buffer, sizeof(buffer));
    printf("Child pid: %d received: %s\n", pid, buffer);
    close(p[1]);
    close(p[0]);

    // wait for child to complete
    wait(0);
    exit(0);
  }
}

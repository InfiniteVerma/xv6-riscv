#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}

// TODO need to recursively search through each directory and try to match file name
void
find(char *fileName)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  // open current directory
  if((fd = open(".", 0)) < 0){
    fprintf(2, "find: cannot search through %s\n", ".");
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", ".");
    close(fd);
    return;
  }
  
  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    fprintf(2, "THIS SHOULD NOT HAPPEN");
    break;

  // TODO run find again with each subdirectory
  case T_DIR:
    if(strlen(".") + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, ".");
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if(strcmp(fmtname(buf), fileName) == 0) {
          printf("FOUND: %s", buf);
      } else {
        int ans = strcmp(fmtname(buf), fileName);
        printf("strcmp value: %s - %s - %d\n", fmtname(buf), fileName, ans);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "find: no args passed\n");
    exit(0);
  }

  find(argv[1]);
  exit(0);
}

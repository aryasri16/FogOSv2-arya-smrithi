#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("In the find.c file\n");
  find("/home/user","document.pdf");
  return 0;
  
}

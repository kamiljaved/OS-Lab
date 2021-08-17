#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  int a = *((int*) 0);
  printf(1, "%d\n", a);
  //exit();
}

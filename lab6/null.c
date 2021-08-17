#include "stdio.h"

int
main(int argc, char *argv[])
{
  int a = *((int*) 0);
  printf("%d\n", a);
}

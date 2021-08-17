#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char ** argv)
{
  printf(1, "(in program: %s)\n", argv[0]);

  char str[255];
  char* ptr = NULL;
  
	if (argc>1) 
	{
    ptr = argv[1];
  }
  
  strcpy(str, ptr);  
	printf(1, "%s\n", str);
  printf(1, "%x\n", str);

  exit();
}

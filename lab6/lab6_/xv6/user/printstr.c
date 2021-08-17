#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) 
{
	char* str = "";
	strcpy(str, argv[1]);
	
	char* strr = printstr(str);
	
	printf(1, "%s", strr);

	exit();
}

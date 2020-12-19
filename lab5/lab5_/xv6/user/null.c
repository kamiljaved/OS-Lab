#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
    printf(1, "(in program: %s)\n", argv[0]);

    int a = *((int*) 0);
    printf(1, "%d\n", a);
    printf(1, "%x\n", a);
    
    exit();
}
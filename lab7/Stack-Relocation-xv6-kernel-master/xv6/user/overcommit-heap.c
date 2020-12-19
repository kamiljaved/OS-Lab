#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
int count=9000;

void growHeap(){
	int *a = (int *)malloc(655360-(5*4096));
	 	if(a==NULL)
			 printf(1,"allocation failed\n");
		*a = 10;
	 	printf(1,"a = %d\n",a);
}

void growStack(){
	int a[1256];
	int i;
	for(i=0;i<1256;i++)
		a[i]=i;
	printf(1,"address of last stack element =%d\n",&a[0]);
}


void heapFail(){
	int *b = (int *)malloc(4096);
	 	printf(1,"b=%d",*b);
	  	exit();
}


int
main(int argc, char *argv[])
{
	growHeap();
	growStack();
	//heapFail();
	exit();

}

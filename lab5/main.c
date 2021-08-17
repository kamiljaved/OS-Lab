#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main (int argc,char **argv) {
	
	int r;
      
  	printf("Enter size for heap memory initialization = ");
 	scanf("%d",&r);	
        Mem_Init(r);
	Mem_Dump();
	//to colaesce and free memory
        Mem_Free(NULL);
 
	int *x=Mem_Alloc(10,FIRSTFIT);    	// FF	
	Mem_Dump();
	
	int *y=Mem_Alloc(30,WORSTFIT);		// WF
	Mem_Dump();
	
        int *z=Mem_Alloc(500,FIRSTFIT);    	// FF	
	Mem_Dump();

        int *a=Mem_Alloc(1000,WORSTFIT);	// WF
	Mem_Dump();

	Mem_Free(y);
	Mem_Dump();

        Mem_Free(z);
	Mem_Dump();

        z=Mem_Alloc(500,FIRSTFIT);    	// FF	
	Mem_Dump();

        a=Mem_Alloc(10,WORSTFIT);	// WF
	Mem_Dump();
	
  	return 0;
}

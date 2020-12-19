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
 
	int *a=Mem_Alloc(10,FIRSTFIT);    	// FF	
	Mem_Dump();
	
	int *b=Mem_Alloc(30,WORSTFIT);		// WF
	Mem_Dump();
	
        int *c=Mem_Alloc(500,FIRSTFIT);    	// FF	
	Mem_Dump();

        int *d=Mem_Alloc(1000,WORSTFIT);	// WF
	Mem_Dump();

	Mem_Free(b);
	Mem_Dump();

        Mem_Free(c);
	Mem_Dump();

        int* e=Mem_Alloc(500,FIRSTFIT);    	// FF	
	Mem_Dump();

        int* f=Mem_Alloc(10,WORSTFIT);	// WF
	Mem_Dump();

	Mem_Free(d);
	Mem_Dump();

	// new cases
	int* g=Mem_Alloc(1000,FIRSTFIT);	// FF
	Mem_Dump();

	int* h=Mem_Alloc(1000,WORSTFIT);	// WF
	Mem_Dump();

	int* i=Mem_Alloc(450,BESTFIT);	// BF
	Mem_Dump();

	int* j=Mem_Alloc(10, BESTFIT); //BF
	Mem_Dump();

	int* k=Mem_Alloc(1000, BESTFIT); //BF
	Mem_Dump();
	
  	return 0;
}

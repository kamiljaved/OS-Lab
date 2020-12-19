#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main (int argc,char **argv) {
	
	int r;
      
  	printf("Enter size for heap memory initialization (Bytes) = ");
 	scanf("%d",&r);	
        
	Mem_Init(r);
	
	Mem_Dump();
	
	//to colaesce and free memory
    Mem_Free(NULL);

	int *a=Mem_Alloc(10,FIRSTFIT);    		// FF	
	Mem_Dump();
	
	int *b=Mem_Alloc(30,WORSTFIT);			// WF
	Mem_Dump();

    int *c=Mem_Alloc(500,FIRSTFIT);    		// FF	
	Mem_Dump();

    int *d=Mem_Alloc(1000,WORSTFIT);		// WF
	Mem_Dump();

	Mem_Free(b);
	Mem_Dump();

    Mem_Free(c);
	Mem_Dump();

    int* e=Mem_Alloc(500,FIRSTFIT);    		// FF	
	Mem_Dump();

    int* f=Mem_Alloc(10,WORSTFIT);			// WF
	Mem_Dump();

	Mem_Free(d);
	Mem_Dump();

	// new cases
	int* g=Mem_Alloc(1000,FIRSTFIT);		// FF
	Mem_Dump();

	int* h=Mem_Alloc(800,WORSTFIT);			// WF
	Mem_Dump();

	int* i=Mem_Alloc(450,BESTFIT);			// BF
	Mem_Dump();

	int* j=Mem_Alloc(200, BESTFIT); 		// BF
	Mem_Dump();

	int* k=Mem_Alloc(1000, BESTFIT); 		// BF
	Mem_Dump();
	

	if (a != NULL) Mem_Free(a);
	if (e != NULL) Mem_Free(e);
	if (f != NULL) Mem_Free(f);
	if (g != NULL) Mem_Free(g);
	if (h != NULL) Mem_Free(h);
	if (i != NULL) Mem_Free(i);
	if (j != NULL) Mem_Free(j);
	if (k != NULL) Mem_Free(k+1);
	if (k != NULL) Mem_Free(k);
	Mem_Dump();
	CoalesceAll();
	Mem_Dump();

  	return 0;
}

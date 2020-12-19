#include "types.h"
#include "stat.h"
#include "user.h"

#define KB 1024

int
main(int argc, char *argv[])
{  

   printf(1,"\nProg Entry Point = 0x%p\n",main);

   printf(1,"\nStack High-End   = 0x%p\n",(&argv[argc-1]+argc* 4) );

   printf(1,"\nInitial Address Space.\n");

   vaSpace();         			// SysCall to dump Virtual Address Space

   int *x=malloc(sizeof(int));		// Extend Heap_Region , Dummy 

   int n=10000;
   x=malloc(n*sizeof(int));		// Extend Heap_Region , Dummy 

   printf(1,"\nHeap_Region = %dKB\n", (n*4)/KB);

   printf(1,"\nAddress Space after Heap_Extension\n");

   vaSpace();         			// Updated VA_Space

   int *p=(int*)(635*1024);     	// p=635KB , page just above stack
   int *d=(int*)(631*1024); 
   int *i=(int*)(627*1024); 
   printf(1,"\nCausing Page_Fault on page just above Stack.\n");

   printf(1,"\nAfter Page Fault just above Stack =0x%p \n",*p);
   vaSpace();
				
   printf(1,"\nAfter Page Fault just above Stack =0x%p \n",*d);
   vaSpace();

   printf(1,"\nAfter Page Fault just above Stack =0x%p \n",*i);
   vaSpace();

   exit();

}


// *********************************************************************************
// 	File Name:	mem.c
// 	Created on: April 7, 2019
//	Author: 	kamiljaved
//	Features:	> Provides definitions for the memory-managing functions declared
//                in "mem.h"
//              > Defines Functions for Basic Memory Management
//              > Allows Memory Initialization, Allocation and Freeing
//              > Supports Automatic Coalescing of Freed Memory Chunks
//              > Provides Function for printing Complete Memory Status
// *********************************************************************************


// includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "mem.h"

// global data

int m_error;

typedef struct __list_t
{
    int size;
    bool free;
    struct __list_t *next;
}
*MemNodePtr;

MemNodePtr memHead = NULL;
bool memInitialized = false;
int pageSize = 0;

// function prototypes
MemNodePtr Previous(MemNodePtr ptr);
void Coalesce(MemNodePtr freed);


// Memory Initialization Function
int Mem_Init(int sizeOfRegion)
{   
    if (sizeOfRegion <= 0 || memInitialized)
    {
        m_error = E_BAD_ARGS;
        return -1;
    }   

    // Minimum Allowed Memory = Page-Size
    // Memory Size is Page-Size Aligned 
    pageSize = getpagesize();
    puts("");
    printf("Initialization Request = %d.\n", sizeOfRegion);
    printf("Page Size = %d.\n", pageSize);
    int sizeCheck = sizeOfRegion % pageSize;
    if(sizeCheck != 0)
    {
        sizeOfRegion += pageSize - sizeCheck;
    }

    int fd = open("/dev/zero", O_RDWR);

    memHead = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (memHead == MAP_FAILED)
    {
        m_error = E_BAD_ARGS;
        return -1;
    }
    memHead->size = sizeOfRegion - sizeof(struct __list_t);
    memHead->next = NULL;
    memHead->free = true;

    close(fd);

    memInitialized = true;
    printf("Successfully Initialized %d Bytes of Memory (8-Byte Aligned).\n", sizeOfRegion);

    return 0;
}


// Memory Allocation Function
void *Mem_Alloc(int size, int style)
{
    printf("Allocation Request = %d (%s).\n", size, style?"FF":"WF");
    size += sizeof(struct __list_t);

    if (size <= 0)    
    {
        m_error = E_BAD_ARGS;
        return NULL;
    }

    // Minimum Allowed Allocation = 8-bytes
    // Memory is 8-byte Aligned
    int sizeCheck = size % 8;
    if (sizeCheck != 0)     
    {
        size += 8 - sizeCheck;
    }

    int objSz = size - sizeof(struct __list_t);

    MemNodePtr tmp = memHead;
    if(style == FIRSTFIT)
    {
        // Allocate requested memory at end of first free chunk
        while(tmp!=NULL)
        {
            if (tmp->free && tmp->size >= size)
            {
                break;
            }
            tmp = tmp->next;
        } 
    }
    else if(style == WORSTFIT)
    {
        // Allocate requested memory at end of biggest free chunk
        MemNodePtr largestFree = NULL;
        while(tmp!=NULL)
        {
            if (tmp->free && tmp->size >= size)
            {
                if (largestFree == NULL || tmp->size > largestFree->size)
                    largestFree = tmp;
            }
            tmp = tmp->next;
        } 
        tmp = largestFree;
    }
    else
    {
        m_error = E_BAD_ARGS;
        return NULL;
    }
    
    if (tmp == NULL)
    {
        m_error = E_NO_SPACE;
        return NULL;
    }
    else
    {
        MemNodePtr pNew = (void*)tmp + tmp->size + sizeof(struct __list_t) - size; 
        pNew->size = size - sizeof(struct __list_t);
        pNew->free = false;
        pNew->next = tmp->next;
        tmp->size -= size;
        tmp->next = pNew;

        printf("Successfully Allocated %d (%d+%d) Bytes at %lu.\n", size, objSz, (int)sizeof(struct __list_t),(unsigned long)pNew);
        return (void*)pNew + sizeof(struct __list_t);
    }
}


// Function to Free Specified Chunk of Memory
int Mem_Free(void *ptr)
{
    if (ptr == NULL) return 0;

    MemNodePtr tmp = memHead->next, freeMe = NULL;
    void* headAddr = ptr - sizeof(struct __list_t);

    while(tmp!=NULL)
    {
        if (tmp == headAddr)
        {
            if (tmp->free) break;
            
            freeMe = tmp;
            freeMe->free = true;

            printf("Successfully Freed %lu.\n", (unsigned long)freeMe);

            // Carry out any necessary Coalescence
            MemNodePtr prv = Previous(freeMe);
            if (prv->free)  Coalesce(prv);
			else            Coalesce(freeMe);

			break;
        }
        tmp = tmp->next;
    }

    return (freeMe == NULL) ? -1 : -0;
}

// Function to get the previous MemNodePtr of a given MemNodePtr
MemNodePtr Previous(MemNodePtr ptr)
{
	MemNodePtr prv = memHead;
    if (prv == NULL || ptr == NULL || prv == ptr) return NULL;

	while(prv->next!=NULL)
    {
        if (prv->next == ptr)
        {
            return prv;
        }
        prv = prv->next;
    }

    return NULL;
}

// Function to combine free chunk of memory with all next free chunks 
void Coalesce(MemNodePtr freed)
{
    MemNodePtr tmp = freed->next, end = freed->next;
	unsigned long size = freed->size;

	while (tmp != NULL && tmp->free)
	{
		end = tmp->next;
		size += tmp->size + sizeof(struct __list_t);
		tmp = tmp->next;
	}

    if (size != freed->size) printf("Successfully Coalesced %lu.\n", (unsigned long)freed);
	freed->next = end;
	freed->size = size;
}

// Function to Print Complete Status of Initialized User Memory
void Mem_Dump()
{
    if (memHead == NULL) return;
    
	MemNodePtr tmp = memHead;
	int nodeNum = 1;
	unsigned long totalFree = 0;
	unsigned long totalOccupied = 0;
    unsigned long size = 0;

    puts("");
	printf("———————————————————————————————————————————————————————————————————\n");
	printf( " %-4s %-20s %-12s %-12s %-8s\n",
            "#", "Address", "Tot. Size", "Obj. Size", "Status");
	printf("———————————————————————————————————————————————————————————————————\n");
	
    while (tmp != NULL)
	{
		size = tmp->size + sizeof(struct __list_t);
		printf( " %-4d %-20lu %-12lu %-12d %-8s\n",
                nodeNum, (unsigned long)tmp, size, tmp->size, tmp->free?"Free":"Occupied");

		if (tmp->free)
		{
			totalFree += tmp->size;
			totalOccupied += sizeof(struct __list_t);
		}
		else
		{
			totalOccupied += size;
		}

		tmp = tmp->next;
		nodeNum++;
	}

	printf("———————————————————————————————————————————————————————————————————\n");
	printf(" Total Occupied Space  =  %lu\n", totalOccupied);
	printf(" Total Free Space      =  %lu\n", totalFree);
	printf("———————————————————————————————————————————————————————————————————\n");
	puts("");
}

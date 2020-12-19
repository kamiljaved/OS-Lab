// *********************************************************************************
// 	Last Modified:  November 6, 2020
// *********************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "mem.h"

#define	MAGIC_NO (0123210)

// global data

int m_error;

typedef struct __list_t
{
    int size;
    bool type;
    struct __list_t *next;
}
*MemNodePtr;

long unsigned int nodeSize = sizeof(struct __list_t); 		// 16 Bytes
long unsigned int occupiedSize = 0;
int numAllocs = 0;

MemNodePtr memHead = NULL;
bool memInitialized = false;
int pageSize = 0;

// (local) function prototypes
MemNodePtr Previous(MemNodePtr ptr);
void CoalesceForw(MemNodePtr freed);
MemNodePtr PrevAny(MemNodePtr ptr);
void CoalesceAll();

// Memory Initialization Function
int Mem_Init(int sizeOfRegion)
{
    if (sizeOfRegion <= 0 || memInitialized)
    {
        m_error = E_BAD_ARGS;
		printf("Bad Arguments.\n");
        return -1;
    }  

	// getpagesize() returns the number of bytes in a memory page
	pageSize = getpagesize();
    
	// user-request
	puts("");
    printf("Page Size = %d Bytes.\n", pageSize);
    printf("Initialization Request = %d %s.\n", sizeOfRegion, sizeOfRegion==1?"Byte":"Bytes");

	sizeOfRegion += nodeSize;		// add extra bytes for head-node (??)
	// Minimum Allowed Region = Page-Size
    // Region Size has to be Page-Size Aligned 
	int sizeCheck = sizeOfRegion % pageSize;
	if(sizeCheck != 0)
    {
        sizeOfRegion += pageSize - sizeCheck;
    }

	// void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
	// mmap() creates a new mapping in the virtual address space of the calling process.
	// 		On success, mmap() returns a pointer to the mapped area.  
	// 		On error, the value MAP_FAILED is returned.  
	// The starting address for the new mapping is specified in addr.  
	// If addr is NULL, then the kernel chooses the (page-aligned) address at which to create the mapping. 
	// The length argument specifies the length of the mapping (which must be greater than 0).
	// prot argument describes the desired memory protection of the mapping.
    //   	PROT_READ:		Pages may be read.
    //   	PROT_WRITE:		Pages may be written.
	// flags argument determines whether updates to the mapping are visible to other 
	// processes mapping the same region.
    //	 	MAP_PRIVATE:	Create a private copy-on-write mapping. Updates to the mapping 
	//						are not visible to other processes mapping the same
    //          			file, and are not carried through to the underlying file.
	// The contents of a file mapping are initialized using length bytes starting at offset 
	// "off_t offset" in the file (or other object) referred to by the file descriptor fd.
	// Mapping /dev/zero gives the calling program a block of zero-filled virtual memory 
	// of the size specified in the call to mmap.
	int fd; 
	if ((fd = open("/dev/zero", O_RDWR)) < 0) {
		return -1;
	}
    memHead = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (memHead == MAP_FAILED)
    {
        m_error = E_BAD_ARGS;
		printf("MMAP Failed.\n");
        return -1;
    }
	memHead->size = sizeOfRegion - nodeSize;
	memHead->next = NULL;

	close(fd);

	memInitialized = true;
	printf("Successfully Initialized %d (%d+%d) Bytes of Usable Memory.\n", memHead->size+(int)nodeSize, memHead->size, (int)nodeSize);
	// but amount that could actually be allocated is dependent upon the number 
	// of allocations (as free-list nodes also take up part of this memory)

	return 0;
}


// Memory Allocation Function
void *Mem_Alloc(int size, int style)
{
	char* strStyle = NULL;		// default allocation style
	if (style == WORSTFIT)		strStyle = "WF";
	else if (style == BESTFIT)	strStyle = "BF";
	else if (style == FIRSTFIT)	strStyle = "FF";
	else 
	{
		style = BESTFIT;			// default allocation style
		char* strStyle = "BF";
	}

	if (size <= 0)
	{
		m_error = E_BAD_ARGS;
		printf("Bad Arguments.\n");
		return NULL;		
	}

	printf("Allocation Request = %d (%s).\n", size, strStyle);

    // Minimum Allowed Allocation = 8-bytes
    // Allocated chunk (request + node) should be 8-byte Aligned
	int sizeCheck = size % 8;
	if (sizeCheck != 0)
	{
		size += 8 - sizeCheck;
	}

	MemNodePtr tmp = memHead;
	if (style == FIRSTFIT)
	{
		// Allocate requested memory at end of first free chunk
		while (tmp != NULL)
		{
			if (tmp->size >= size) break;
			tmp = tmp->next;
		}
	}
	else if (style == WORSTFIT)
	{
		// Allocate rquested memory at end of biggest free chunk
		MemNodePtr largest = NULL;
		while (tmp != NULL)
		{
			if (tmp->size >= size && (largest == NULL || tmp->size > largest->size)) largest = tmp;
			tmp = tmp->next;
		}
		tmp = largest;
			
	}
	else if (style == BESTFIT)
	{
        // Allocate requested memory at end of nearest larger value free chunk
        // We'll chose to stick with the first best candidate (in case there are more than one)
		MemNodePtr best = NULL;

		while (tmp != NULL)
		{
			if (tmp->size >= size && (best == NULL || tmp->size < best->size)) best = tmp;
			tmp = tmp->next;
		}
		tmp = best;
	}
    else
    {
        m_error = E_BAD_ARGS;
        printf("Bad Arguments.\n");
        return NULL;
    }

	if (tmp == NULL)
	{
		m_error = E_NO_SPACE;
        printf("Not Enough Space for This Allocation.\n");
        return NULL;
	}
	else
	{
		// at end-side of selected block
		MemNodePtr pNew = (void*)tmp + nodeSize + tmp->size - size - nodeSize;
		if (pNew == tmp)
		{
			// alloc this node+chunk
			if (pNew == memHead) memHead = pNew->next;
			else Previous(tmp)->next = tmp->next;	
		}
		else
		{
			// splitting
			tmp->size -= (size + nodeSize);
			pNew->size = size;
		}
		
		occupiedSize += pNew->size;
		numAllocs++;
		pNew->next = (MemNodePtr) MAGIC_NO;
		printf("Successfully Allocated %d (%d+%d) Bytes at %lu.\n", size+(int)nodeSize, size, (int)nodeSize, (unsigned long)pNew);
        return (void*)pNew + nodeSize;
	}
	
	return NULL;
}


// Function to Free Specified Chunk of Memory
int Mem_Free(void *ptr)
{
	if (ptr == NULL) return 0;

	MemNodePtr freeMe = (void*) ptr - nodeSize;
	printf("Node to Free = %lu (%d).\n", (unsigned long)freeMe, freeMe->size);


	// test if node memory untampered
	if (freeMe->next != (MemNodePtr) MAGIC_NO)
	{
		m_error = E_CORRUPT_FREESPACE;
		printf("Corrupt Freespace (or Bad Address).\n");
		return -1;
	}

	if (memHead == NULL)
	{
		// list empty, treat freed-node as head node 
		memHead = freeMe;
		memHead->next = NULL;
		occupiedSize -= freeMe->size;
		numAllocs--;
	}
	else if (freeMe < memHead)
	{
		// attach to list start, make head-node
		freeMe->next = memHead;
		memHead = freeMe;
		occupiedSize -= freeMe->size;
		numAllocs--;
		CoalesceForw(memHead);
	}
	else 
	{
		// node either in middle or at end
		MemNodePtr prev = PrevAny(freeMe);
		if (!prev)
		{
			m_error = E_BAD_ARGS;
			printf("Bad Arguments.\n");
			return -1;
		}
		else
		{
			if (prev->next)	freeMe->next = prev->next;	// freeMe should be in middle of prev and prev->next
			else 			freeMe->next = NULL;		// freeMe is to be put at end of free-list
			prev->next = freeMe;
			occupiedSize -= freeMe->size;
			numAllocs--;
			CoalesceForw(freeMe);
		}
	}
	
	return 0;
}

// Function to get the previous MemNodePtr of a given (free) MemNodePtr
MemNodePtr Previous(MemNodePtr ptr)
{
	MemNodePtr prv = memHead;
    if (prv == NULL || ptr == NULL || prv == ptr) return NULL;

	while(prv->next != NULL)
    {
        if (prv->next == ptr) return prv;
        prv = prv->next;
    }

	return NULL;
}

// Function to get the previous MemNodePtr of any MemNodePtr
// ptr should be in middle of prev and prev->next
MemNodePtr PrevAny(MemNodePtr ptr)
{
	MemNodePtr prv = memHead;
    if (prv == NULL || ptr == NULL || prv == ptr) return NULL;

	while(prv != NULL && prv < ptr)
    {
		if (prv->next > ptr || !prv->next) return prv;
        else prv = prv->next;
    }

	return NULL;
}

// Function to combine free chunk of memory with all next free chunks 
void CoalesceForw(MemNodePtr freed)
{
	if (!freed) return;

	// combine all consecutively next free-nodes with freed node
	while(freed->next != NULL && freed->next == ((void*) freed + nodeSize + freed->size))
	{
		freed->size += nodeSize + freed->next->size;
		freed->next = freed->next->next;
	}
}

void CoalesceAll()
{
	CoalesceForw(memHead);
}

// Function to Print Complete Status of Initialized User Memory
void Mem_Dump()
{
    if (memHead == NULL) return;
    
	MemNodePtr tmp = memHead;
	int nodeNum = 1;
	unsigned long totalFree = 0;
    unsigned long size = 0;

    puts("");
	printf("———————————————————————————————————————————————————————————————————\n");
	printf( " %-4s %-20s %-12s %-12s\n",
            "#", "Address", "Tot. Size", "Obj. Size");
	printf("———————————————————————————————————————————————————————————————————\n");
	
    while (tmp != NULL)
	{
		size = tmp->size + nodeSize;
		printf( " %-4d %-20lu %-12lu %-12d\n",
                nodeNum, (unsigned long)tmp, size, tmp->size);


		totalFree += tmp->size;

		tmp = tmp->next;
		nodeNum++;
	}

	printf("———————————————————————————————————————————————————————————————————\n");
	printf(" Total Occupied Space  =  %-8lu (%lu+%d)\n", occupiedSize+(numAllocs*nodeSize), occupiedSize, numAllocs*(int)nodeSize);
	printf(" Total Free Space      =  %-8lu (%lu+%d)\n", totalFree+(nodeNum-1)*nodeSize, totalFree, (nodeNum-1)*(int)nodeSize);
	printf("———————————————————————————————————————————————————————————————————\n");
	puts("");
}

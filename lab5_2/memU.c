#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mem.h"


typedef struct __list_t {

	int size;
	_Bool isfree;
	struct __list_t *next;

}*MemNodePtr;


int m_error;

// Head to the memory
MemNodePtr memHead = NULL;

int Mem_Init(int sizeOfRegion)
{
	static bool firstTime = true;

	// Check if func is called first time and
	// sizeOfRegion is valid
	if (!firstTime || sizeOfRegion <= 0)
	{
		// Report error
		m_error = E_BAD_ARGS;
		return -1;
	}

	firstTime = false;

	printf("Executing Mem_Init Func\n\n");

	// Page alligning the size given by user
	int pagesize = getpagesize();

	if (sizeOfRegion % pagesize != 0)
	{
		sizeOfRegion = pagesize * ((sizeOfRegion / pagesize) + 1);
	}

	printf("Allocating size: %d\n", sizeOfRegion);

	// Allocating the memory
	int fd = open("/dev/zero", O_RDWR);

	memHead = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

	if (memHead == MAP_FAILED)
	{
		m_error = E_BAD_ARGS;
		return -1;
	}

	puts("After mmap");

	// Initializing the header variable
	memHead->size = sizeOfRegion - sizeof(struct __list_t);
	memHead->isfree = true;
	memHead->next = NULL;

	close(fd);

	return 0;
}

MemNodePtr findFirstFit(MemNodePtr head, int allocsize)
{
	MemNodePtr nd = memHead;
	while (nd != NULL)
	{
		if (nd->isfree && nd->size >= allocsize)
		{
			return nd;
		}
		nd = nd->next;
	}

	return NULL;
}

MemNodePtr findWorstFit(MemNodePtr head, int allocsize)
{
	MemNodePtr nd = NULL, tmp;
	int biggestsize = 0;
	tmp = head;
	while (tmp != NULL)
	{
		if (tmp->isfree && tmp->size >= allocsize && tmp->size > biggestsize)
		{
			nd = tmp;
			biggestsize = nd->size;
		}

		tmp = tmp->next;
	}

	return nd;
}

void *Mem_Alloc(int size, int style)
{

	int actsize = size + sizeof(struct __list_t);
	MemNodePtr nd = NULL;


	if (size <= 0)
	{
		m_error = E_BAD_ARGS;
		return NULL;
	}

	// Allign size to 8 bytes
	if (actsize % 8 != 0)
	{
		actsize = 8 * (actsize / 8 + 1);
	}

	// Find suitable memory block
	if (style == FIRSTFIT)
	{
		nd = findFirstFit(memHead, actsize);
	}
	else if (style == WORSTFIT)
	{
		nd = findWorstFit(memHead, actsize);
	}
	else
	{
		m_error = E_BAD_ARGS;
		return NULL;
	}

	// Make another block of memory
	if (nd != NULL)
	{
		MemNodePtr new = (void*)nd + (nd->size + sizeof(struct __list_t) - actsize);
		new->size = actsize - sizeof(struct __list_t);
		new->isfree = false;
		new->next = nd->next;
		nd->size -= actsize;
		nd->next = new;

		return ((void*)new + sizeof(struct __list_t));
	}
	else
	{
		m_error = E_NO_SPACE;
		return NULL;
	}
}

MemNodePtr previous(MemNodePtr head, MemNodePtr curr)
{
	MemNodePtr prev = NULL;

	MemNodePtr tmp = head;
	if (tmp != NULL)
	{
		while (tmp->next != NULL)
		{
			if (tmp->next == curr)
			{
				prev = tmp;
				break;
			}

			tmp = tmp->next;
		}
	}

	return prev;
}

void colaesce(MemNodePtr nd)
{
	MemNodePtr colaesceTill = nd->next, tmp;
	unsigned long colaesceSize = nd->size;
	tmp = nd->next;
	while (tmp != NULL && tmp->isfree)
	{
		colaesceTill = tmp->next;
		colaesceSize += tmp->size + sizeof(struct __list_t);
		tmp = tmp->next;
	}
	nd->next = colaesceTill;
	nd->size = colaesceSize;
}

int Mem_Free(void *ptr)
{
	void * headaddr;
	MemNodePtr tobefreed = NULL, tmp;

	if (ptr == NULL) return 0;

	headaddr = ptr - sizeof(struct __list_t);
	printf("Inside Free %lu\n", (unsigned long)headaddr);

	tmp = memHead->next;

	while (tmp != NULL)
	{
		if (tmp == headaddr)
		{
			if (tmp->isfree) break;
			tobefreed = tmp;
			tobefreed->isfree = true;

			MemNodePtr prev = previous(memHead, tobefreed);
			if (prev->isfree)
				colaesce(prev);
			else
				colaesce(tobefreed);

			break;
		}
		tmp = tmp->next;
	}

	return (tobefreed != NULL) ? 0 : -1;
}

void Mem_Dump()
{
	MemNodePtr tmp = memHead;
	int count = 1;
	unsigned long tfree = 0;
	unsigned long toccup = 0;

	printf("|----------------------------------------------------------------\n");
	printf("|%-4s: %-20s | %-10s | %-10s | %-8s|\n", "id", "Address",
	       "T Size", "Obj Size", "Free");
	printf("|----------------------------------------------------------------\n");
	while (tmp != NULL)
	{
		unsigned long tsize = tmp->size + sizeof(struct __list_t);
		printf("|%-4d: %-20lu | %-10lu | %-10d | %-8d|\n", count, (unsigned long)tmp,tsize,
		       tmp->size, tmp->isfree);

		if (tmp->isfree)
		{
			tfree += tmp->size;
			toccup += sizeof(struct __list_t);
		}
		else
		{
			toccup += tsize;
		}

		tmp = tmp->next;
		count++;
	}
	printf("|----------------------------------------------------------------\n");
	printf("Total occupied space = %lu\n", toccup);
	printf("Total free space = %lu\n", tfree);
	printf("|----------------------------------------------------------------\n");
	puts("");
}

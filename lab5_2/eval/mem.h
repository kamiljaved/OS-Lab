#ifndef __mem_h__
#define __mem_h__

//different supported errors
#define E_NO_SPACE              (1)
#define E_CORRUPT_FREESPACE     (2)
#define E_PADDING_OVERWRTTTEN   (3)
#define E_BAD_ARGS              (4)
#define E_BAD_POINTER           (5)

//used for errors
extern int m_error;

//style for free space search

#define WORSTFIT    (0)
#define FIRSTFIT    (1)
#define BESTFIT     (2)

//routines
int Mem_Init(int sizeOfRegion);
void *Mem_Alloc(int size, int style);
int Mem_Free(void *ptr);
void Mem_Dump();

#endif  //__mem_h__

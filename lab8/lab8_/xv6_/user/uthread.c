#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

#define NULL 0
#define NPROC 64
#define PGSIZE 4096

struct thread_stack {
    void* p;            // ptr to malloced mem for stack
    void* stack;        // ptr to stack (PGSZIE aligned)
};

struct thread_stack thread_stacks[NPROC];       // tracks all threads' stack (of this proc)

int uthread_init = 0;
lock_t thread_creation_lock;    // provides mutual exclusion when creating proc's threads
int use_thread_lock = 0;        // not used by default

int set_use_thread_lock(int set)
{
    if (set == 1) use_thread_lock = 1;
    else use_thread_lock = 0;
}

// creates new thread for lib-using proc
// allocs new stack mem and calls syscall clone() 
int thread_create(void (*start_routine)(void*), void* arg) 
{
    if (!uthread_init) {
        lock_init(&thread_creation_lock);
        for(int i = 0; i < NPROC; i++) thread_stacks[i].stack = thread_stacks[i].p = NULL;
        uthread_init = 1;
    }

    int local_use_thread_lock = use_thread_lock;
    
    if (local_use_thread_lock) lock_acquire(&thread_creation_lock);

    void* memptr;
    // try to alloc PGSIZE*2 bytes of mem, to be able to align stack to next free page
    if ((memptr = malloc(PGSIZE*2)) == 0) 
    {
        if (local_use_thread_lock) lock_release(&thread_creation_lock);
        return -1;
    }

    // stack address must be aligned to the next available PGSIZE-aligned va
    void* stack;
    uint szCheck = (uint)memptr % PGSIZE;
    if (szCheck == 0) stack = memptr;
    else stack = memptr - szCheck + PGSIZE;


    // find a free place to put entry in thread_stacks[]
    int i;
    for (i=0; i<NPROC; i++)
    {
        if (thread_stacks[i].stack == NULL && thread_stacks[i].p == NULL) 
        {
            thread_stacks[i].p = memptr;
            thread_stacks[i].stack = stack;
            lprintf(1, "Thread stack pointer: %p.\n", stack);
            break;
        }
    }

    int threadpid;
    // try to clone() - create new thread
    // int clone(void (*func)(void*), void* arg, void* stack)
    if ((threadpid = clone(start_routine, arg, stack)) < 0)
    {
        // failed to create thread, free up entry in thread_stacks[]
        free(memptr);
        thread_stacks[i].p = thread_stacks[i].stack = NULL;
    }

    if (local_use_thread_lock) lock_release(&thread_creation_lock);

    return threadpid;
}



int thread_join()
{
    void* stack = NULL;     // will point to stack of thread, arg filled by join()

    // call to reap a child-thread (or sleep until a child wakes us up)
    // int join(void **stack)
    int threadpid = join(&stack);

    // a zombie thread-child was found and reaped
    if (threadpid >= 0 && stack !=  NULL)
    {
        // get corresponding entry in thread_stacks (by comparing stack ptr)
        for (int i=0; i<NPROC; i++)
        {
            if (thread_stacks[i].stack == stack)
            {
                // free up entry and stack 'p' memory
                free(thread_stacks[i].p);
                thread_stacks[i].p = thread_stacks[i].stack = NULL;
                break;
            }
        }
    }

    return threadpid;
}

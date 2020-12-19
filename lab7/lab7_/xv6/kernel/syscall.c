#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "sysfunc.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from process p.
int
fetchint(struct proc *p, uint addr, int *ip)    // 4 bytes
{
  // if addr is outside code+heap && stack, return -1.
  // if addr is in first page, return -1.
  if( ((addr >= p->sz)&&(addr < (USERTOP-p->nPg_stk*PGSIZE))) || 
      ((addr+4 > p->sz)&&(addr+4 < (USERTOP-p->nPg_stk*PGSIZE)))|| 
      addr==0 || addr >= USERTOP || addr+4 > USERTOP)
    return -1;
  
  if (addr >0 && addr < PGSIZE) {
    //cprintf("proc %s, pid %d is accessing addr in first page \n", p->name, p->pid);
    if (p->pid != 1)
      return -1;
  }

  
  if ((addr >= proc->sz_code && addr < (proc->sz_code + PGSIZE)) || 
      ((addr+4) > proc->sz_code && (addr+4) < (proc->sz_code + PGSIZE))
      )
  {
    cprintf("int : addr %x from pid %d is illegal. proc code sz = %x  \n", addr, proc->pid, proc->sz_code);
    return -1;
  }
  

  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from process p.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(struct proc *p, uint addr, char **pp)
{
  char *s, *ep;

  if( ((addr >= p->sz)&&(addr <(USERTOP-p->nPg_stk*PGSIZE)))
    || (addr==0) || addr >= USERTOP)
    return -1;
  if (addr >0 && addr < PGSIZE) {
    //cprintf("proc %s, pid %d is accessing addr in first page \n", p->name, p->pid);
    if (p->pid != 1)
      return -1;
  }
  
  if (addr >= p->sz_code && addr < (p->sz_code + PGSIZE)) {
cprintf("str : addr %x from pid %d is illegal. \n", addr, p->pid);

    return -1;
  }
  
  
  *pp = (char*)addr;
  // addr in code/heap then last legal addr is p->sz. Else it is in stack and last legal addr is USERTOP. 
  if (addr < p->sz) 
    ep = (char*)p->sz;
  else
    ep = (char*)USERTOP;
  for(s = *pp; s < ep; s++)
    if(*s == 0)
      return s - *pp;
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint(proc, proc->tf->esp + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;
  
  if(argint(n, &i) < 0)
    return -1;
  if((((uint)i >= proc->sz)&&((uint)i < (USERTOP-proc->nPg_stk*PGSIZE))) ||
      (((uint)(i+size) > proc->sz)&&((uint)(i+size) < (USERTOP-proc->nPg_stk*PGSIZE))) ||
      ((uint)i == 0) || (uint)i >= USERTOP || (uint)(i+size) > USERTOP)
    return -1;
  
  if ((uint)i >0 && (uint)i < PGSIZE) {
    //cprintf("proc %s, pid %d is accessing addr in first page \n", proc->name, proc->pid);
    if (proc->pid != 1)
      return -1;
  }
 
  if (
      ((uint)i >= proc->sz_code && (uint)i < (proc->sz_code + PGSIZE)) || 
      (((uint)i+size) > proc->sz_code && ((uint)i+size) < (proc->sz_code + PGSIZE))
      ) {
cprintf("ptr: addr %x from pid %d is illegal. \n", (uint)i, proc->pid);
    return -1;
  }
  

  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(proc, addr, pp);
}



// // Fetch the int at addr from process p.
// int
// fetchint(struct proc *p, uint addr, int *ip)
// {
//   if(addr >= p->sz || addr+4 > p->sz)
//     return -1;
//   *ip = *(int*)(addr);
//   return 0;
// }

// // Fetch the nul-terminated string at addr from process p.
// // Doesn't actually copy the string - just sets *pp to point at it.
// // Returns length of string, not including nul.
// int
// fetchstr(struct proc *p, uint addr, char **pp)
// {
//   char *s, *ep;

//   if(addr >= p->sz)
//     return -1;
//   *pp = (char*)addr;
//   ep = (char*)p->sz;
//   for(s = *pp; s < ep; s++)
//     if(*s == 0)
//       return s - *pp;
//   return -1;
// }

// // Fetch the nth 32-bit system call argument.
// int
// argint(int n, int *ip)
// {
//   return fetchint(proc, proc->tf->esp + 4 + 4*n, ip);
// }

// // Fetch the nth word-sized system call argument as a pointer
// // to a block of memory of size n bytes.  Check that the pointer
// // lies within the process address space.
// int
// argptr(int n, char **pp, int size)
// {
//   int i;
  
//   if(argint(n, &i) < 0)
//     return -1;
//   if((uint)i >= proc->sz || (uint)i+size > proc->sz)
//     return -1;
//   *pp = (char*)i;
//   return 0;
// }

// // Fetch the nth word-sized system call argument as a string pointer.
// // Check that the pointer is valid and the string is nul-terminated.
// // (There is no shared writable memory, so the string can't change
// // between this check and being used by the kernel.)
// int
// argstr(int n, char **pp)
// {
//   int addr;
//   if(argint(n, &addr) < 0)
//     return -1;
//   return fetchstr(proc, addr, pp);
// }

extern int sys_count(void);
extern int countCalls;
extern int sys_getsyscallinfo(void);
extern char* sys_printstr(char* str);
extern int sys_vaSpace(void);


// syscall function declarations moved to sysfunc.h so compiler
// can catch definitions that don't match

// array of function pointers to handlers for all the syscalls
static int (*syscalls[])(void) = {
[SYS_chdir]   sys_chdir,
[SYS_close]   sys_close,
[SYS_dup]     sys_dup,
[SYS_exec]    sys_exec,
[SYS_exit]    sys_exit,
[SYS_fork]    sys_fork,
[SYS_fstat]   sys_fstat,
[SYS_getpid]  sys_getpid,
[SYS_kill]    sys_kill,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_mknod]   sys_mknod,
[SYS_open]    sys_open,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_unlink]  sys_unlink,
[SYS_wait]    sys_wait,
[SYS_write]   sys_write,
[SYS_uptime]  sys_uptime,
[SYS_count]   sys_count,
[SYS_getsyscallinfo]   sys_getsyscallinfo,
[SYS_printstr]   sys_printstr,
[SYS_vaSpace] sys_vaSpace,
};

// Called on a syscall trap. Checks that the syscall number (passed via eax)
// is valid and then calls the appropriate handler for the syscall.
void
syscall(void)
{
  int num;
  countCalls++;

  num = proc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num] != NULL) {
    proc->tf->eax = syscalls[num]();
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            proc->pid, proc->name, num);
    proc->tf->eax = -1;
  }
}

#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

// system call that creates (allocs and initializes) the user part of an address space.
// initializes the user part of an address space from a file stored in the file system
int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;

  if((ip = namei(path)) == 0)
    return -1;
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  // setupkvm() allocates new page-table (with no user-mappings)
  // (sets up kernel part of page table)
  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program (text) into memory.
  // sz = 0;          // OLD starting point/size
  // NEW starting point/size: 1 PAGE / 4 KiB / 0x1000
  sz = PGSIZE;        // requires changes to copyuvm(), fork() will work
  
  // for each ELF segment (load code at start of user space) 
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    // read from file
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    // allocate memory for each ELF segment
    // int allocuvm(pde_t *pgdir, uint oldsz, uint newsz) -- returns new size
    // for each page kalloc(), memset(0), mappages()
    if((sz = allocuvm(pgdir, sz, ph.va + ph.memsz)) == 0)
      goto bad;
    // load each segment into memory
    // int loaduvm(pde_t *pgdir, char *addr, struct inode *ip, uint offset, uint sz)
    // requires that addr must be page-aligned and the pages from addr to addr+sz must already be mapped
    if(loaduvm(pgdir, (char*)ph.va, ip, ph.offset, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  ip = 0;

  // -------------- old ---------------------------------------  
  // // Allocate a one-page stack at the next page boundary
  // sz = PGROUNDUP(sz);
  // if((sz = allocuvm(pgdir, sz, sz + PGSIZE)) == 0)
  //   goto bad;
  // ----------------------------------------------------------

  // round up sz
  // #define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
  sz = PGROUNDUP(sz);

  // store size of code seg in proc struct
  proc->sz_code = sz;

  // leave one page unmapped between code and heap (why though ???)
  // sz += PGSIZE;

  // allocate initial 1-page stack (last page of user space, for a start) VA = 636K->640K
  
  sp = USERTOP - PGSIZE;    // 640K - 4K = 636K
  proc->gp = sp - PGSIZE;   // the page below stack 

  // alloc 1 page initally for stack (636K to 640K)
  if((sp = allocuvm(pgdir, sp, sp + PGSIZE)) == 0)
    goto bad;

  // Push argument strings, prepare rest of stack in ustack.
  // sp = sz;
  // fill stack-page with main() args
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp &= ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;     // stack ptr after copying ustack
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(proc->name, last, sizeof(proc->name));

  // Commit to the user image.
  oldpgdir = proc->pgdir;
  proc->pgdir = pgdir;
  proc->sz = sz;
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;

  // init custom vars in proc
  proc->nPg_stk = 1;      // size of stack = 1 initially
  proc->max_nPg_stk = 0;  // this will allow to have as many stack pages as usable free-space in uvm

  switchuvm(proc);
  freevm(oldpgdir);

  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip)
    iunlockput(ip);
  return -1;
}

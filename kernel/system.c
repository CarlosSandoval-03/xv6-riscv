#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "stat.h"
#include "proc.h"
#include "sysinfo.h"

int
get_system_info(uint64 addr)
{
  struct proc *p = myproc();
  struct sysinfo info;

  if ((info.freemem = num_free_mem()) < 0 || (info.nproc = num_active_proc()) < 0)
    return -1;

  if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}

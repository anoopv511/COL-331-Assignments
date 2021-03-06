#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// toggles the system trace (1 - ON | 0 - OFF)
int toggle_state = 0;
int
sys_toggle(void)
{
  toggle_state = toggle_state == 0 ? 1 : 0;
  return toggle_state;
}

// add two numbers
int
sys_add(void)
{
  int a;
  int b;

  if(argint(0, &a) < 0)
    return -1;
  if(argint(1, &b) < 0)
    return -1;
  return a + b;
}

extern int ps(void);

// list all processes
int
sys_ps(void)
{
  return ps();
}

extern int setpriority(int,int);

// set priority
int
sys_setpriority(void)
{
  int pid;
  int priority;

  if(argint(0, &pid) < 0)
    return -1;
  if(argint(1, &priority) < 1 && argint(1, &priority) > 20){
    cprintf("Error\n");
    return -1;
  }
  return setpriority(pid,priority);
}

extern int getpriority(int);

// get priority
int
sys_getpriority(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return getpriority(pid);
}
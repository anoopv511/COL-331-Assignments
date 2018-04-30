#include "types.h"
#include "user.h"
#include "uthread.c"

extern int schedule_mode;

void 
mythread(void)
{
  int i;
  printf(1, "%s: start\n",current_thread->name);
  for (i = 0; i < 20; i++) {
    printf(1, "%s (%d) -> %d\n", current_thread->name,current_thread->priority,i);
    thread_yield();
  }
  printf(1, "%s: exit\n",current_thread->name);
  current_thread->state = FREE;
  thread_schedule();
  return;
}

int
main(void)
{
  schedule_mode = 2;
  thread_init();
  thread_create("T0", mythread, 6);
  thread_create("T1", mythread, 5);
  thread_create("T2", mythread, 7);
  thread_schedule();
  exit();
}
#include "types.h"
#include "user.h"
#include "uthread.c"

void 
mythread(void)
{
  int i;
  printf(1, "Thread: running\n");
  for (i = 0; i < 5; i++) {
    printf(1, "Thread 0x%x -> i = %d\n", (int) current_thread,i);
    thread_yield();
  }
  printf(1, "Thread: exit\n");
  current_thread->state = FREE;
  thread_schedule();
  return;
}

int
main(void)
{
  thread_init();
  thread_create("T0", mythread);
  thread_create("T1", mythread);
  thread_schedule();
	exit();
}
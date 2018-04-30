#include "types.h"
#include "user.h"
#include "uthread.c"

int sum = 0;
int arr[] = {1,2,3,4,5,6,7,8,9,10};
int i = 0;
lock lk;

void 
add(void)
{
  printf(1, "%s: start\n",current_thread->name);
  lock_acquire(&lk);
  for ( ;i < 10; ) {
    thread_yield();
    print_states();
    sum += arr[i++];
    printf(1,"%s -> Sum = %d\n",current_thread->name,sum);
    lock_wait_release(&lk);
    lock_acquire(&lk);
  }
  lock_wait_release(&lk);
  printf(1, "%s: exit\n",current_thread->name);
  current_thread->state = FREE;
  thread_schedule();
  return;
}

int
main(void)
{
  thread_init();
  thread_create("T0", add, 5);
  thread_create("T1", add, 5);
  initlock(&lk);
  thread_schedule();
	exit();
}
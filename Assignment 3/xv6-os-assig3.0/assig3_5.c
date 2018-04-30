#include "types.h"
#include "user.h"
#include "uthread.c"

int sum = 0;
int arr[] = {1,2,3,4,5,6,7,8,9,10};
int i = 0;
lock lk;
int flag = 0;

void 
mythread(void)
{
  printf(1, "%s: start\n",current_thread->name);
  int j = 0;
  for(j = 0;j < 10;j++){
      printf(1,"%s (%d) : ---\n",current_thread->name,current_thread->priority);
  }
  printf(1, "%s: exit\n",current_thread->name);
  current_thread->state = FREE;
  thread_schedule();
  return;  
}

void 
add(void)
{
  printf(1, "%s: start\n",current_thread->name);
  if(current_thread->name[1] == '0'){
    lock_busy_wait_acquire(&lk);
      if(flag == 0){
        thread_create("T1", add, 15);
        thread_create("T2", mythread, 6);
        printf(1, "Threads created\n");
        flag = 1;
      }
    thread_yield();
    int j = 0;
    for(j = 0;j < 10;j++){
      printf(1,"%s (%d) : ---\n",current_thread->name,current_thread->priority);
    }
    lock_busy_wait_release(&lk);
  }
  lock_busy_wait_acquire(&lk);
  for ( ;i < 10; ) {
    thread_yield();
    // print_states();
    sum += arr[i++];
    printf(1,"%s (%d) -> Sum = %d\n",current_thread->name,current_thread->priority,sum);
    lock_busy_wait_release(&lk);
    lock_busy_wait_acquire(&lk);
  }
  lock_busy_wait_release(&lk);
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
  thread_create("T0", add, 5);
  initlock(&lk);
  thread_schedule();
  exit();
}
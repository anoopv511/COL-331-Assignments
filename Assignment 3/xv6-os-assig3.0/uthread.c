#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"

static thread all_thread[MAX_THREAD];
thread*  current_thread;
thread*  next_thread;
extern void thread_switch(void);

void 
thread_init(void)
{
  current_thread = &all_thread[0];
  current_thread->state = RUNNABLE;
}

static void 
thread_schedule(void)
{
  thread* t, *t1;
  int flag = 1;

  /* Find another runnable thread. */
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == RUNNABLE && t != current_thread) {
      next_thread = t;
      break;
    }
  }

  for(t1 = all_thread; t1 < all_thread + MAX_THREAD; t1++){
    if(t1->state == RUNNABLE){
      flag = 0;
      break;
    }
  }

  if (t >= all_thread + MAX_THREAD && current_thread->state == RUNNABLE) {
    /* The current thread is the only runnable thread; run it. */
    next_thread = current_thread;
  }

  if (next_thread == 0 && !flag) {
    printf(1, "thread_schedule: no runnable threads; deadlock\n");
    exit();
  }

  if (flag) {
    printf(1, "thread_schedule: no runnable threads\n");
    exit();
  }

  if (current_thread != next_thread) {         /* switch threads?  */
    next_thread->state = RUNNING;
    thread_switch();
  } else
    next_thread = 0;
}

int
thread_create(char* name, void (*func)())
{
  thread* t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }

  if(t->state != FREE){
    return 0;
  }

  t->name = (char*)malloc(sizeof(char)*strlen(name));   // malloc name field
  strcpy(t->name,name);                                 // assign names
  t->sp = (int) (t->stack + STACK_SIZE);   // set sp to the top of the stack
  t->sp -= 4;                              // space for return address
  *(int *) (t->sp) = (int)func;            // push return address on stack
  t->sp -= 32;                             // space for registers that thread_switch will push
  t->state = RUNNABLE;

  return 1;
}

void 
thread_yield(void)
{
  current_thread->state = RUNNABLE;
  thread_schedule();
}
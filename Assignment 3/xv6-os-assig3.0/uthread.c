#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"

static thread all_thread[MAX_THREAD];
thread*  current_thread;
thread*  next_thread;
extern void thread_switch(void);

int schedule_mode = 0;

void 
thread_init(void)
{
  current_thread = &all_thread[0];
  current_thread->name = "main";
  current_thread->priority = 20;
  current_thread->counter = 0;
  current_thread->state = RUNNING;
}

void 
print_states(void)
{
  thread* t;
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    printf(1,"%s (%d) - %d\n",t->name,t->priority,t->state);
  }
}

static void 
basic_schedule(void)
{
  thread *t, *t1;
  int flag = 1;

  /* Find another runnable thread. */
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == RUNNABLE && t != current_thread) {
      next_thread = t;
      break;
    }
  }

  for(t1 = all_thread; t1 < all_thread + MAX_THREAD; t1++){
    if(t1->state == RUNNABLE || t1->state == WAITING){
      flag = 0;
      break;
    }
  }

  if (t >= all_thread + MAX_THREAD && current_thread->state == RUNNABLE) {
    /* The current thread is the only runnable thread; run it. */
    next_thread = current_thread;
  }

  if (next_thread == 0 && !flag) {
    print_states();
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

static void 
priority_schedule(void)
{
  thread *t, *t1;
  int flag = 1;

  /* Find another runnable thread. */
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == RUNNABLE) {
      next_thread = t;
      break;
    }
  }

  for (t1 = all_thread; t1 < all_thread + MAX_THREAD; t1++) {
    if (t1->state == RUNNABLE && t1->priority > next_thread->priority) {
      next_thread = t1;
    }
  }

  for(t1 = all_thread; t1 < all_thread + MAX_THREAD; t1++){
    if(t1->state == RUNNABLE || t1->state == WAITING){
      flag = 0;
      break;
    }
  }

  if (t >= all_thread + MAX_THREAD && current_thread->state == RUNNABLE) {
    /* The current thread is the only runnable thread; run it. */
    next_thread = current_thread;
  }

  if (next_thread == 0 && !flag) {
    print_states();
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

static void 
priority_aging_schedule(void)
{
  thread *t, *t1;
  int flag = 1;

  /* Find another runnable thread. */
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == RUNNABLE) {
      next_thread = t;
      break;
    }
  }

  for (t1 = all_thread; t1 < all_thread + MAX_THREAD; t1++) {
    if (t1->state == RUNNABLE && t1->priority > next_thread->priority) {
      next_thread = t1;
    }
  }

  for(t1 = all_thread; t1 < all_thread + MAX_THREAD; t1++){
    if(t1->state == RUNNABLE || t1->state == WAITING){
      flag = 0;
      break;
    }
  }

  if (t >= all_thread + MAX_THREAD && current_thread->state == RUNNABLE) {
    /* The current thread is the only runnable thread; run it. */
    next_thread = current_thread;
  }

  if (next_thread == 0 && !flag) {
    print_states();
    printf(1, "thread_schedule: no runnable threads; deadlock\n");
    exit();
  }

  if (flag) {
    printf(1, "thread_schedule: no runnable threads\n");
    exit();
  }

  for(t = all_thread; t < all_thread + MAX_THREAD; t++){
    if(t != next_thread && t->state == RUNNABLE){
      t->counter++;
      if(t->counter >= 10){
        t->counter = 0;
        t->priority = t->priority == 20 ? t->priority : t->priority + 1;
      }
    }
  }

  if (current_thread != next_thread) {         /* switch threads?  */
    next_thread->state = RUNNING;
    thread_switch();
  } else
    next_thread = 0;
}

static void 
thread_schedule(void)
{
  switch(schedule_mode)
  {
    case 0:
      basic_schedule();
      break;
    case 1:
      priority_schedule();
      break;
    case 2:
      priority_aging_schedule();
      break;
    default:
      basic_schedule();
      break;
  }
}

int
thread_create(char* name, void (*func)(), int priority)
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
  t->priority = priority;                               // assign priority
  t->counter = 0;                                       // init counter
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

void 
initlock(struct lock *lk)
{
  lk->locked = 0;
  lk->counter = 0;
}

int 
test_and_set(lock *lk)
{
	int rv = lk->locked;
  lk->locked = 1;
  return rv;
}

void 
lock_busy_wait_acquire(lock *lk)
{
	if(test_and_set(lk) == 1){
    lk->waitarr[lk->counter] = current_thread;
		lk->counter += 1;
    if(lk->holder_priority < current_thread->priority){
      lk->holder->priority = current_thread->priority;
    }
		thread_yield();
    lock_busy_wait_acquire(lk);
	}
  else{
    lk->holder = current_thread;    
    lk->holder_priority = lk->holder->priority;
		lk->locked = 1;
	}
}

void 
lock_busy_wait_release(lock *lk){
	lk->locked = 0;
  int i;
	for(i=0;i<lk->counter;i++){
		lk->waitarr[i] = 0;
	}
  lk->holder->priority = lk->holder_priority;
  if(lk->counter != 0){
    lk->counter = 0;
    thread_yield();
  }
  lk->counter = 0;
}

void 
lock_acquire(lock *lk){
	if(test_and_set(lk) == 1){
		current_thread->state = WAITING;
		lk->waitarr[lk->counter] = current_thread;
		lk->counter += 1;
    if(lk->holder_priority < current_thread->priority){
      lk->holder->priority = current_thread->priority;
    }
		thread_schedule();
    lock_acquire(lk);
	}else{
    lk->holder = current_thread;
    lk->holder_priority = lk->holder->priority;
		lk->locked = 1;
	}
}

void 
lock_wait_release(lock *lk){
	lk->locked = 0;
	int i;
	for(i=0;i<lk->counter;i++){
		(lk->waitarr[i])->state = RUNNABLE;
		lk->waitarr[i] = 0;
	}
  lk->holder->priority = lk->holder_priority;
  if(lk->counter != 0){
    lk->counter = 0;
    thread_yield();
  }
  lk->counter = 0;
}
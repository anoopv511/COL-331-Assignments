// Thread States
#define FREE        0
#define RUNNING     1
#define RUNNABLE    2
#define WAITING     3

#define STACK_SIZE  8192
#define MAX_THREAD  4

// Thread Structure 
typedef struct thread {
  int  sp;                      // curent stack pointer
  char stack[STACK_SIZE];       // the thread's stack
  int  state;                   // running, runnable, waiting
  char* name;                   // name of thread
} thread;

// Lock Structure
typedef struct lock {
  int locked;        // Lock status
  thread *waitarr[MAX_THREAD];    // Threads waiting on lock
  int counter;       // Number of waiting threads
} lock;
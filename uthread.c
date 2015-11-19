#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "mmu.h"

uint stack_thread[64];
int upper_bound = 64; 


//int PGSIZE = 4096;

void lock_init(lock_t *lock) {
	lock->locked = 0; 
}

void lock_acquire(lock_t *lock) {
	while(xchg(&lock->locked, 1) != 0);
}

void lock_release(lock_t *lock) {
	xchg(&lock->locked,0);
}

int thread_join(int pid) {
	
	int next_pid = join(pid);
	if(pid != -1) {
		free((void*)stack_thread[next_pid-1]);
	}
	return next_pid;
}

int thread_create(void (*start_routine)(void *), void *arg) {
	
	void *stack = malloc((uint)PGSIZE * 2);

	if((uint)stack%PGSIZE) {
		stack = stack + (PGSIZE -(uint)stack % PGSIZE);
	}
	
	int pid = clone(start_routine, arg, stack); 
	
	if (pid > 0) {
		if (pid < upper_bound) {
			stack_thread[pid-1] = (uint)stack;
		}
	}
	//stack_thread[pid] = stack;	
	
	return pid;
}




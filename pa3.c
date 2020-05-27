#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "types.h"
#include "locks.h"
#include "atomic.h"

/******************************************************************
 * Spinlock implementation
 */
void init_spinlock(struct spinlock *lock)
{
	return;
}

void acquire_spinlock(struct spinlock *lock)
{
	return;
}

void release_spinlock(struct spinlock *lock)
{
	return;
}


/******************************************************************
 * Blocking mutex implementation
 *
 * Hint: Use pthread_self, pthread_kill, pause, and signal
 */
void init_mutex(struct mutex *lock)
{
	return;
}

void acquire_mutex(struct mutex *lock)
{
	return;
}

void release_mutex(struct mutex *lock)
{
	return;
}


/******************************************************************
 * Semaphore implementation
 *
 * Hint: Use pthread_self, pthread_kill, pause, and signal
 */
void init_semaphore(struct semaphore *sem, int S)
{
	return;
}

void wait_semaphore(struct semaphore *sem)
{
	return;
}

void signal_semaphore(struct semaphore *sem)
{
	return;
}


/******************************************************************
 * Spinlock tester exmaple
 */
struct spinlock testlock;
int testlock_held = 0;

void *test_thread(void *_arg_)
{
	usleep(random() % 1000 * 1000);

	printf("Tester is acquiring the lock...\n");
	acquire_spinlock(&testlock);
	printf("Tester acquired the lock\n");
	assert(testlock_held == 0);
	testlock_held = 1;

	usleep(100);

	printf("Tester releases the lock\n");
	testlock_held = 0;
	release_spinlock(&testlock);
	printf("Tester released the lock\n");
	return 0;
}

void test_lock(void)
{
	/* Set nr_testers as you need
	 *  1: one main, one tester. easy :-)
	 * 16: one main, 16 testers contending the lock :-$
	 */
	const int nr_testers = 1;
	int i;
	pthread_t tester[nr_testers];

	printf("Main initializes the lock\n");
	init_spinlock(&testlock);

	printf("Main graps the lock...");
	acquire_spinlock(&testlock);
	assert(testlock_held == 0);
	testlock_held = 1;
	printf("acquired!\n");

	for (i = 0; i < nr_testers; i++) {
		pthread_create(tester + i, NULL, test_thread, NULL);
	}

	sleep(1);

	printf("Main releases the lock\n");
	testlock_held = 0;
	release_spinlock(&testlock);
	printf("Main released the lock\n");

	for (i = 0; i < nr_testers; i++) {
		pthread_join(tester[i], NULL);
	}
	assert(testlock_held == 0);
	printf("Your spinlock implementation looks OK.\n");

	return;
}




/*********************************************************************
 * Ring buffer
 *
 * DO NOT ALLOCATE SLOTS AS ARRAY
 */
static int nr_slots = 0;

static enum lock_types lock_type;

void (*enqueue_fn)(int value) = NULL;
int (*dequeue_fn)(void) = NULL;

void enqueue_ringbuffer(int value)
{
	assert(enqueue_fn);
	assert(value >= MIN_VALUE && value < MAX_VALUE);

	enqueue_fn(value);
}

int dequeue_ringbuffer(void)
{
	int value;

	assert(dequeue_fn);

	value = dequeue_fn();
	assert(value >= MIN_VALUE && value < MAX_VALUE);

	return value;
}


/*********************************************************************
 * TODO: Implement using spinlock
 */
void enqueue_using_spinlock(int value)
{
}

int dequeue_using_spinlock(void)
{
	return 0;
}

void init_using_spinlock(void)
{
	enqueue_fn = &enqueue_using_spinlock;
	dequeue_fn = &dequeue_using_spinlock;
}

void fini_using_spinlock(void)
{
}


/*********************************************************************
 * TODO: Implement using mutex
 */
void enqueue_using_mutex(int value)
{
}

int dequeue_using_mutex(void)
{
	return 0;
}

void init_using_mutex(void)
{
	enqueue_fn = &enqueue_using_mutex;
	dequeue_fn = &dequeue_using_mutex;
}

void fini_using_mutex(void)
{
}


/*********************************************************************
 * TODO: Implement using semaphore
 */
void enqueue_using_semaphore(int value)
{
}

int dequeue_using_semaphore(void)
{
	return 0;
}

void init_using_semaphore(void)
{
	enqueue_fn = &enqueue_using_semaphore;
	dequeue_fn = &dequeue_using_semaphore;
}

void fini_using_semaphore(void)
{
}


/*********************************************************************
 * Common implementation
 */
int init_ringbuffer(const int _nr_slots_, const enum lock_types _lock_type_)
{
	assert(_nr_slots_ > 0);
	nr_slots = _nr_slots_;

	/* Initialize lock! */
	lock_type = _lock_type_;
	switch (lock_type) {
	case lock_spinlock:
		init_using_spinlock();
		break;
	case lock_mutex:
		init_using_mutex();
		break;
	case lock_semaphore:
		init_using_semaphore();
		break;
	}

	/* TODO: Initialize your ringbuffer and synchronization mechanism */

	return 0;
}

void fini_ringbuffer(void)
{
	/* TODO: Clean up what you allocated */
	switch (lock_type) {
	default:
		break;
	}
}

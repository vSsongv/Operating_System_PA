/**********************************************************************
 * Copyright (c) 2020
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "types.h"
#include "locks.h"

static struct mutex *testlock = NULL;;
int testlock_held = 0;

/* Set nr_testers as you need
 *  1: one main, one tester. easy :-)
 * 16: one main, 16 testers contending the lock :-$
 */
const int nr_testers = 32;
const int nr_iterate = 100000;

void *test_thread(void *_arg_)
{
	long id = (long)_arg_;

	for (int i = 0; i < nr_iterate; i++) {
		if (verbose) printf("Tester %d is acquiring the lock\n", id);
		acquire_mutex(testlock);

		if (verbose) printf("Tester %d acquired the lock\n", id);
		assert(testlock_held == 0);
		testlock_held = 1;

		if (verbose) printf("Tester %d is releasing the lock\n", id);

		assert(testlock_held == 1);
		testlock_held = 0;
		assert(testlock_held == 0);
		release_mutex(testlock);
		if (verbose) printf("Tester %d released the lock\n", id);
	}
	return 0;
}

void test_lock(void)
{
	long i;
	pthread_t tester[nr_testers];

	printf("Main initializes the lock\n");
	testlock = malloc(4096); /* We don't know the exact size of the object
								here. So, just allocate a big memory, and 
								ask to initialize it as a lock */
	init_mutex(testlock);

	printf("Main graps the lock...");
	acquire_mutex(testlock);
	assert(testlock_held == 0);
	testlock_held = 1;
	printf("acquired!\n");

	for (i = 0; i < nr_testers; i++) {
		pthread_create(tester + i, NULL, test_thread, (void *)i);
	}

	sleep(10);

	printf("Main releases the lock\n");
	testlock_held = 0;
	release_mutex(testlock);
	printf("Main released the lock\n");

	for (i = 0; i < nr_testers; i++) {
		pthread_join(tester[i], NULL);
	}
	assert(testlock_held == 0);
	printf("\n\n >>>> Congraturations! Your spinlock implementation looks OK!! <<<<\n\n\n");

	return;
}

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
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "types.h"
#include "ringbuffer.h"
#include "counter.h"

static unsigned long nr_requests = 0;
static pthread_t counter_thread;
static unsigned long occurrances[MAX_VALUE] = { 0 };

static int __initialized = 0;
static int delayed = 0;

static void dump_counting_result(void)
{
	int i;
	unsigned long nr = 0;
	FILE *fp = fopen(COUNTER_FILENAME, "w");
	assert(fp);
	
	if (verbose) {
		printf("Occurrances ------\n");
	}
	for (i = MIN_VALUE; i < MAX_VALUE; i++) {
		if (occurrances[i] == 0) continue;

		if (verbose) {
			printf("    %3d : %lu\n", i, occurrances[i]);
		}		
		fprintf(fp, "%d %lu\n", i, occurrances[i]);
		nr += occurrances[i];
	}
	if (verbose) {
		printf("  Total : %lu\n", nr);
	}

	if (fp) {
		fclose(fp);
	}
}

void *counter_main(void *_args_)
{
	unsigned long i;

	if (verbose) {
		printf("Counting %lu numbers...\n", nr_requests);
	}

	for (i = 0; i < nr_requests; i++) {
		int value = dequeue_ringbuffer();

		if (verbose && i && i % 1000000 == 0) {
			printf("%lu k / %lu k counted\n", (i >> 10), (nr_requests >> 10));
		}

		occurrances[value]++;
		if (delayed) usleep(10);
	}

	if (verbose) {
		printf("Counting finished...\n");
	}

	dump_counting_result();
	return 0;
}

int spawn_counter(const enum counter_types type, const unsigned long _nr_requests_)
{
	nr_requests = _nr_requests_;
	if (type == counter_delayed) {
		delayed = 1;
	}
	pthread_create(&counter_thread, NULL, counter_main, NULL);
	__initialized = 1;
	return 0;
}

void fini_counter(void)
{
	if (__initialized) {
		pthread_join(counter_thread, NULL);
	}
}


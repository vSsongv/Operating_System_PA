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
static pthread_t counter_thread = 0;
static unsigned long value_counter[MAX_VALUE] = { 0 };
static unsigned long print_interval = 100000;

static int delayed = 0;

void *counter_main(void *_args_)
{
	unsigned long i;

	if (verbose) {
		printf("Counting %lu requests...\n", nr_requests);
	}

	for (i = 0; i < nr_requests; i++) {
		int value = dequeue_ringbuffer();

		if (verbose && i && i % print_interval == 0) {
			printf("%lu k / %lu k counted\n", (i >> 10), (nr_requests >> 10));
		}

		value_counter[value]++;
		if (delayed) usleep(10);
	}

	if (verbose) {
		printf("Counting finished...\n");
	}

	// dump_counting_result();
	return 0;
}

int spawn_counter(const enum counter_types type, const unsigned long _nr_requests_)
{
	nr_requests = _nr_requests_;
	print_interval = nr_requests / 10 > print_interval ? print_interval : nr_requests / 10;

	if (type == counter_delayed) {
		delayed = 1;
	}
	pthread_create(&counter_thread, NULL, counter_main, NULL);
	return 0;
}

void fini_counter(void)
{
	if (counter_thread) {
		pthread_join(counter_thread, NULL);
	}
}

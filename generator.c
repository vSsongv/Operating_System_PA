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
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>

#include "types.h"
#include "generator.h"
#include "ringbuffer.h"

static int nr_generators = 0;
static unsigned long nr_generate = 0;

static int running = 0;

/* Generator functions */
int generator_fn_constant(int tid)
{
	return 43;
}

int generator_fn_random(int tid)
{
	return MIN_VALUE + (random() % (MAX_VALUE - MIN_VALUE));
}

int generator_fn_delayed(int tid)
{
	usleep(100);
	return generator_fn_random(tid);
}

int (*assign_generator_fn(enum generator_types type, int tid))(int)
{
	switch(type) {
	case generator_constant:
		return &generator_fn_constant;
	case generator_random:
		return &generator_fn_random;
	case generator_delayed:
		return &generator_fn_delayed;
	case generator_mixed:
		if (tid < 4) return &generator_fn_constant;
		return &generator_fn_random;
	default:
		assert(0);
		break;
	}

	/* Unreachable */
	assert(0);
}

struct generator {
	pthread_t thread;
	int tid;
	int (*generator_fn)(int tid);
	unsigned long generated[MAX_VALUE];
};
static struct generator *generators = NULL;

void *generator_main(void *_args_)
{
	unsigned long i;
	struct generator *my = (struct generator *)_args_;
	int value;

	if (verbose) {
		printf("Generator %d started...\n", my->tid);
	}

	for (i = 0; i < nr_generate && running; i++) {
		value = my->generator_fn(my->tid);

		enqueue_ringbuffer((int)value);
		my->generated[value]++;

		if (verbose && i && i % (1 << 10) == 0) {
			printf("Generator %d generated %lu k / %lu k\n",
					my->tid, (i >> 10), (nr_generate >> 10));
		}
	}
	if (verbose) {
		printf("Generator %d finished...\n", my->tid);
	}

	return 0;
}

int spawn_generators(const enum generator_types type, const int _nr_generators_, const unsigned long _nr_generate_)
{
	int i;

	assert(_nr_generators_ > 0);
	nr_generators = _nr_generators_;

	assert(_nr_generate_ > 0);
	nr_generate = _nr_generate_;

	generators = calloc(nr_generators, sizeof(*generators));
	assert(generators);
	bzero(generators, sizeof(*generators) * nr_generators);

	running = 1;
	for (i = 0; i < nr_generators; i++) {
		struct generator *g = generators + i;
		g->tid = i;
		g->generator_fn = assign_generator_fn(type, i);
		pthread_create(&g->thread, NULL, generator_main, g);
	}
	return 0;
}

void dump_generation_result(const struct generator *g)
{
	char filename[80];
	FILE *fp;
	int value;

	/* Log the generating summary */
	snprintf(filename, sizeof(filename), GENERATOR_FILENAME, g->tid);
	fp = fopen(filename, "w");
	assert(fp);
	for (value = MIN_VALUE; value < MAX_VALUE; value++) {
		if (!g->generated[value]) continue;
		fprintf(fp, "%d %lu\n", value, g->generated[value]);
	}
	fclose(fp);
}


void fini_generators(void)
{
	int i;
	if (running) {
		for (i = 0; i < nr_generators; i++) {
			struct generator *g = generators + i;
			if ((unsigned long)(g->thread) != 0) {
				pthread_join(g->thread, NULL);
			}
		}
		free(generators);
	}
}

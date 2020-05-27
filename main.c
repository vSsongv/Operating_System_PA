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
#include <errno.h>
#include <pthread.h>

#include "types.h"

#include "locks.h"
#include "ringbuffer.h"
#include "generator.h"
#include "counter.h"

/* Common */
int verbose = 1;

/* Generator */
static enum generator_types generator_type = generator_constant;
static int nr_generators = 1;
static unsigned long nr_generate = 128;

/* Counter */
static enum counter_types counter_type = counter_normal;

/* Ring buffer */
static int nr_slots = 32;
static enum lock_types lock_type = lock_spinlock;

int parse_options(int argc, char * const argv[])
{
	char opt;
	while ((opt = getopt(argc, argv, "vqg:s:n:R:rSM01234h?T")) != -1) {
		switch(opt) {
		case 'v':
			verbose = 1;
			break;
		case 'q':
			verbose = 0;
			break;
		case 'T':
			test_lock();
			exit(0);
		case 'R':
			srandom(atoi(optarg));
			break;
		case 'r':
			generator_type = generator_random;
			break;
		case 'g':
			nr_generators = atoi(optarg);
			break;
		case 'n':
			nr_generate = atoll(optarg);
			break;
		case 'S':
			lock_type = lock_semaphore;
			break;
		case 'M':
			lock_type = lock_mutex;
			break;
		case 's':
			nr_slots = atoi(optarg);
			break;
		case '0':
			generator_type = generator_random;
			nr_generators = 4;
			nr_generate = (1 << 12);
			lock_type = lock_spinlock;
			verbose = 0;
			break;
		case '1':
			generator_type = generator_random;
			nr_generators = 4;
			nr_generate = (1 << 12);
			lock_type = lock_mutex;
			verbose = 0;
			break;
		case '2':
			generator_type = generator_random;
			nr_generators = 4;
			nr_generate = (1 << 12);
			lock_type = lock_semaphore;
			verbose = 0;
			break;
		case '3': /* Overflow test */
			generator_type = generator_random;
			nr_generators = 4;
			nr_generate = (1 << 10);
			counter_type = counter_delayed;
			verbose = 0;
			break;
		case '4': /* Underflow test */
			generator_type = generator_delayed;
			nr_generators = 4;
			nr_generate = (1 << 10);
			verbose = 0;
			break;
		case 'h':
		case '?':
		default:
			printf("Usage: %s {options}\n", argv[0]);
			printf("\n");
			printf("  -g [number]: Spawn @number generators for test\n", argv[0]);
			printf("  -n [number]: Generate @number requests per generator\n");
			printf("  -r         : Use random generator rather than constant generator\n");
			printf("\n");
			printf("  -s [number]: Initialize the ringbuffer with @number slots\n");
			printf("  -M         : Test mutex */\n");
			printf("  -S         : Test semaphore */\n");
			printf("  -h | -?    : Print usage */\n");
			printf("  -v | -q    : Make verbose or quiet */\n");
			printf("\n");
			return EXIT_FAILURE;
		}
	}
	return 0;
}

int main(int argc, char * const argv[])
{
	int retval = EXIT_SUCCESS;

	system("rm -f GEN* RESULT*");

	if ((retval = parse_options(argc, argv))) {
		goto exit;
	}
	if ((retval = init_ringbuffer(nr_slots, lock_type))) {
		goto exit;
	}

	if ((retval = spawn_counter(counter_type, nr_generators * nr_generate))) {
		goto exit_ring;
	}

	spawn_generators(generator_type, nr_generators, nr_generate);

	fini_generators();
	fini_counter();

exit_ring:
	fini_ringbuffer();
exit:
	return retval;
}

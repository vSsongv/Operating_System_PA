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

#ifndef __TYPES_H__
#define __TYPES_H__

typedef int bool;
#define true	1
#define false	0

enum lock_types {
	lock_spinlock = 0,
	lock_mutex = 1,
	lock_semaphore = 2,
};

#define MIN_VALUE 0
#define MAX_VALUE 128

extern int verbose;

#define GENERATOR_FILENAME "_generated_%d"
#define COUNTER_FILENAME "_counter"

#endif

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

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

enum lock_types;

int init_ringbuffer(const int nr_slots, const enum lock_types lock_type);

void fini_ringbuffer(void);

void enqueue_ringbuffer(int value);
int dequeue_ringbuffer(void);

#endif

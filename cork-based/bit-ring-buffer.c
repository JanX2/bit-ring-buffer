//
//  bit-ring-buffer.c
//  bit-ring-buffer
//
//  Created by Jan on 2017-10-01.
//
//  Based on the ring buffer implementation from libcork:
//  https://github.com/redjack/libcork
//
//

#include "bit-ring-buffer.h"

#include <stdlib.h>
#include <stdbool.h>


const bool true_value = true;
const bool *true_p = &true_value;

const bool false_value = false;
const bool *false_p = &false_value;


static void
jx_bit_ring_buffer_check_and_handle_index_wraparound(jx_bit_ring_buffer *self, size_t *index)
{
	if (*index == jx_bit_ring_buffer_get_allocated_size(self)) {
		*index = 0;
	}
}


bool
jx_bit_ring_buffer_init(jx_bit_ring_buffer *self, size_t bit_count)
{
	jx_bitset_init(&self->bitset, bit_count);
	//self->elements = calloc(bit_count, sizeof(void *));
	//self->allocated_size = bit_count;
	
	self->used_bit_count = 0;
	self->read_index = 0;
	self->write_index = 0;
	
	return true;
}

jx_bit_ring_buffer *
jx_bit_ring_buffer_new(size_t bit_count)
{
	jx_bit_ring_buffer *buf = malloc(sizeof(jx_bit_ring_buffer));
	jx_bit_ring_buffer_init(buf, bit_count);
	
	return buf;
}

void
jx_bit_ring_buffer_done(jx_bit_ring_buffer *self)
{
	jx_bitset_done(&self->bitset);
	//free(self->elements);
}

void
jx_bit_ring_buffer_free(jx_bit_ring_buffer *self)
{
	jx_bit_ring_buffer_done(self);
	free(self);
}

bool
jx_bit_ring_buffer_add(jx_bit_ring_buffer *self, bool element)
{
	if (jx_bit_ring_buffer_is_full(self)) {
		return false;
	}
	
	jx_bitset_set(&self->bitset, self->write_index, element);
	self->write_index += 1;
	self->used_bit_count += 1;
	
	jx_bit_ring_buffer_check_and_handle_index_wraparound(self, &self->write_index);

	return true;
}

void
jx_bit_ring_buffer_add_with_overwrite(jx_bit_ring_buffer *self, bool element)
{
	jx_bitset_set(&self->bitset, self->write_index, element);
	self->write_index += 1;
	jx_bit_ring_buffer_check_and_handle_index_wraparound(self, &self->write_index);
	
	if (!jx_bit_ring_buffer_is_full(self)) {
		self->used_bit_count += 1;
	}
}

const bool *
jx_bit_ring_buffer_pop(jx_bit_ring_buffer *self)
{
	if (jx_bit_ring_buffer_is_empty(self)) {
		return NULL;
	}
	else {
		bool result = jx_bitset_get(&self->bitset, self->read_index);
		self->read_index += 1;
		self->used_bit_count--;
		jx_bit_ring_buffer_check_and_handle_index_wraparound(self, &self->read_index);
		
		return result ? true_p : false_p;
	}
}

const bool *
jx_bit_ring_buffer_peek(jx_bit_ring_buffer *self)
{
	if (jx_bit_ring_buffer_is_empty(self)) {
		return NULL;
	}
	else {
		bool result = jx_bitset_get(&self->bitset, self->read_index);
		
		return result ? true_p : false_p;
	}
}

size_t
jx_bit_ring_buffer_population_count(jx_bit_ring_buffer *self)
{
	return jx_bitset_popcount(&self->bitset);
}

/*
 Copyright 2011-2014, RedJack, LLC.
 Copyright 2017 Jan Weiß
 
 Some rights reserved: https://opensource.org/licenses/BSD-3-Clause
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 
 3. Neither the name of the copyright holder nor the names of any
 contributors may be used to endorse or promote products derived
 from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

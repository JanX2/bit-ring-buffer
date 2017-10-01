//
//  bit-ring-buffer.h
//  bit-ring-buffer
//
//  Created by Jan on 2017-10-01.
//
//  Based on the ring buffer implementation from libcork:
//  https://github.com/redjack/libcork
//
//

#ifndef LIBJX_DS_BIT_RING_BUFFER_H
#define LIBJX_DS_BIT_RING_BUFFER_H

#include "bitset.h"


typedef struct jx_bit_ring_buffer {
	/* The elements of the bit ring buffer */
	jx_bitset bitset;
	/* The actual number of elements currently in the ring buffer. */
	size_t  used_bit_count;
	/* The index of the next element to read from the buffer */
	size_t  read_index;
	/* The index of the next element to write into the buffer */
	size_t  write_index;
} jx_bit_ring_buffer;


bool
jx_bit_ring_buffer_init(jx_bit_ring_buffer *buf, size_t bit_count);

jx_bit_ring_buffer *
jx_bit_ring_buffer_new(size_t bit_count);

void
jx_bit_ring_buffer_done(jx_bit_ring_buffer *buf);

void
jx_bit_ring_buffer_free(jx_bit_ring_buffer *buf);


#define jx_bit_ring_buffer_get_allocated_size(buf) (jx_bitset_get_bit_count(&((buf)->bitset)))

#define jx_bit_ring_buffer_is_empty(buf) ((buf)->used_bit_count == 0)
#define jx_bit_ring_buffer_is_full(buf) ((buf)->used_bit_count == jx_bit_ring_buffer_get_allocated_size(buf))

/* Return the number of bits that have been stored to the ring buffer. */
#define jx_bit_ring_buffer_get_used_bit_count(buf) \
	((buf)->used_bit_count)

bool
jx_bit_ring_buffer_add(jx_bit_ring_buffer *buf, bool element);

void
jx_bit_ring_buffer_add_with_overwrite(jx_bit_ring_buffer *buf, bool element);

const bool *
jx_bit_ring_buffer_pop(jx_bit_ring_buffer *buf);

const bool *
jx_bit_ring_buffer_peek(jx_bit_ring_buffer *buf);

/* Return the number of 1-bits in the ring buffer. */
size_t
jx_bit_ring_buffer_population_count(jx_bit_ring_buffer *buf);

#endif /* LIBJX_DS_BIT_RING_BUFFER_H */

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

//
//  bitset.h
//  bit-ring-buffer
//
//  Created by Jan on 2017-09-24.
//
//  Based on the bitset implementation from libcork:
//  https://github.com/redjack/libcork
//
//

#ifndef LIBJX_DS_BITS_H
#define LIBJX_DS_BITS_H

#include <limits.h>
#include <stdbool.h>


/*-----------------------------------------------------------------------
 * Bit sets
 */

#define JX_BITSET_BITS_PER_BYTE	8
#define JX_BITSET_INVERT_BIT_ORDER 1
#define JX_BITSET_USE_INLINE_STORAGE 1

#define JX_BITSET_INLINE_STORAGE_SIZE (sizeof(size_t))
#define JX_BITSET_INLINE_STORAGE_COUNT (JX_BITSET_INLINE_STORAGE_SIZE * JX_BITSET_BITS_PER_BYTE)

typedef struct jx_bitset {
	uint8_t *bits;
#if JX_BITSET_USE_INLINE_STORAGE
	size_t  bits_inline;
#endif
	size_t  bit_count;
	size_t  byte_count;
} jx_bitset;

void
jx_bitset_init(jx_bitset *set, size_t bit_count);

void
jx_bitset_deinit(jx_bitset *set);

jx_bitset *
jx_bitset_new(size_t bit_count);

void
jx_bitset_free(jx_bitset *set);

void
jx_bitset_clear(jx_bitset *set);

#if JX_BITSET_INVERT_BIT_ORDER
void
jx_bitset_set_all_to_true(jx_bitset *set);

void
jx_bitset_set_all(jx_bitset *set, bool val);

void
jx_bitset_shift_all_bits_forward(jx_bitset *set);
#endif

/* Shift every bit to the next index. 
 * The first bit (index 0) is set to `false`.
 * The last bit’s value is lost. */
void
jx_bitset_shift_all_bits_forward_slowest(jx_bitset *set);

/* Return the number of 1-bits in the set. */
int
jx_bitset_popcount(jx_bitset *set);

/* Calculate the offset of the byte for a particular bit within the byte array. */
#define jx_bitset_byte_offset_in_array(i) \
	((i) / JX_BITSET_BITS_PER_BYTE)

/* Calculate the offset for a particular bit within a byte. */
#define jx_bitset_bit_offset_in_byte(i) \
	((i) % JX_BITSET_BITS_PER_BYTE)

/* Extract the byte that contains a particular bit in an array.
 * Bytes are stored in big-endian order with the lowest byte address
 * storing the lowest-numbered bits. */
#define jx_bitset_byte_for_bit(set, i) \
	((set)->bits[jx_bitset_byte_offset_in_array(i)])

#if JX_BITSET_INVERT_BIT_ORDER
#define JX_BITSET_SINGLE_BIT_MASK	0b00000001
#define JX_BITSET_SINGLE_BIT_SHIFT	<<
#else
#define JX_BITSET_SINGLE_BIT_MASK	0b10000000
#define JX_BITSET_SINGLE_BIT_SHIFT	>>
#endif

/* Create a bit mask that extracts a particular bit from the byte that it lives
 * in. */
#define jx_bitset_pos_mask_for_bit(i) \
	(JX_BITSET_SINGLE_BIT_MASK JX_BITSET_SINGLE_BIT_SHIFT jx_bitset_bit_offset_in_byte(i))

/* Create a bit mask that extracts everything except for a particular bit from
 * the byte that it lives in. */
#define jx_bitset_neg_mask_for_bit(i) \
	(~jx_bitset_pos_mask_for_bit(i))

/* Return whether a particular bit is set in a byte array.
 * Bits are numbered from 0 for the least significant bit up to `bit_count`. */
#define jx_bitset_get(set, i) \
	((jx_bitset_byte_for_bit(set, i) & jx_bitset_pos_mask_for_bit(i)) != false)

/* Set (or unset) a particular bit in a byte array.  
 * Bits are numbered from 0 for the least significant bit up to `bit_count`. */
#define jx_bitset_set(set, i, val) \
	(jx_bitset_byte_for_bit(set, i) = \
	 (jx_bitset_byte_for_bit(set, i) & jx_bitset_neg_mask_for_bit(i)) \
	 | ((val)? jx_bitset_pos_mask_for_bit(i): false))

/* Return the number of bits for which storage is reserved in the set. */
#define jx_bitset_get_bit_count(set) \
	((set)->bit_count)

#endif /* LIBJX_DS_BITS_H */

/*
 Copyright 2011-2013, RedJack, LLC.
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

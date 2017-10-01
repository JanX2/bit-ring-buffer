//
//  bitset.m
//  bit-ring-buffer
//
//  Created by Jan on 2017-09-24.
//
//  Based on the bitset implementation from libcork:
//  https://github.com/redjack/libcork
//
//

#include <stdlib.h>
#include <string.h>

#include "bitset.h"


#if JX_BITSET_USE_INLINE_STORAGE
#define jx_bitset_uint8_pointer_for_inline_storage(set) \
	((uint8_t *)&(set->bits_inline))
#endif

#if JX_BITSET_USE_INLINE_STORAGE
#define jx_bitset_uses_inline_storage(set) \
	(set->bits == jx_bitset_uint8_pointer_for_inline_storage(set))
#endif

#define jx_bitset_generic_popcount(X) _Generic((X), \
			char:				__builtin_popcount, \
			unsigned int:		__builtin_popcount, \
			unsigned long:		__builtin_popcountl, \
			unsigned long long:	__builtin_popcountll, \
			default:			__builtin_popcountll \
		)(X)

// NOTE: For alternative ways to calculate popcount, see:
// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
// https://en.wikipedia.org/wiki/Hamming_weight

static size_t
bytes_needed(size_t bit_count)
{
	// We need one byte for every bit…
	size_t bytes_needed = bit_count / JX_BITSET_BITS_PER_BYTE;
	// Plus one extra for the leftovers that don’t fit into a whole byte.
	bytes_needed += ((bit_count % JX_BITSET_BITS_PER_BYTE) > 0);
	
	return bytes_needed;
}

void
jx_bitset_init(jx_bitset *set, size_t bit_count)
{
	set->bit_count = bit_count;
	set->byte_count = bytes_needed(bit_count);
	
#if JX_BITSET_USE_INLINE_STORAGE
	if (bit_count > JX_BITSET_INLINE_STORAGE_COUNT)
#endif
	{
		set->bits = calloc(set->byte_count, sizeof(uint8_t));
		
	}
#if JX_BITSET_USE_INLINE_STORAGE
	else {
		set->bits = jx_bitset_uint8_pointer_for_inline_storage(set);
	}
#endif

	jx_bitset_clear(set);
}

jx_bitset *
jx_bitset_new(size_t bit_count)
{
	jx_bitset *set = malloc(sizeof(jx_bitset));
	
	jx_bitset_init(set, bit_count);
	
	return set;
}

void
jx_bitset_done(jx_bitset *set)
{
#if JX_BITSET_USE_INLINE_STORAGE
	if (!jx_bitset_uses_inline_storage(set))
#endif
	{
		free(set->bits);
	}
}

void
jx_bitset_free(jx_bitset *set)
{
	jx_bitset_done(set);
	free(set);
}

void
jx_bitset_clear(jx_bitset *set)
{
#if JX_BITSET_USE_INLINE_STORAGE
	set->bits_inline = 0;
	
	if (!jx_bitset_uses_inline_storage(set))
#endif
	{
		memset(set->bits, 0, set->byte_count);
	}
}

#if JX_BITSET_INVERT_BIT_ORDER

#define JX_BITSET_BYTE_WITH_ALL_BITS_SET	~((uint8_t)0)
#define JX_BITSET_INLINE_STORAGE_WITH_ALL_BITS_SET	~((size_t)0)

/* “One thing to notice about bitmasks like that is that they are always one less than a power of two.”
 * https://stackoverflow.com/a/1392065/152827 */
#define jx_bitset_get_mask_for_bits_below_power_of_two(val) \
	((val) - 1)

size_t
imaginary_bit_count_pos_mask_for_bit_count(const size_t bit_count)
{
	const size_t imaginary_bit_count_pos_mask = jx_bitset_pos_mask_for_bit(bit_count);
	// As the above is a mask for a single bit, its value is always a power of two.
	
	return imaginary_bit_count_pos_mask;
}

static size_t
bits_inline_mask_for_bit_count(const size_t bit_count)
{
	const size_t imaginary_bit_count_pos_mask = imaginary_bit_count_pos_mask_for_bit_count(bit_count);
	
	const size_t imaginary_byte_offset = jx_bitset_byte_offset_in_array(bit_count) % JX_BITSET_INLINE_STORAGE_SIZE;
	const size_t byte_offset_in_bits = imaginary_byte_offset * JX_BITSET_BITS_PER_BYTE;
	const size_t imaginary_bit_count_mask = imaginary_bit_count_pos_mask << byte_offset_in_bits;
	const size_t bits_inline_mask = jx_bitset_get_mask_for_bits_below_power_of_two(imaginary_bit_count_mask);
	return bits_inline_mask;
}

static uint8_t
last_byte_mask_for_bit_count(const size_t bit_count)
{
	const size_t imaginary_bit_count_pos_mask = imaginary_bit_count_pos_mask_for_bit_count(bit_count);
	
	uint8_t last_byte_mask;

	if (imaginary_bit_count_pos_mask == 1) {
		last_byte_mask = JX_BITSET_BYTE_WITH_ALL_BITS_SET;
	}
	else {
		last_byte_mask = (uint8_t)(jx_bitset_get_mask_for_bits_below_power_of_two(imaginary_bit_count_pos_mask));
	}
	
	return last_byte_mask;
}

void
jx_bitset_set_all_to_true(jx_bitset *set)
{
	const size_t bit_count = jx_bitset_get_bit_count(set);
	
#if JX_BITSET_USE_INLINE_STORAGE
	if (jx_bitset_uses_inline_storage(set)) {
		if (bit_count == JX_BITSET_INLINE_STORAGE_SIZE * JX_BITSET_BITS_PER_BYTE) {
			set->bits_inline = JX_BITSET_INLINE_STORAGE_WITH_ALL_BITS_SET;
		}
		else {
			const size_t bits_inline_mask = bits_inline_mask_for_bit_count(bit_count);
			set->bits_inline = bits_inline_mask;
		}
	}
	else
#endif
	{
		const uint8_t all_bits_true = JX_BITSET_BYTE_WITH_ALL_BITS_SET;
		memset(set->bits, all_bits_true, set->byte_count);
		
		const uint8_t last_byte_mask = last_byte_mask_for_bit_count(bit_count);
		set->bits[set->byte_count - 1] &= last_byte_mask;

	}
}

void
jx_bitset_set_all(jx_bitset *set, bool val)
{
	if (val) {
		jx_bitset_set_all_to_true(set);
	}
	else {
		jx_bitset_clear(set);
	}
}

static void
shift_bits_with_count(uint8_t *start_byte_p, uint8_t *last_byte_p, uint8_t prev_overflow, const size_t bit_count)
{
	const size_t byte_count = (last_byte_p + 1) - start_byte_p;
	
	const size_t last_bit_in_byte = JX_BITSET_BITS_PER_BYTE - 1;
	const uint8_t byte_overflow_mask = jx_bitset_pos_mask_for_bit(last_bit_in_byte);
	
	uint8_t byte_overflow = prev_overflow;
	
	for (size_t i = 0; i < byte_count; i += 1) {
		uint8_t *byte_p = &(start_byte_p[i]);
		uint8_t byte = *byte_p;
		*byte_p <<= 1;
		*byte_p |= byte_overflow;
		
		byte_overflow = (byte & byte_overflow_mask) >> last_bit_in_byte;
	}
	
	if (bit_count % JX_BITSET_BITS_PER_BYTE > 0) {
		const uint8_t last_byte_mask = last_byte_mask_for_bit_count(bit_count);
		*last_byte_p &= last_byte_mask;
	}
}

void
jx_bitset_shift_all_bits_forward_using_bytes(jx_bitset *set)
{
	const size_t bit_count = jx_bitset_get_bit_count(set);
	
	uint8_t *start_byte_p = set->bits;
	uint8_t *last_byte_p = &(set->bits[set->byte_count - 1]);
	
	shift_bits_with_count(start_byte_p, last_byte_p, 0b0, bit_count);
}

void
jx_bitset_shift_all_bits_forward_using_units(jx_bitset *set)
{
	const size_t bit_count = jx_bitset_get_bit_count(set);
	
	const size_t unit_size = sizeof(size_t);
	const size_t bits_per_unit = unit_size * JX_BITSET_BITS_PER_BYTE;
	const size_t last_bit_in_unit = bits_per_unit - 1;
	
	const size_t unit_count = set->byte_count / unit_size;
	const size_t unit_remainder = set->byte_count % unit_size;
	
	const size_t unit_overflow_mask =
	(size_t)0b1 JX_BITSET_SINGLE_BIT_SHIFT last_bit_in_unit;
	
	size_t unit_overflow = 0b0;
	size_t *units = (size_t *)set->bits;
	
	for (size_t i = 0; i < unit_count; i += 1) {
		size_t *unit_p = &(units[i]);
		size_t unit = *unit_p;
		*unit_p <<= 1;
		*unit_p |= unit_overflow;
		
		unit_overflow = (unit & unit_overflow_mask) >> last_bit_in_unit;
	}
	
	if (unit_remainder > 0) {
		uint8_t *start_byte_p = (uint8_t *)&(units[unit_count]);
		uint8_t *last_byte_p = &(set->bits[set->byte_count - 1]);
		
		shift_bits_with_count(start_byte_p, last_byte_p, (uint8_t)unit_overflow, bit_count);
	}
	else if (bit_count % bits_per_unit > 0) {
		size_t *last_unit = &(units[unit_count - 1]);
		const size_t last_unit_mask = bits_inline_mask_for_bit_count(bit_count);
		*last_unit &= last_unit_mask;
	}
}

void
jx_bitset_shift_all_bits_forward(jx_bitset *set)
{
#if JX_BITSET_USE_INLINE_STORAGE
	if (jx_bitset_uses_inline_storage(set)) {
		set->bits_inline <<= 1;
		
		const size_t bit_count = jx_bitset_get_bit_count(set);
		if (bit_count != JX_BITSET_INLINE_STORAGE_SIZE * JX_BITSET_BITS_PER_BYTE) {
			const size_t bits_inline_mask = bits_inline_mask_for_bit_count(set->bit_count);
			set->bits_inline &= bits_inline_mask;
		}
	}
	else
#endif
	{
		jx_bitset_shift_all_bits_forward_using_units(set);
	}
}

#endif

void
jx_bitset_shift_all_bits_forward_slowest(jx_bitset *set)
{
	const size_t bit_count = jx_bitset_get_bit_count(set);
	
	bool previous_bit = false;
	
	for (size_t i = 0; i < bit_count; i += 1) {
		const bool overflow_bit = jx_bitset_get(set, i);
		jx_bitset_set(set, i, previous_bit);
		
		previous_bit = overflow_bit;
	}
}

size_t
jx_bitset_popcount(jx_bitset *set)
{
#if JX_BITSET_USE_INLINE_STORAGE
	if (jx_bitset_uses_inline_storage(set)) {
		return jx_bitset_generic_popcount(set->bits_inline);
	}
	else
#endif
	{
		const size_t unit_size = sizeof(size_t);
		
		const size_t unit_count = set->byte_count / unit_size;
		const size_t unit_remainder = set->byte_count % unit_size;
		
		size_t popcount = 0;
		size_t const *units = (size_t *)set->bits;
		
		for (size_t i = 0; i < unit_count; i += 1) {
			size_t const *unit_p = &(units[i]);
			popcount += jx_bitset_generic_popcount(*unit_p);
		}
		
		if (unit_remainder > 0) {
			uint8_t *start_byte_p = (uint8_t *)&(units[unit_count]);
			for (size_t i = 0; i < unit_remainder; i += 1) {
				uint8_t const *byte_p = &(start_byte_p[i]);
				popcount += jx_bitset_generic_popcount(*byte_p);
			}
		}
		
		return popcount;
	}
}

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

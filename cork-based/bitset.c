/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013-2014, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

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
jx_bitset_deinit(jx_bitset *set)
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
	jx_bitset_deinit(set);
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
#endif

int
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
		
		int popcount = 0;
		size_t const *units = (size_t *)set->bits;
		
		for (size_t i = 0; i < unit_count; i += 1) {
			size_t unit = units[i];
			popcount += jx_bitset_generic_popcount(unit);
		}
		
		if (unit_remainder > 0) {
			uint8_t const *bits_remainder = (uint8_t *)&(units[unit_count]);
			for (size_t i = 0; i < unit_remainder; i += 1) {
				uint8_t byte = bits_remainder[i];
				popcount += jx_bitset_generic_popcount(byte);
			}
		}
		
		return popcount;
	}
}

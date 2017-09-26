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
jx_bitset_init(struct jx_bitset *set, size_t bit_count)
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

struct jx_bitset *
jx_bitset_new(size_t bit_count)
{
	struct jx_bitset *set = malloc(sizeof(struct jx_bitset));
	
	jx_bitset_init(set, bit_count);
	
	return set;
}

void
jx_bitset_deinit(struct jx_bitset *set)
{
#if JX_BITSET_USE_INLINE_STORAGE
	if (!jx_bitset_uses_inline_storage(set))
#endif
	{
		free(set->bits);
	}
}

void
jx_bitset_free(struct jx_bitset *set)
{
	jx_bitset_deinit(set);
	free(set);
}

void
jx_bitset_clear(struct jx_bitset *set)
{
#if JX_BITSET_USE_INLINE_STORAGE
	set->bits_inline = 0;
	
	if (!jx_bitset_uses_inline_storage(set))
#endif
	{
		memset(set->bits, 0, set->byte_count);
	}
}

int
jx_bitset_popcount(struct jx_bitset *set)
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
